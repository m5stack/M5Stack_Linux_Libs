# SConstruct
from pathlib import Path
import os, sys, shutil
import copy
from SCons.Node.FS import File
import subprocess
import parse
from collections.abc import Iterable
import configparser
# from parse import compile as parse_compile
SDK_PATH = os.environ.get('SDK_PATH', str(Path(os.getcwd())/'..'/'..'))
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


    with open(GLOBAL_CONFIG_MK_FILE, 'r') as f:
        pattern = parse.compile("CONFIG_{}={}\n")
        for line in f.readlines():
            Pobj = pattern.parse(line)
            if Pobj:
                key, value = Pobj.fixed
                os.environ['CONFIG_' + key] = value.strip('"')



    if sys.platform.startswith('win'):
        env = Environment(tools=['gcc', 'g++', 'gnulink', 'ar', 'gas', 'as'])
        
        if os.environ['CONFIG_TOOLCHAIN_PATH']:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ';' + env['ENV']['PATH']
        else:
            env['ENV']['PATH'] = env['ENV']['PATH'] + ';' +  os.getenv('Path')
        if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
            if os.environ['CONFIG_TOOLCHAIN_PATH']:
                env['CC'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'gcc.exe')) 
                env['AS'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'as.exe' ))
                env['AR'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'ar.exe' ))
                env['CXX'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"')) / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'))
                env['LINK'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))/ str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'))
                env['PROGSUFFIX'] = ''
            else:
                env['CC'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'gcc.exe' 
                env['AS'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'as.exe' 
                env['AR'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'ar.exe' 
                env['CXX'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'
                env['LINK'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'
                env['PROGSUFFIX'] = ''
        else:
            env['CC'] = 'gcc.exe'
            env['AS'] = 'as.exe'
            env['AR'] = 'ar.exe'
            env['CXX'] = 'g++.exe'
            env['LINK'] = 'g++.exe'
            env['PROGSUFFIX'] = ''
        
            
    elif sys.platform.startswith('linux'):
        env = Environment()
        if os.environ['CONFIG_TOOLCHAIN_PATH']:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ':' + env['ENV']['PATH']
        if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
            CONFIG_TOOLCHAIN_PATH = os.environ['CONFIG_TOOLCHAIN_PREFIX']
            env['CC'] = CONFIG_TOOLCHAIN_PATH + 'gcc'
            env['AS'] = CONFIG_TOOLCHAIN_PATH + 'as'
            env['AR'] = CONFIG_TOOLCHAIN_PATH + 'ar'
            env['CXX'] = CONFIG_TOOLCHAIN_PATH + 'g++'
            env['LINK'] = CONFIG_TOOLCHAIN_PATH + 'g++'
    else:
        print('unknow os!')

    if os.environ['CONFIG_TOOLCHAIN_FLAGS']:
        env.MergeFlags(os.environ['CONFIG_TOOLCHAIN_FLAGS'])


    env.Append(CPPPATH=[BUILD_CONFIG_PATH])
    # env['LINKCOM'] = '$LINK -o $TARGET $SOURCES $LINKFLAGS $__RPATH  $_LIBDIRFLAGS '

    if 'CONFIG_COMMPILE_DEBUG' not in os.environ:
        env['CCCOMSTR'] = "CC $TARGET"
        env['CXXCOMSTR'] = "CXX $TARGET"
        env['SHCCCOMSTR'] = "CC -fPIC $TARGET"
        env['SHCXXCOMSTR'] = "CXX -fPIC $TARGET"
        env['ARCOMSTR'] = "LD $TARGET"
        env['SHLINKCOMSTR'] = "Linking $TARGET"
        env['LINKCOMSTR'] = 'Linking $TARGET'

    env['PROJECT_PATH'] = PROJECT_PATH
    env['PROJECT_NAME'] = os.path.basename(PROJECT_PATH)
    env['PROJECT_TOOL_S'] = str(Path(SDK_PATH)/'tools'/'scons'/'SConstruct_tool.py')

    env['COMPONENTS'] = []
    env['COMPONENTS_ENV'] = env.Clone()
    env['COMPONENTS_PATH'] = [str(Path(SDK_PATH)/'components')]
    # env['COMPONENTS_PATH'] = []


    for component in env['COMPONENTS_PATH']:
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
            ofile = file.replace('/', '_')
            ofile = ofile.replace('\\', '_')
            ofile = ofile.replace(':', '_')
            ofile = str(Path(component_build_dir)/ofile) + '.o'
            return ofile
        _srcs = [str(o) for o in deep_iter_or_return(component['SRCS'])]
        _srco = list(map(get_srcs, _srcs))
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
                _LIBO += list(map(lambda o: str(o), task_lists[requirement]['DYNAMIC_LIB'] + task_lists[requirement]['STATIC_LIB']))
                for c_requirement in task_lists[requirement]['REQUIREMENTS']: 
                    if c_requirement not in task_lists:
                        _BUILD_ENV.Append(LIBS=[c_requirement])
            else:
                _BUILD_ENV.Append(LIBS=[requirement])
        # reduse = lambda value: sorted(set(value),key=value.index)
        # _LIBO = reduse(_LIBO)
        
        if component['REGISTER'] == 'static':
            _OBJS = list(map(lambda file: _BUILD_ENV.Object(target = file[0], source = file[1]), list(zip(_srco, _srcs))))
            # _LIBO = list(map(lambda o: str(o), component['STATIC_LIB']))
            # component['_target'] = _BUILD_ENV.Library(target = _TARGET, source = _OBJS + _LIBO)
            component['_target'] = _BUILD_ENV.Library(target = _TARGET, source = _OBJS)
            component['_target_build_env'] = _BUILD_ENV
        elif component['REGISTER'] == 'shared':
            _OBJS = list(map(lambda file: _BUILD_ENV.SharedObject(target = file[0], source = file[1]), list(zip(_srco, _srcs))))
            # _LIBO = list(map(lambda o: str(o), component['DYNAMIC_LIB']))
            # component['_target'] = _BUILD_ENV.SharedLibrary(target = _TARGET, source = _OBJS + _LIBO)
            component['_target'] = _BUILD_ENV.SharedLibrary(target = _TARGET, source = _OBJS)
            component['_target_build_env'] = _BUILD_ENV
            if 'CONFIG_TOOLCHAIN_SYSTEM_UNIX' in os.environ:
                _BUILD_ENV.Command(os.path.join('dist', f'lib{component["target"]}.so'), str(Path('build')/component['target']/f'lib{component["target"]}.so'), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])
            elif 'CONFIG_TOOLCHAIN_SYSTEM_WIN' in os.environ:
                _BUILD_ENV.Command(os.path.join('dist', f'lib{component["target"]}.dll'), str(Path('build')/component['target']/f'lib{component["target"]}.dll'), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])
            else:
                pass
            
        elif component['REGISTER'] == 'project':
            _OBJS = list(map(lambda file: _BUILD_ENV.Object(target = file[0], source = file[1]), list(zip(_srco, _srcs))))
            _LIBO += list(map(lambda o: str(o), component['DYNAMIC_LIB'] + component['STATIC_LIB']))
            _BUILD_ENV.Library(target = _TARGET, source = _OBJS)
            empty_src_file = str(Path(component_build_dir)/'empty_src_file.cpp')
            open(empty_src_file, 'w').close()
            component['_target'] = _BUILD_ENV.Program(target = _TARGET, source = [empty_src_file, component_build_dir + '/lib' + component['target'] + '.a'] + _LIBO)
            # _BUILD_ENV['LIBS'] = [component['target']] + _BUILD_ENV['LIBS']
            # _BUILD_ENV.Append(LIBPATH=[component_build_dir])
            component['_target_build_env'] = _BUILD_ENV
            if 'CONFIG_TOOLCHAIN_SYSTEM_UNIX' in os.environ:
                 _BUILD_ENV.Command(os.path.join('dist', component['target']), str(Path('build')/component['target']/component['target']), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])
            elif 'CONFIG_TOOLCHAIN_SYSTEM_WIN' in os.environ:
                 _BUILD_ENV.Command(os.path.join('dist', component['target']) + '.exe', str(Path('build')/component['target']/component['target']) + '.exe', action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])
            else:
                pass
           

def add_commpile_Program_requirements():
    for task in task_lists:
        component = task_lists[task]
        for requirement in component['REQUIREMENTS']:
            if requirement in task_lists:
                Depends(component['_target'], task_lists[requirement]['_target'])


build_task_init()
creat_commpile_Program()
add_commpile_Program_requirements()


















