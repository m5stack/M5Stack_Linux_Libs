# SConstruct
from pathlib import Path
import os, sys, shutil
import copy
from SCons.Node.FS import File
import subprocess
import parse
import asyncio
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



env = None
task_lists = {}

def build_task_init():
    global env
    global task_lists
    fun_list = {'menuconfig':menuconfig_fun, 'clean':clean_fun, 'distclean':distclean_fun, 'save':save_fun, 'SET_CROSS':set_tools_fun}
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
        if os.environ.get('CONFIG_TOOLCHAIN_PREFIX') is not None:
            if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
                env['CC'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'gcc.exe')) 
                env['AS'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'as.exe' ))
                env['AR'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))  / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'ar.exe' ))
                env['CXX'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"')) / str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'))
                env['LINK'] = str(Path(os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'))/ str(os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++.exe'))
                env['PROGSUFFIX'] = ''
            if os.environ['CONFIG_TOOLCHAIN_PATH']:
                env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ';' + env['ENV']['PATH']
            
    elif sys.platform.startswith('linux'):
        env = Environment()
        if 'CONFIG_TOOLCHAIN_PREFIX' in os.environ:
            CONFIG_TOOLCHAIN_PATH = os.environ['CONFIG_TOOLCHAIN_PREFIX']
            env['CC'] = CONFIG_TOOLCHAIN_PATH + 'gcc'
            env['AS'] = CONFIG_TOOLCHAIN_PATH + 'as'
            env['AR'] = CONFIG_TOOLCHAIN_PATH + 'ar'
            env['CXX'] = CONFIG_TOOLCHAIN_PATH + 'g++'
            env['LINK'] = CONFIG_TOOLCHAIN_PATH + 'g++'
        if 'CONFIG_TOOLCHAIN_PATH' in os.environ:
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ':' + env['ENV']['PATH']

        # if os.environ.get('CONFIG_TOOLCHAIN_PREFIX') is not None:
        #     if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
        #         CONFIG_TOOLCHAIN_PATH = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"')
        #         env['CC'] = CONFIG_TOOLCHAIN_PATH + 'gcc'
        #         env['AS'] = CONFIG_TOOLCHAIN_PATH + 'as'
        #         env['AR'] = CONFIG_TOOLCHAIN_PATH + 'ar'
        #         env['CXX'] = CONFIG_TOOLCHAIN_PATH + 'g++'
        #         env['LINK'] = CONFIG_TOOLCHAIN_PATH + 'g++'

        #     if os.environ['CONFIG_TOOLCHAIN_PATH']:
        #         env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip(
        #             '"') + ':' + env['ENV']['PATH']
    else:
        print('unknow os!')

    env.Append(CPPPATH=[BUILD_CONFIG_PATH])
    # env['LINKCOM'] = '$LINK -o $TARGET $SOURCES $LINKFLAGS $__RPATH  $_LIBDIRFLAGS '


    env['PROJECT_PATH'] = PROJECT_PATH
    env['PROJECT_NAME'] = os.path.basename(PROJECT_PATH)
    env['PROJECT_TOOL_S'] = str(Path(SDK_PATH)/'tools'/'scons'/'SConstruct_tool.py')

    env['COMPONENTS'] = []
    env['COMPONENTS_ENV'] = env.Clone()
    env['COMPONENTS_PATH'] = [str(Path(SDK_PATH)/'components')]
    # env['COMPONENTS_PATH'] = []


    for component in env['COMPONENTS_PATH']:
        for component_name in os.listdir(component):
            if os.path.exists(str(Path(component)/component_name/'SConscript')):
                env['component_dir'] = str(Path(component)/component_name)
                SConscript(str(Path(component)/component_name/'SConscript'), exports='env')



    for project_dir in os.listdir(PROJECT_PATH):
        if project_dir.startswith("main"):
            env['component_dir'] = str(Path(PROJECT_PATH)/project_dir)
            SConscript(str(Path(PROJECT_PATH)/project_dir/'SConscript'), exports='env')

    
    for iteam in env['COMPONENTS']:
        task_lists[iteam['target']] = iteam

# print(task_lists)



async def _commpile(task):
    global task_lists
    global env
    env['CCCOMSTR'] = "CC $TARGET"
    env['CXXCOMSTR'] = "CXX $TARGET"
    env['ARCOMSTR'] = "LD $TARGET"
    # env['CXXCOMSTR'] = "CXX $TARGET"
    # print(env.Dump())
    for requirement in task['REQUIREMENTS']:
        if requirement in task_lists:
            await task_lists[requirement]['event'].wait()
    
    mkasdad = copy.deepcopy(task['REQUIREMENTS'])
    
    for requirement in mkasdad:
        if requirement in task_lists:
            print(task_lists[task['target']])
            task_lists[task['target']]['PRIVATE_INCLUDE'] += task_lists[requirement]['INCLUDE']
            task_lists[task['target']]['REQUIREMENTS'] += task_lists[requirement]['REQUIREMENTS']
            task_lists[task['target']]['DYNAMIC_LIB'] += task_lists[requirement]['DYNAMIC_LIB']
            task_lists[task['target']]['STATIC_LIB'] += task_lists[requirement]['STATIC_LIB']
            task_lists[task['target']]['LINK_SEARCH_PATH'] += task_lists[requirement]['LINK_SEARCH_PATH']
            task_lists[task['target']]['DEFINITIONS'] += task_lists[requirement]['DEFINITIONS']
            task_lists[task['target']]['LINK_SEARCH_PATH'].append(os.path.dirname(str(task_lists[requirement]['COMPONENT'][0])))

    if not os.path.exists('build'):
        os.mkdir('build')

    build_env = env.Clone()
    OBJS = []
    # print(task_lists[task['target']]['REQUIREMENTS'] + task_lists[task['target']]['STATIC_LIB'] + task_lists[task['target']]['DYNAMIC_LIB'])
    
    _STATIC_LIB = task_lists[task['target']]['STATIC_LIB']
    _LIBS = task_lists[task['target']]['REQUIREMENTS'] + task_lists[task['target']]['DYNAMIC_LIB']
    _LIBPATH = task_lists[task['target']]['LINK_SEARCH_PATH']
    _CPPPATH = task_lists[task['target']]['INCLUDE'] + task_lists[task['target']]['PRIVATE_INCLUDE']
    _LINKFLAGS = task_lists[task['target']]['LDFLAGS']
    _CCFLAGS = task_lists[task['target']]['DEFINITIONS_PRIVATE'] + task_lists[task['target']]['DEFINITIONS']
    
    _LIBS = [str(item) for item in _LIBS]
    _LIBPATH = [str(item) for item in _LIBPATH]
    _CPPPATH = [str(item) for item in _CPPPATH]
    _LINKFLAGS = [str(item) for item in _LINKFLAGS]
    _STATIC_LIB = [str(item) for item in _STATIC_LIB]

    reduse = lambda value: sorted(set(value),key=value.index)
    _LIBS = reduse(_LIBS)
    _LIBPATH = reduse(_LIBPATH)
    _CCFLAGS = reduse(_CCFLAGS)
    _CPPPATH = reduse(_CPPPATH)
    _LINKFLAGS = reduse(_LINKFLAGS)
    _STATIC_LIB = reduse(_STATIC_LIB)
    
    # build_env.Append(CCFLAGS=       '')
    # build_env.Append(CXXFLAGS=      '')
    # build_env.Append(LINKFLAGS=     '')
    # build_env.Append(CPPDEFINES=    '')
    # build_env.Append(CPPPATH=       '')
    # build_env.Append(LIBPATH=       '')
    # build_env.MergeFlags(_CCFLAGS)

    build_env.Append(LIBS=_LIBS)
    build_env.Append(LIBPATH=_LIBPATH)
    build_env.MergeFlags(_CCFLAGS)
    build_env.Append(CPPPATH=_CPPPATH)
    build_env.Append(LINKFLAGS=_LINKFLAGS)


    if task['REGISTER'] == 'project':
        component_dir = 'main_' + task['target']
        component_dir = str(Path('build')/component_dir)
        if not os.path.exists(component_dir):
            os.mkdir(component_dir)
        
        for sfile in task_lists[task['target']]['SRCS']:
            if isinstance(sfile, str) or isinstance(sfile, File):
                file = str(sfile)
            elif isinstance(sfile, list):
                file = str(sfile[0])
            ofile = file.replace('/', '_')
            ofile = ofile.replace('\\', '_')
            ofile = ofile.replace(':', '_')
            
            ofile = str(Path(component_dir)/ofile) + '.o'
            OBJS.append(build_env.Object(target = ofile, source = file))
        
        OBJS += _STATIC_LIB
        # print(build_env.Dump())
        component=build_env.Program(target=str(Path('build')/task['target']), source=OBJS)
        # print(build_env.Dump())
        task_lists[task['target']]['COMPONENT'] = component
        build_env.Command(os.path.join('dist', task['target']), str(Path('build')/task['target']), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])

        def find_file_in_directories(directories, target_file):
            for directory in directories:
                result = search_file_recursive(directory, target_file)
                if result:
                    return result
            return None

        def search_file_recursive(directory, target_file):
            for root, dirs, files in os.walk(directory):
                if target_file in files:
                    return os.path.join(root, target_file)
            return None
        for obj in _LIBS:
            if '.so' in obj:
                result = find_file_in_directories(_LIBPATH, obj)
                if result:
                    build_env.Command(result, str(Path('build')/os.path.basename(result)), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])

    elif task['REGISTER'] == 'static':
        component_dir = task['target']
        component_dir = str(Path('build')/component_dir)
        if not os.path.exists(component_dir):
            os.mkdir(component_dir)
        
        for sfile in task_lists[task['target']]['SRCS']:
            if isinstance(sfile, str) or isinstance(sfile, File):
                file = str(sfile)
            elif isinstance(sfile, list):
                file = str(sfile[0])
            ofile = file.replace('/', '_')
            ofile = ofile.replace('\\', '_')
            ofile = ofile.replace(':', '_')
            
            ofile = str(Path(component_dir)/ofile) + '.o'
            OBJS.append(build_env.Object(target = ofile, source = file))
        component=build_env.Library(target=str(Path('build')/task['target']/task['target']), source=OBJS)

    elif task['REGISTER'] == 'shared':
        component = None
    
    task_lists[task['target']]['COMPONENT'] = component
    task_lists[task['target']]['event'].set()

    requirement_lists = []
    for requirement in mkasdad:
        if requirement in task_lists:
            requirement_lists.append(task_lists[requirement]['COMPONENT'])
    if requirement_lists:
        Depends(component, requirement_lists)

async def main():
    global task_lists

    for iteam in task_lists:
        task_lists[iteam]['event'] = asyncio.Event()

    tasks = []
    for iteam in task_lists:
        tasks.append(_commpile(task_lists[iteam]))

    await asyncio.gather(*tasks)


build_task_init()
asyncio.run(main())



















