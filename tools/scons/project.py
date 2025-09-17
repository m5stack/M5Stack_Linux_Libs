# SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
#
# SPDX-License-Identifier: MIT

# SConstruct
from pathlib import Path
import os
import sys
import shutil
import copy
from SCons.Node.FS import File
import subprocess
import parse
from collections.abc import Iterable
import configparser
import uuid
import logging

# Setup paths and environment variables
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
GIT_REPO_PATH = os.environ.get('GIT_REPO_PATH', str(Path(SDK_PATH)/'github_source'))
GIT_REPO_FILE = os.environ.get('GIT_REPO_FILE', str(Path(GIT_REPO_PATH)/'source-list.sh'))

# Set GIT_REPO_PATH in environment if not already set
if 'GIT_REPO_PATH' not in os.environ:
    os.environ['GIT_REPO_PATH'] = GIT_REPO_PATH

# Global variables
env = None
task_lists = {}

def setup_logging():
    """Configure logging for the build system"""
    logging.basicConfig(
        level=logging.INFO,
        format='%(levelname)s: %(message)s'
    )
    return logging.getLogger('build')

logger = setup_logging()

def ourspawn(sh, escape, cmd, args, e):
    """Custom spawn function for executing commands with long argument lists"""
    filename = str(uuid.uuid4())
    newargs = ' '.join(args[1:])
    cmdline = cmd + " " + newargs
    
    # Handle command line length limit by writing arguments to a file
    if (len(cmdline) > 16 * 1024):
        with open(filename, 'w') as f:
            f.write(' '.join(args[1:]).replace('\\', '/'))
        # Execute with response file
        cmdline = cmd + " @" + filename
    
    # Execute the command
    proc = subprocess.Popen(
        cmdline, 
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE, 
        shell=False, 
        env=e
    )
    
    data, err = proc.communicate()
    rv = proc.wait()
    
    # Output handling function
    def res_output(_output, _s):
        if len(_s):
            if isinstance(_s, str):
                _output(_s)
            elif isinstance(_s, bytes):
                _output(str(_s, 'UTF-8'))
            else:
                _output(str(_s))
    
    # Write output to stdout/stderr
    res_output(sys.stderr.write, err)
    res_output(sys.stdout.write, data)
    
    # Clean up temporary file if created
    if os.path.isfile(filename):
        os.remove(filename)
    
    return rv

def run_kconfig_tool(menuconfig=False, build_type='Debug'):
    """Run the Kconfig tool to generate configuration files"""
    os.makedirs(BUILD_CONFIG_PATH, exist_ok=True)
    
    if not os.path.exists(CONFIG_TOOL_FILE):
        logger.error("Kconfig tool not found: {}".format(CONFIG_TOOL_FILE))
        sys.exit(1)
    
    cmd = [sys.executable, CONFIG_TOOL_FILE, "--kconfig", KCONFIG_FILE]
    cmd.extend(["--defaults", CONFIG_DEFAULT_FILE])
    
    # Environment variables
    cmd.extend([
        "--env", "SDK_PATH={}".format(SDK_PATH),
        "--env", "PROJECT_PATH={}".format(PROJECT_PATH),
        "--env", "BUILD_TYPE={}".format(build_type)
    ])
    
    # Add menuconfig option if requested
    if menuconfig:
        cmd.extend(["--menuconfig", "True"])
    
    # Output files
    cmd.extend([
        "--output", "makefile", GLOBAL_CONFIG_MK_FILE,
        "--output", "header", GLOBAL_CONFIG_H_FILE
    ])
    
    return subprocess.call(cmd)

def copy_file(target, source, env):
    """Copy files or directories from source to target"""
    source_path = str(source[0])
    target_path = str(target[0])
    
    # Create target directory if it doesn't exist
    os.makedirs(os.path.dirname(target_path), exist_ok=True)
    
    # Copy file or directory
    if os.path.isfile(source_path):
        shutil.copy2(source_path, target_path)
    elif os.path.isdir(source_path):
        shutil.copytree(source_path, target_path)

def menuconfig_fun():
    """Run menuconfig and exit"""
    run_kconfig_tool(menuconfig=True)
    sys.exit(0)

def clean_fun():
    """Clean build artifacts and exit"""
    subprocess.call(['scons', '-c'])
    sys.exit(0)

def distclean_fun():
    """Remove all build artifacts and configuration files"""
    paths_to_remove = ['build', 'dist', '.sconsign.dblite', '.config.old', '.config', '.config.mk']
    for path in paths_to_remove:
        try:
            if os.path.isdir(path):
                shutil.rmtree(path)
            else:
                os.remove(path)
        except Exception as e:
            logger.debug("Failed to remove {}: {}".format(path, e))
    sys.exit(0)

def save_fun():
    """Save current configuration to default config file"""
    if os.path.exists(GLOBAL_CONFIG_MK_FILE):
        shutil.copy(GLOBAL_CONFIG_MK_FILE, CONFIG_DEFAULT_FILE)
        logger.info("Configuration saved to {}".format(CONFIG_DEFAULT_FILE))
    else:
        logger.error("Configuration file {} not found".format(GLOBAL_CONFIG_MK_FILE))
    sys.exit(0)

def set_tools_fun():
    """Set toolchain configuration"""
    config_file = GLOBAL_CONFIG_MK_FILE
    
    if not os.path.exists(config_file):
        logger.error("Configuration file {} not found".format(config_file))
        sys.exit(1)
        
    with open(config_file, 'a') as fs:
        fs.write("\n")
        for confs in sys.argv:
            if confs.startswith('CROSS_DIR'):
                fs.write('CONFIG_TOOLCHAIN_PATH="{}"\n'.format(confs.split("=")[1]))
            elif confs.startswith('CROSS'):
                fs.write('CONFIG_TOOLCHAIN_PREFIX="{}"\n'.format(confs.split("=")[1]))
    
    logger.info("Toolchain configuration updated")
    sys.exit(0)

def push_fun():
    """Push files to remote server using SSH"""
    if not os.path.exists('setup.ini'):
        logger.error("Please create setup.ini!")
        sys.exit(1)
    
    config = configparser.ConfigParser()
    config.read(str(Path(PROJECT_PATH)/'setup.ini'))
    
    cmd = [
        sys.executable, 
        str(Path(SDK_PATH)/'tools'/'scons'/'push.py'), 
        config['ssh']['local_file_path'],
        config['ssh']['remote_file_path'],
        config['ssh']['remote_host'],
        config['ssh']['remote_port'],
        config['ssh']['username'],
        config['ssh']['password']
    ]
    
    logger.info("Pushing files to {}".format(config['ssh']['remote_host']))
    subprocess.call(cmd)
    sys.exit(0)

def Fatal(env, message):
    """Fatal error handler for SCons environment"""
    logger.error(message)
    env.Exit(1)

def load_config_mk():
    """Load configuration from global_config.mk file"""
    try:
        with open(GLOBAL_CONFIG_MK_FILE, 'r') as f:
            pattern = parse.compile("CONFIG_{}={}\n")
            for line in f.readlines():
                Pobj = pattern.parse(line)
                if Pobj:
                    key, value = Pobj.fixed
                    key = 'CONFIG_' + key 
                    if key not in os.environ:
                        os.environ[key] = value.strip('"')
    except Exception as e:
        logger.error('Error parsing {}: {}'.format(GLOBAL_CONFIG_MK_FILE, e))
        sys.exit(-1)

def load_git_repos(env):
    """Load git repository information from source-list.sh"""
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
                        
                        env['GIT_REPO_LISTS'][git_repo_name] = {
                            'url': git_repo_url, 
                            'commit': git_repo_commit, 
                            "path": str(Path(SDK_PATH)/'github_source'/git_repo_name)
                        }
                except Exception as e:
                    logger.debug("Failed to parse git repo line: {}".format(e))
    except Exception as e:
        logger.debug("Failed to load git repos: {}".format(e))

def setup_environment():
    """Set up the SCons build environment"""
    global env
    
    # Create sconsign file in build directory
    SConsignFile('build/sconsign.dblite')
    
    # Initialize environment with basic tools
    env = Environment(tools=['gcc', 'g++', 'gnulink', 'ar', 'gas', 'as'])
    
    # Set up basic environment variables
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

    # Set up command strings
    env['CCCOM'] = '$CC -o $TARGET -c $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
    env['CXXCOM'] = '$CXX -o $TARGET -c $CXXFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'
    env['ASCOM'] ='$AS $ASFLAGS -o $TARGET $SOURCES'
    env['ARCOM'] = '$AR $ARFLAGS $TARGET $SOURCES'
    env['SHCCCOM'] = '$SHCC -o $TARGET -c $SHCFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
    env['SHCXXCOM'] = '$SHCXX -o $TARGET -c $SHCXXFLAGS $SHCCFLAGS $_CCCOMCOM $SOURCES'
    env['LINKCOM'] = '$LINK -o $TARGET $SOURCES $LINKFLAGS $__RPATH $_LIBDIRFLAGS $_LIBFLAGS'
    env['SHLINKCOM'] = '$SHLINK -o $TARGET $SHLINKFLAGS $__SHLIBVERSIONFLAGS $__RPATH $SOURCES $_LIBDIRFLAGS $_LIBFLAGS'
    env['STRIPCOM'] = '$STRIP $SOURCES'

    # Set up command string display (for less verbose output)
    if 'CONFIG_COMMPILE_DEBUG' not in os.environ:
        env['ASCOMSTR'] = "AS $SOURCES"
        env['CCCOMSTR'] = "CC $SOURCES"
        env['CXXCOMSTR'] = "CXX $SOURCES"
        env['SHCCCOMSTR'] = "CC -fPIC $SOURCES"
        env['SHCXXCOMSTR'] = "CXX -fPIC $SOURCES"
        env['ARCOMSTR'] = "LD $TARGET"
        env['SHLINKCOMSTR'] = "Linking $TARGET"
        env['LINKCOMSTR'] = 'Linking $TARGET'

    # Add Fatal method to environment
    env.AddMethod(Fatal, "Fatal")

    # Configure toolchain prefix if specified
    if os.environ.get('CONFIG_TOOLCHAIN_PREFIX'):
        env['GCCPREFIX'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"')    

    # Configure PATH based on host OS
    if env['HOST_OS'].startswith('win'):
        if os.environ.get('CONFIG_TOOLCHAIN_PATH'):
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ';' + env['ENV']['PATH'] + ';' + os.getenv('Path')
        else:
            env['ENV']['PATH'] = env['ENV']['PATH'] + ';' + os.getenv('Path')
        env['GCCSUFFIX'] = '.exe'
        env['SPAWN'] = ourspawn
    elif env['HOST_OS'].startswith('posix'):
        if os.environ.get('CONFIG_TOOLCHAIN_PATH'):
            env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"') + ':' + env['ENV']['PATH']
    else:
        logger.warning('Unknown OS: {}'.format(env["HOST_OS"]))
    
    # Set full toolchain prefix path
    if os.environ.get('CONFIG_TOOLCHAIN_PATH') and os.environ.get('CONFIG_TOOLCHAIN_PREFIX'):
        env['GCCPREFIX'] = os.path.join(
            os.environ['CONFIG_TOOLCHAIN_PATH'].strip('"'), 
            os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"')
        )
    
    # Add toolchain flags if specified
    if os.environ.get('CONFIG_TOOLCHAIN_FLAGS'):
        env.MergeFlags(os.environ['CONFIG_TOOLCHAIN_FLAGS'])
    
    # Add build config path to include paths
    env.Append(CPPPATH=[BUILD_CONFIG_PATH])
    
    # Set up compiler tools
    env['CC'] = '${_concat(GCCPREFIX, "gcc", GCCSUFFIX, __env__)}'          
    env['CXX'] = '${_concat(GCCPREFIX, "g++", GCCSUFFIX, __env__)}'     
    env['AR'] = '${_concat(GCCPREFIX, "ar", GCCSUFFIX, __env__)}'      
    env['AS'] = '${_concat(GCCPREFIX, "as", GCCSUFFIX, __env__)}'        
    env['STRIP'] = '${_concat(GCCPREFIX, "strip", GCCSUFFIX, __env__)}'  
    env['OBJCOPY'] = '${_concat(GCCPREFIX, "objcopy", GCCSUFFIX, __env__)}'     
    env['SIZE'] = '${_concat(GCCPREFIX, "size", GCCSUFFIX, __env__)}'  

    # Get GCC version and target information
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
    except Exception as e:
        logger.warning('Failed to obtain GCC parameters: {}'.format(e))

    # Load git repository information
    load_git_repos(env)

    # Set project information
    env['PROJECT_PATH'] = PROJECT_PATH
    env['PROJECT_NAME'] = os.path.basename(PROJECT_PATH)
    env['PROJECT_TOOL_S'] = str(Path(SDK_PATH)/'tools'/'scons'/'SConstruct_tool.py')

    # Initialize components list and environment
    env['COMPONENTS'] = []
    env['COMPONENTS_ENV'] = env.Clone()
    env['COMPONENTS_PATH'] = [str(Path(SDK_PATH)/'components')]
    
    # Add external component paths if specified
    if 'EXT_COMPONENTS_PATH' in os.environ:
        for ecp in os.environ['EXT_COMPONENTS_PATH'].split(':'):
            env['COMPONENTS_PATH'].append(ecp)

def load_components():
    """Load components from component directories and main project"""
    global env
    
    # Load components from component directories
    for component_path in env['COMPONENTS_PATH']:
        if os.path.exists(component_path):
            for component_name in os.listdir(component_path):
                component_scons = str(Path(component_path)/component_name/'SConstruct')
                if os.path.exists(component_scons):
                    env['component_dir'] = str(Path(component_path)/component_name)
                    SConscript(component_scons, exports='env')

    # Load main project components
    for project_dir in os.listdir(PROJECT_PATH):
        if project_dir.startswith("main"):
            main_scons = str(Path(PROJECT_PATH)/project_dir/'SConstruct')
            if os.path.exists(main_scons):
                env['component_dir'] = str(Path(PROJECT_PATH)/project_dir)
                SConscript(main_scons, exports='env')
    
    # Store components in task_lists
    for item in env['COMPONENTS']:
        task_lists[item['target']] = item

def deep_iter_or_return(obj):
    """Recursively iterate through nested iterables"""
    if isinstance(obj, Iterable) and not isinstance(obj, str):
        for item in obj:
            yield from deep_iter_or_return(item)
    else:
        yield obj

def get_object_path(src_file, component_build_dir):
    """Get the object file path for a source file"""
    file = str(src_file)
    if file.startswith(SDK_PATH):
        file = file[len(SDK_PATH) + 1:]
    
    ofile = file
    if os.path.isabs(ofile):
        ofile = os.path.join(component_build_dir, ofile[1:] + env['OBJSUFFIX'])
    else:
        ofile = os.path.join(component_build_dir, ofile + env['OBJSUFFIX'])
    
    return ofile

def create_empty_source_file(component_build_dir):
    """Create an empty source file for components with no sources"""
    empty_src_file = str(Path(component_build_dir)/'empty_src_file.cpp')
    if 'CONFIG_EMPTY_SRC_FILE_C' in os.environ:
        empty_src_file = str(Path(component_build_dir)/'empty_src_file.c')
    with open(empty_src_file, 'w'):
        pass  # Create empty file
    return empty_src_file

def setup_build_environment(component, base_env):
    """Set up build environment for a component"""
    component_build_env = base_env.Clone()
    
    # Add include paths
    component_build_env.Append(CPPPATH=component['INCLUDE'])
    component_build_env.Append(CPPPATH=component['PRIVATE_INCLUDE'])
    
    # Add definitions
    component_build_env.MergeFlags(component['DEFINITIONS'])
    component_build_env.MergeFlags(component['DEFINITIONS_PRIVATE'])
    
    # Add linker flags
    component_build_env.Append(LINKFLAGS=component['LDFLAGS'])
    
    # Add library search paths
    component_build_env.Append(LIBPATH=component['LINK_SEARCH_PATH'])
    
    # Process requirements
    required_libraries = []
    for requirement in component['REQUIREMENTS']:
        if requirement in task_lists:
            req_component = task_lists[requirement]
            
            # Add include paths from requirement
            component_build_env.Append(CPPPATH=req_component['INCLUDE'])
            
            # Add library
            component_build_env.Append(LIBS=[requirement])
            
            # Add library paths
            component_build_env.Append(LIBPATH=str(Path('build')/requirement))
            component_build_env.Append(LIBPATH=req_component['LINK_SEARCH_PATH'])
            
            # Add linker flags
            component_build_env.Append(LINKFLAGS=req_component['LDFLAGS'])
            
            # Add definitions
            component_build_env.MergeFlags(req_component['DEFINITIONS'])
            
            # Add static and dynamic libraries
            required_libraries += list(map(str, req_component['STATIC_LIB'] + req_component['DYNAMIC_LIB']))
            
            # Add transitive requirements
            for c_requirement in req_component['REQUIREMENTS']: 
                if c_requirement not in task_lists:
                    component_build_env.Append(LIBS=[c_requirement])
        else:
            component_build_env.Append(LIBS=[requirement])
    
    return component_build_env, required_libraries

def build_static_library(component, build_env, source_files, object_files, custom_source_files, target_path):
    """Build a static library component"""
    # Create object files
    compiled_objects = list(map(lambda file: build_env.Object(target=file[0], source=file[1]), list(zip(object_files, source_files))))
    
    # Add custom source files
    for src_file in custom_source_files:
        compiled_objects += build_env.Object(
            target=custom_source_files[src_file]['SRCO'], 
            source=str(src_file), 
            CPPFLAGS=custom_source_files[src_file]['CPPFLAGS'], 
            CCFLAGS=custom_source_files[src_file]['CCFLAGS']
        )
    
    # Build library
    component['_target'] = build_env.Library(target=target_path, source=compiled_objects)
    component['_target_build_env'] = build_env

def build_shared_library(component, build_env, source_files, object_files, custom_source_files, target_path):
    """Build a shared library component"""
    # Create shared object files
    compiled_objects = list(map(lambda file: build_env.SharedObject(target=file[0], source=file[1]), list(zip(object_files, source_files))))
    
    # Add custom source files
    for src_file in custom_source_files:
        compiled_objects += build_env.SharedObject(
            target=custom_source_files[src_file]['SRCO'], 
            source=str(src_file), 
            CPPFLAGS=custom_source_files[src_file]['CPPFLAGS'], 
            CCFLAGS=custom_source_files[src_file]['CCFLAGS']
        )
    
    # Build shared library
    component['_target'] = build_env.SharedLibrary(target=target_path, source=compiled_objects)
    component['_target_build_env'] = build_env
    
    # Copy to dist directory based on system type
    if 'CONFIG_TOOLCHAIN_SYSTEM_UNIX' in os.environ:
        build_env.Command(
            os.path.join('dist', 'lib{}.so'.format(component["target"])), 
            str(Path('build')/component['target']/'lib{}.so'.format(component["target"])), 
            action=copy_file
        )
    elif 'CONFIG_TOOLCHAIN_SYSTEM_WIN' in os.environ:
        build_env.Command(
            os.path.join('dist', 'lib{}.dll'.format(component["target"])), 
            str(Path('build')/component['target']/'lib{}.dll'.format(component["target"])), 
            action=copy_file
        )

def build_project(component, build_env, source_files, object_files, custom_source_files, target_path, required_libraries, component_build_dir):
    """Build a project component (executable)"""
    # Create object files
    compiled_objects = list(map(lambda file: build_env.Object(target=file[0], source=file[1]), list(zip(object_files, source_files))))
    
    # Add custom source files
    for src_file in custom_source_files:
        compiled_objects += build_env.Object(
            target=custom_source_files[src_file]['SRCO'], 
            source=str(src_file), 
            CPPFLAGS=custom_source_files[src_file]['CPPFLAGS'], 
            CCFLAGS=custom_source_files[src_file]['CCFLAGS']
        )
    
    # Add static and dynamic libraries
    required_libraries += list(map(str, component['STATIC_LIB'] + component['DYNAMIC_LIB']))
    
    # Build static library from objects
    build_env.Library(target=target_path, source=compiled_objects)
    
    itemized_srcs = []
    if 'ITEMIZED_SRCS' in component:
        itemized_srcs += component['ITEMIZED_SRCS']

    # Create empty source file for linking
    # if len(itemized_srcs) == 0:
    itemized_srcs.append(create_empty_source_file(component_build_dir))
    
    itemized_object_files = [get_object_path(src, component_build_dir) for src in itemized_srcs]
    itemized_objects = list(map(lambda file: build_env.Object(target=file[0], source=file[1]), list(zip(itemized_object_files, itemized_srcs))))

    # Add rpath for dynamic libraries if needed
    if component['DYNAMIC_LIB']:
        build_env.Append(LINKFLAGS=['-Wl,-rpath=./'])
    
    # Build program
    component['_target'] = build_env.Program(
        target=target_path, 
        source=[itemized_objects, '{}/lib{}.a'.format(component_build_dir, component['target'])] + required_libraries
    )
    component['_target_build_env'] = build_env
    
    # Copy to dist directory based on system type
    if 'CONFIG_TOOLCHAIN_SYSTEM_UNIX' in os.environ:
        build_env.Command(
            os.path.join('dist', component['target']), 
            str(Path('build')/component['target']/component['target']), 
            action=copy_file
        )
    elif 'CONFIG_TOOLCHAIN_SYSTEM_WIN' in os.environ:
        build_env.Command(
            os.path.join('dist', component['target']) + '.exe', 
            [str(Path('build')/component['target']/component['target']) + '.exe', 'dist'], 
            action=copy_file
        )
    
    # Copy dynamic libraries to dist directory
    for lib_file in component['DYNAMIC_LIB']:
        build_env.Command(
            os.path.join('dist', os.path.basename(str(lib_file))), 
            str(lib_file), 
            action=copy_file
        )
    
    # Copy static files to dist directory if specified
    if 'STATIC_FILES' in component:
        for file in component['STATIC_FILES']:
            build_env.Command(
                os.path.join('dist', os.path.basename(str(file))), 
                str(file), 
                action=copy_file
            )

def create_compile_program():
    """Create compilation targets for all components"""
    global env, task_lists
    
    # Create build directory if it doesn't exist
    if not os.path.exists('build'):
        os.mkdir('build')
    
    # Create dist directory if it doesn't exist
    if not os.path.exists('dist'):
        os.mkdir('dist')
    
    # Process each component
    for task in task_lists:
        component = task_lists[task]
        
        # Create component build directory
        component_build_dir = str(Path('build')/component['target'])
        component['_component_build_dir'] = component_build_dir
        if not os.path.exists(component_build_dir):
            os.mkdir(component_build_dir)
        
        # Get source files and object files
        source_files = [str(o) for o in deep_iter_or_return(component['SRCS'])]
        object_files = [get_object_path(src, component_build_dir) for src in source_files]
        
        # Process custom source files
        custom_source_files = {}
        if 'SRCS_CUSTOM' in component:
            custom_source_files = component['SRCS_CUSTOM']
            custom_sources_list = list(custom_source_files.keys())
            custom_objects_list = [get_object_path(src, component_build_dir) for src in custom_sources_list]
            
            for index, obj in enumerate(custom_sources_list):
                custom_source_files[obj]['SRCO'] = custom_objects_list[index]
        
        # Create empty source file if no sources
        if len(source_files) == 0:
            empty_src_file = create_empty_source_file(component_build_dir)
            source_files.append(empty_src_file)
            object_files.append(empty_src_file + env['OBJSUFFIX'])
        
        # Set up build environment
        target_path = str(Path(component_build_dir)/component['target'])
        component_build_env, required_libraries = setup_build_environment(component, env)
        
        # Build component based on its type
        if component['REGISTER'] == 'static':
            build_static_library(component, component_build_env, source_files, object_files, custom_source_files, target_path)
        elif component['REGISTER'] == 'shared':
            build_shared_library(component, component_build_env, source_files, object_files, custom_source_files, target_path)
        elif component['REGISTER'] == 'project':
            build_project(component, component_build_env, source_files, object_files, custom_source_files, target_path, required_libraries, component_build_dir)
        else:
            logger.error("Unknown component type: {}".format(component['REGISTER']))
            sys.exit(1)

def add_compile_program_requirements():
    """Add dependencies between components"""
    global task_lists
    
    for task in task_lists:
        component = task_lists[task]
        for requirement in component['REQUIREMENTS']:
            if requirement in task_lists:
                Depends(component['_target'], task_lists[requirement]['_target'])

def build_task_init():
    """Initialize the build task"""
    global env
    
    # Check for special commands
    fun_list = {
        'menuconfig': menuconfig_fun, 
        'clean': clean_fun, 
        'distclean': distclean_fun, 
        'save': save_fun, 
        'SET_CROSS': set_tools_fun, 
        'push': push_fun
    }
    
    for fun_name in fun_list:
        if fun_name in sys.argv:
            fun_list[fun_name]()

    # Generate configuration if it doesn't exist
    if not os.path.exists(GLOBAL_CONFIG_MK_FILE):
        os.makedirs(BUILD_CONFIG_PATH, exist_ok=True)
        run_kconfig_tool(menuconfig=False)

    # Load configuration
    load_config_mk()
    
    # Set up build environment
    setup_environment()
    
    # Load components
    load_components()

# Main execution
build_task_init()
create_compile_program()
add_compile_program_requirements()
env['task_list'] = task_lists
Return('env')