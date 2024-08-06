# SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
#
# SPDX-License-Identifier: MIT

# SConstruct
from pathlib import Path
import os, sys, shutil
import copy
from SCons.Node.FS import File
import subprocess
import parse
from collections.abc import Iterable
import configparser

import subprocess
import uuid

# from parse import compile as parse_compile
SDK_PATH = os.path.normpath(os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..')))
PROJECT_NAME = os.environ.get('PROJECT_NAME', os.path.basename(sys.path[0]))
PROJECT_PATH = os.environ.get('PROJECT_PATH', sys.path[0])
BUILD_PATH = os.environ.get('BUILD_PATH', str(Path(PROJECT_PATH)/'build'))
BUILD_CONFIG_PATH = os.environ.get('BUILD_CONFIG_PATH', str(Path(PROJECT_PATH)/'build'/'config'))
CONFIG_TOOL_FILE = os.environ.get('CONFIG_TOOL_FILE', str(Path(SDK_PATH)/'tools'/'kconfig'/'genconfig.py'))
CONFIG_DEFAULT_FILE = os.environ.get('CONFIG_DEFAULT_FILE', str(Path(PROJECT_PATH)/'config_defaults.mk'))
KCONFIG_FILE = os.environ.get('KCONFIG_FILE', str(Path(SDK_PATH)/'Kconfig'))
GLOBAL_CONFIG_MK_FILE = os.environ.get('GLOBAL_CONFIG_MK_FILE', str(Path(PROJECT_PATH)/'build'/'config'/'global_config.mk'))
GLOBAL_CONFIG_H_FILE = os.environ.get('GLOBAL_CONFIG_H_FILE', str(Path(PROJECT_PATH)/'build'/'config'/'global_config.h'))
TOOL_FILE = os.environ.get('TOOL_FILE', str(Path(SDK_PATH)/'tools'))
GIT_REPO_FILE = os.environ.get('GIT_REPO_FILE', str(Path(SDK_PATH)/'github_source'/'source-list.sh'))

def ourspawn(sh, escape, cmd, args, e):
    filename = str(uuid.uuid4())
    newargs = ' '.join(args[1:])
    cmdline = cmd + " " + newargs
    if (len(cmdline) > 16 * 1024):
        f = open(filename, 'w')
        f.write(' '.join(args[1:]).replace('\\', '/'))
        f.close()
        # exec
        cmdline = cmd + " @" + filename
    proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, shell = False, env = e)
    data, err = proc.communicate()
    rv = proc.wait()
    def res_output(_output, _s):
        if len(_s):
            if isinstance(_s, str):
                _output(_s)
            elif isinstance(_s, bytes):
                _output(str(_s, 'UTF-8'))
            else:
                _output(str(_s))
    res_output(sys.stderr.write, err)
    res_output(sys.stdout.write, data)
    if os.path.isfile(filename):
        os.remove(filename)
    return rv


def menuconfig_fun():
    os.makedirs(BUILD_CONFIG_PATH, exist_ok=True)

    build_type = 'Debug'
    if not os.path.exists(CONFIG_TOOL_FILE):
        print("[ERROR] kconfig tool not found:", CONFIG_TOOL_FILE)
        exit(1)
    # get default files
    # config_files = get_config_files(project_args.config_file, sdk_path, project_path)
    cmd = [sys.executable, CONFIG_TOOL_FILE, "--kconfig", KCONFIG_FILE]
    for path in [CONFIG_DEFAULT_FILE]:
        cmd.extend(["--defaults", path])
    cmd.extend(["--menuconfig", "True", "--env", "SDK_PATH={}".format(SDK_PATH),
                                        "--env", "PROJECT_PATH={}".format(PROJECT_PATH),
                                        "--env", "BUILD_TYPE={}".format(build_type)])
    cmd.extend(["--output", "makefile", GLOBAL_CONFIG_MK_FILE])
    cmd.extend(["--output", "header", GLOBAL_CONFIG_H_FILE])
    subprocess.call(cmd)
    exit(0)

def clean_fun():
    subprocess.call(['scons', '-c'])
    exit(0)

def distclean_fun():
    paths_to_remove = ['build', 'dist', '.sconsign.dblite', '.config.old', '.config', '.config.mk']
    for path in paths_to_remove:
        try:
            if os.path.isdir(path):
                shutil.rmtree(path)
            else:
                os.remove(path)
        except :
            pass
    exit(0)
def save_fun():
    if os.path.exists(GLOBAL_CONFIG_MK_FILE):
        shutil.copy(GLOBAL_CONFIG_MK_FILE, CONFIG_DEFAULT_FILE)
    exit(0)

def set_tools_fun():
    config_file = GLOBAL_CONFIG_MK_FILE
    for confs in sys.argv:
        if confs.startswith('CROSS_DIR'):
            with open(config_file, 'a') as fs:
                fs.write("\n")
                fs.write(f'CONFIG_TOOLCHAIN_PATH="{confs.split("=")[1]}"\n')
        elif confs.startswith('CROSS'):
            with open(config_file, 'a') as fs:
                fs.write("\n")
                fs.write(f'CONFIG_TOOLCHAIN_PREFIX="{confs.split("=")[1]}"\n')
    exit(0)

def push_fun():
    if not os.path.exists('setup.ini'):
        print("please creat setup.ini!")
        exit(1)
    config = configparser.ConfigParser()
    config.read(str(Path(PROJECT_PATH)/'setup.ini'))
    cmd = [sys.executable, str(Path(SDK_PATH)/'tools'/'scons'/'push.py'), config['ssh']['local_file_path']
                                                                        , config['ssh']['remote_file_path']
                                                                        , config['ssh']['remote_host']
                                                                        , config['ssh']['remote_port']
                                                                        , config['ssh']['username']
                                                                        , config['ssh']['password']]
    print(cmd)
    subprocess.call(cmd)
    exit(0)


env = None
task_lists = {}

def Fatal(env, message):
    print("Fatal error: " + message)
    env.Exit(1)

def copy_file(target, source, env):
    source_path = str(source[0])
    target_path = str(target[0])
    os.makedirs(os.path.dirname(target_path), exist_ok=True)
    if os.path.isfile(source_path):
        shutil.copy2(source_path, target_path)
    elif os.path.isdir(source_path):
        shutil.copytree(source_path, target_path)
    

def build_task_init():
    global env
    global task_lists
    fun_list = {'menuconfig':menuconfig_fun, 'clean':clean_fun, 'distclean':distclean_fun, 'save':save_fun, 'SET_CROSS':set_tools_fun, 'push':push_fun}
    for fun_name in fun_list:
        if fun_name in sys.argv:
            fun_list[fun_name]()


    if not os.path.exists(GLOBAL_CONFIG_MK_FILE):
        os.makedirs(BUILD_CONFIG_PATH, exist_ok=True)
        build_type = 'Debug'
        tool_path = CONFIG_TOOL_FILE
        if not os.path.exists(tool_path):
            print("[ERROR] kconfig tool not found:", tool_path)
            exit(1)
        cmd = [sys.executable, tool_path, "--kconfig", KCONFIG_FILE]
        for path in [CONFIG_DEFAULT_FILE]:
            cmd.extend(["--defaults", path])
        cmd.extend(["--env", f'SDK_PATH={SDK_PATH}'])
        cmd.extend(["--env", f'PROJECT_PATH={PROJECT_PATH}'])
        cmd.extend(["--env", f'BUILD_TYPE={build_type}'])
        cmd.extend(["--output", "makefile", GLOBAL_CONFIG_MK_FILE])
        cmd.extend(["--output", "header", GLOBAL_CONFIG_H_FILE])
        subprocess.call(cmd)


    try:
        with open(GLOBAL_CONFIG_MK_FILE, 'r') as f:
            pattern = parse.compile("CONFIG_{}={}\n")
            for line in f.readlines():
                Pobj = pattern.parse(line)
                if Pobj:
                    key, value = Pobj.fixed
                    key = 'CONFIG_' + key 
                    if key in os.environ:
                        continue
                    os.environ[key] = value.strip('"')

    except:
        print('Error in parsing the {}'.format(GLOBAL_CONFIG_MK_FILE))
        exit(-1)
    
    SConsignFile('build/sconsign.dblite')
    env = Environment(tools=['gcc', 'g++', 'gnulink', 'ar', 'gas', 'as'])
    env['GCCPREFIX'] = ''
    env['GCCSUFFIX'] = ''
    env['LINK'] = '$SMARTLINK'
    env['SHLINK'] = '$LINK'
    env['OBJPREFIX'] = ''
    env['OBJSUFFIX'] = '.o'
    env['LIBPREFIX'] = 'lib'
    env['LIBSUFFIX'] = '.a'
    env['SHLIBPREFIX'] = '$LIBPREFIX'
    env['SHLIBSUFFIX'] = '.so'
    env['INCPREFIX'] = '-I'
    env['INCSUFFIX'] = ''
    env['LIBDIRPREFIX'] = '-L'
    env['LIBDIRSUFFIX'] = ''
    env['LIBLINKPREFIX'] = '-l'
    env['LIBLINKSUFFIX'] = ''
    env['CPPDEFPREFIX'] = '-D'
    env['CPPDEFSUFFIX'] = ''
    env['PROGSUFFIX'] = ''
    env['ARFLAGS'] = ['rc']
    env['SHCCFLAGS'] = ['$CCFLAGS', '-fPIC']
    env['SHLINKFLAGS'] = ['$LINKFLAGS', '-shared']
    env['CFLAGS'] = []
    env['CXXFLAGS'] = []
    env['ASFLAGS'] = []
    env['LINKFLAGS'] = []

    env['CCCOM'] = '$CC -o $TARGET -c $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
    env['CXXCOM'] = '$CXX -o $TARGET -c $CXXFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
    env['ASCOM'] ='$AS $ASFLAGS -o $TARGET $SOURCES'
    env['ARCOM'] = '$AR $ARFLAGS $TARGET $SOURCES'
    env['SHCCCOM'] = '$SHCC -o $TARGET -c $SHCFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
    env['SHCXXCOM'] = '$SHCXX -o $TARGET -c $SHCXXFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
    env['LINKCOM'] = '$LINK -o $TARGET $SOURCES $LINKFLAGS $__RPATH  $_LIBDIRFLAGS $_LIBFLAGS'
    env['SHLINKCOM'] = '$SHLINK -o $TARGET $SHLINKFLAGS $__SHLIBVERSIONFLAGS $__RPATH $SOURCES $_LIBDIRFLAGS $_LIBFLAGS'
    env['STRIPCOM'] = '$STRIP $SOURCES'

    if 'CONFIG_COMMPILE_DEBUG' not in os.environ:
        env['CCCOMSTR'] = "CC $SOURCES"
        env['CXXCOMSTR'] = "CXX $SOURCES"
        env['SHCCCOMSTR'] = "CC -fPIC $SOURCES"
        env['SHCXXCOMSTR'] = "CXX -fPIC $SOURCES"
        env['ARCOMSTR'] = "LD $TARGET"
        env['SHLINKCOMSTR'] = "Linking $TARGET"
        env['LINKCOMSTR'] = 'Linking $TARGET'

    env.AddMethod(Fatal, "Fatal")

    if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
        env['GCCPREFIX'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"')    

    if env['HOST_OS'].startswith('win'):
        if os.environ['CONFIG_TOOLCHAIN_PATH']:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ';' + env['ENV']['PATH'] + ';' + os.getenv('Path')
        else:
            env['ENV']['PATH'] = env['ENV']['PATH'] + ';' +  os.getenv('Path')
        env['GCCSUFFIX'] = '.exe'
        env['SPAWN'] = ourspawn
    elif env['HOST_OS'].startswith('posix'):
        if os.environ['CONFIG_TOOLCHAIN_PATH']:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ':' + env['ENV']['PATH']
    else:
        print('unknow os!', env['HOST_OS'])
    
    env['GCCPREFIX'] = os.path.join(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'), os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"'))
    if os.environ['CONFIG_TOOLCHAIN_FLAGS']:
        env.MergeFlags(os.environ['CONFIG_TOOLCHAIN_FLAGS'])
    env.Append(CPPPATH=[BUILD_CONFIG_PATH])
    env['CC']    = '${_concat(GCCPREFIX, "gcc", GCCSUFFIX, __env__)}'          
    env['CXX']   = '${_concat(GCCPREFIX, "g++", GCCSUFFIX, __env__)}'     
    env['AR']    = '${_concat(GCCPREFIX, "ar", GCCSUFFIX, __env__)}'      
    env['AS']    = '${_concat(GCCPREFIX, "as", GCCSUFFIX, __env__)}'        
    env['STRIP'] = '${_concat(GCCPREFIX, "strip", GCCSUFFIX, __env__)}'     

    try:
        env.ParseConfig('${CC} -v 2> gcc_out.txt')
        version_r = parse.compile("gcc version {} {}")
        Target_r = parse.compile("Target: {}\n")
        with open('gcc_out.txt', 'r') as conf_file:
            for line in conf_file.readlines():
                version = version_r.parse(line)
                if version:
                    env['CCVERSION'] = version[0]
                Target = Target_r.parse(line)
                if Target:
                    env['GCC_DUMPMACHINE'] = Target[0]
        os.remove('gcc_out.txt')
    except:
        print('Failed to obtain GCC parameters!')

    env['GIT_REPO_LISTS'] = {}
    try:
        with open(GIT_REPO_FILE, 'r') as f:
            pattern = parse.compile("{start}_clone_and_checkout_commit {url} {commit}")
            pattern_name = parse.compile("{}://{}/{}/{}.git")
            for line in f.readlines():
                try:
                    Pobj = pattern.parse(line)
                    if Pobj and '#' not in Pobj.named['start']:
                        git_repo_url = Pobj.named['url'].replace(" ", "")
                        git_repo_commit = Pobj.named['commit'].replace(" ", "").replace("\n", "").replace("\r", "")
                        git_repo_name = pattern_name.parse(git_repo_url)[3]
                        env['GIT_REPO_LISTS'][git_repo_name] = {'url':git_repo_url, 'commit':git_repo_commit, "path":str(Path(SDK_PATH)/'github_source'/git_repo_name)}
                except:
                    pass       
    except:
        pass

    # print(env.Dump())

    env['PROJECT_PATH'] = PROJECT_PATH
    env['PROJECT_NAME'] = os.path.basename(PROJECT_PATH)
    env['PROJECT_TOOL_S'] = str(Path(SDK_PATH)/'tools'/'scons'/'SConstruct_tool.py')

    env['COMPONENTS'] = []
    env['COMPONENTS_ENV'] = env.Clone()
    env['COMPONENTS_PATH'] = [str(Path(SDK_PATH)/'components')]
    if 'EXT_COMPONENTS_PATH' in os.environ:
        for ecp in os.environ['EXT_COMPONENTS_PATH'].split(':'):
            env['COMPONENTS_PATH'].append(ecp)

    for component in env['COMPONENTS_PATH']:
        if os.path.exists(component):
            for component_name in os.listdir(component):
                if os.path.exists(str(Path(component)/component_name/'SConstruct')):
                    env['component_dir'] = str(Path(component)/component_name)
                    SConscript(str(Path(component)/component_name/'SConstruct'), exports='env')

    for project_dir in os.listdir(PROJECT_PATH):
        if project_dir.startswith("main"):
            env['component_dir'] = str(Path(PROJECT_PATH)/project_dir)
            SConscript(str(Path(PROJECT_PATH)/project_dir/'SConstruct'), exports='env')
    
    for iteam in env['COMPONENTS']:
        task_lists[iteam['target']] = iteam

def creat_commpile_Program():
    if not os.path.exists('build'):
        os.mkdir('build')
    for task in task_lists:
        component = task_lists[task]
        _BUILD_ENV = env.Clone()
        component_build_dir = str(Path('build')/component['target'])
        _TARGET = str(Path(component_build_dir)/component['target'])
        component['_component_build_dir'] = component_build_dir
        if not os.path.exists(component_build_dir):
            os.mkdir(component_build_dir)
        def deep_iter_or_return(obj):
            if isinstance(obj, Iterable) and not isinstance(obj, str):
                for item in obj:
                    yield from deep_iter_or_return(item)
            else:
                yield obj
        def get_srcs(obj):
            file = str(obj)
            if file.startswith(SDK_PATH):
                file = file[len(SDK_PATH) + 1:]
            ofile = file
            ofile = os.path.join(component_build_dir, ofile + '.o')
            return ofile
        _srcs = [str(o) for o in deep_iter_or_return(component['SRCS'])]
        _srco = list(map(get_srcs, _srcs))
        _srcs_custom = {}
        if 'SRCS_CUSTOM' in component:
            _srcs_custom = component['SRCS_CUSTOM']
            _srcs_list = list(_srcs_custom.keys())
            _srco_custom = list(map(get_srcs, _srcs_list))
            for index, obj in enumerate(_srcs_list):
                _srcs_custom[obj]['SRCO'] = _srco_custom[index]

        if len(_srcs) == 0:
            empty_src_file = str(Path(component_build_dir)/'empty_src_file.cpp')
            open(empty_src_file, 'w').close()
            _srcs.append(empty_src_file)
            _srco.append(empty_src_file + '.o')
        _BUILD_ENV.Append(CPPPATH=component['INCLUDE'])
        _BUILD_ENV.Append(CPPPATH=component['PRIVATE_INCLUDE'])

        _BUILD_ENV.MergeFlags(component['DEFINITIONS'])
        _BUILD_ENV.MergeFlags(component['DEFINITIONS_PRIVATE'])

        _BUILD_ENV.Append(LINKFLAGS=component['LDFLAGS'])

        _BUILD_ENV.Append(LIBPATH=component['LINK_SEARCH_PATH'])
        _LIBO = []

        for requirement in component['REQUIREMENTS']:
            if requirement in task_lists:
                _BUILD_ENV.Append(CPPPATH=task_lists[requirement]['INCLUDE'])
                _BUILD_ENV.Append(LIBS=[requirement])
                _BUILD_ENV.Append(LIBPATH=str(Path('build')/requirement))
                _BUILD_ENV.Append(LIBPATH=task_lists[requirement]['LINK_SEARCH_PATH'])
                _BUILD_ENV.Append(LINKFLAGS=task_lists[requirement]['LDFLAGS'])
                _BUILD_ENV.MergeFlags(task_lists[requirement]['DEFINITIONS'])
                _LIBO += list(map(lambda o: str(o), task_lists[requirement]['STATIC_LIB'] + task_lists[requirement]['DYNAMIC_LIB']))
                for c_requirement in task_lists[requirement]['REQUIREMENTS']: 
                    if c_requirement not in task_lists:
                        _BUILD_ENV.Append(LIBS=[c_requirement])
            else:
                _BUILD_ENV.Append(LIBS=[requirement])
        
        if component['REGISTER'] == 'static':
            _OBJS = list(map(lambda file: _BUILD_ENV.Object(target = file[0], source = file[1]), list(zip(_srco, _srcs))))
            for src_file in _srcs_custom:
                _OBJS += _BUILD_ENV.Object(target = _srcs_custom[src_file]['SRCO'], source = str(src_file), CPPFLAGS=_srcs_custom[src_file]['CPPFLAGS'], CCFLAGS=_srcs_custom[src_file]['CCFLAGS'])
            component['_target'] = _BUILD_ENV.Library(target = _TARGET, source = _OBJS)
            component['_target_build_env'] = _BUILD_ENV
        elif component['REGISTER'] == 'shared':
            _OBJS = list(map(lambda file: _BUILD_ENV.SharedObject(target = file[0], source = file[1]), list(zip(_srco, _srcs))))
            for src_file in _srcs_custom:
                _OBJS += _BUILD_ENV.SharedObject(target = _srcs_custom[src_file]['SRCO'], source = str(src_file), CPPFLAGS=_srcs_custom[src_file]['CPPFLAGS'], CCFLAGS=_srcs_custom[src_file]['CCFLAGS'])
            component['_target'] = _BUILD_ENV.SharedLibrary(target = _TARGET, source = _OBJS)
            component['_target_build_env'] = _BUILD_ENV
            if 'CONFIG_TOOLCHAIN_SYSTEM_UNIX' in os.environ:
                _BUILD_ENV.Command(os.path.join('dist', f'lib{component["target"]}.so'), str(Path('build')/component['target']/f'lib{component["target"]}.so'), action=copy_file)
            elif 'CONFIG_TOOLCHAIN_SYSTEM_WIN' in os.environ:
                _BUILD_ENV.Command(os.path.join('dist', f'lib{component["target"]}.dll'), str(Path('build')/component['target']/f'lib{component["target"]}.dll'), action=copy_file)
            else:
                pass
            
        elif component['REGISTER'] == 'project':
            _OBJS = list(map(lambda file: _BUILD_ENV.Object(target = file[0], source = file[1]), list(zip(_srco, _srcs))))
            for src_file in _srcs_custom:
                _OBJS += _BUILD_ENV.Object(target = _srcs_custom[src_file]['SRCO'], source = str(src_file), CPPFLAGS=_srcs_custom[src_file]['CPPFLAGS'], CCFLAGS=_srcs_custom[src_file]['CCFLAGS'])
            _LIBO += list(map(lambda o: str(o), component['STATIC_LIB'] + component['DYNAMIC_LIB']))
            _BUILD_ENV.Library(target = _TARGET, source = _OBJS)
            empty_src_file = str(Path(component_build_dir)/'empty_src_file.cpp')
            open(empty_src_file, 'w').close()
            if component['DYNAMIC_LIB']:
                _BUILD_ENV.Append(LINKFLAGS=['-Wl,-rpath=./'])
            component['_target'] = _BUILD_ENV.Program(target = _TARGET, source = [empty_src_file, component_build_dir + '/lib' + component['target'] + '.a'] + _LIBO)
            component['_target_build_env'] = _BUILD_ENV
            if 'CONFIG_TOOLCHAIN_SYSTEM_UNIX' in os.environ:
                _BUILD_ENV.Command(os.path.join('dist', component['target']), str(Path('build')/component['target']/component['target']), action=copy_file)
            elif 'CONFIG_TOOLCHAIN_SYSTEM_WIN' in os.environ:
                _BUILD_ENV.Command(os.path.join('dist', component['target']) + '.exe', [str(Path('build')/component['target']/component['target']) + '.exe', 'dist'], action=copy_file)
            else:
                pass
            for lib_file in component['DYNAMIC_LIB']:
                _BUILD_ENV.Command(os.path.join('dist', os.path.basename(str(lib_file))), str(lib_file), action=copy_file)
            if 'STATIC_FILES' in component:
                for file in component['STATIC_FILES']:
                    _BUILD_ENV.Command(os.path.join('dist', os.path.basename(str(file))), str(file), action=copy_file)


def add_commpile_Program_requirements():
    for task in task_lists:
        component = task_lists[task]
        for requirement in component['REQUIREMENTS']:
            if requirement in task_lists:
                Depends(component['_target'], task_lists[requirement]['_target'])


build_task_init()
creat_commpile_Program()
add_commpile_Program_requirements()


















