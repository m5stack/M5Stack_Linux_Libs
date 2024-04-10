# import os

# SDK_ENV_NAME = 'DIANJIXZ_LIB'
# SDK_ENV_PATH = '/home/nihao/work/dianjixz-lib'
# SDK_ENV_FLAGS = ''
# SDK_ENV_C_FLAGS = ''
# SDK_ENV_CXX_FLAGS = ''
# SDK_ENV_LINK_FLAGS = ''

# CONFIG_TOOLCHAIN_PATH = ''
# CONFIG_TOOLCHAIN_PREFIX = ''

# # CONFIG_TOOLCHAIN_PATH = '/opt/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin'
# # CONFIG_TOOLCHAIN_PREFIX = 'arm-linux-gnueabihf-'
# CONFIG_FLAGS = ''
# CONFIG_LINK_FLAGS = ''
# CONFIG_C_FLAGS = ''
# CONFIG_CXX_FLAGS = ''
# COMPONENT_ADD_INCLUDE = ''
# COMPONENT_ADD_PRIVATE_INCLUDE = ''
# COMPONENT_ADD_SRCS = ''
# COMPONENT_ADD_ASM_SRCS = ''
# COMPONENT_ADD_DEFINITIONS = ''
# COMPONENT_ADD_DEFINITIONS_PRIVATE = ''
# COMPONENT_ADD_REQUIREMENTS = ''
# COMPONENT_ADD_LINKOPTIONS_PRIVATE = ''
# COMPONENT_ADD_STATIC_LIB = ''
# COMPONENT_ADD_DYNAMIC_LIB = ''
# COMPONENT_ADD_C_FLAGS = ''
# COMPONENT_ADD_CXX_FLAGS = ''
# COMPONENT_ADD_C_LINK_FLAGS = ''
# COMPONENT_ADD_CXX_LINK_FLAGS = ''

# COMPONENT_C_FLAGS = ''
# COMPONENT_CXX_FLAGS = ''
# COMPONENT_C_LINK_FLAGS = ''
# COMPONENT_CXX_LINK_FLAGS = ''

# SCONS_PROJECT_NAME = 'scons_examples'
# SCONS_PROJECT_PATH = '/home/nihao/work/dianjixz-lib/examples/scons_examples'
# SCONS_FLAGS = ''
# SCONS_C_FLAGS = ''
# SCONS_CXX_FLAGS = ''
# SCONS_LINK_FLAGS = ''

# # env.Append(LIBS=['c'])

# # env.Object()
# PROJECT_OBJS = []
# # for srcf in COMPONENT_ADD_SRCS:
# #     PROJECT_OBJS.append(env.Object(srcf))

# PROJECT_OBJS.append(env.Object('build/main/src/main.cc.o', 'main/src/main.c'))
# PROJECT_OBJS.append(env.Object('build/main/src/haha.cxx.o', 'main/src/haha.cpp'))

# PROJECT_TARGET = os.path.join(SCONS_PROJECT_PATH, 'build', SCONS_PROJECT_NAME)
# env.Program(target = PROJECT_TARGET, source=PROJECT_OBJS)


# Glob(dir + '/' + file_ext)


# # GCC_ARM_PATH = 'D:/env/gcc-arm-none-eabi-4_8-2014q3-20140805-win32'

# # #PLF = 'arm'
# # PLF = ''

# # # toolchains
# # if PLF == 'arm':
# #     PREFIX = GCC_ARM_PATH + '/bin/arm-none-eabi-'
# # else:
# #     PREFIX = ''

# # CC = PREFIX + 'gcc'
# # AS = PREFIX + 'as'
# # AR = PREFIX + 'ar'
# # CXX = PREFIX + 'g++'
# # LINK = PREFIX + 'ld'

# # SIZE = PREFIX + 'size'
# # OBJDUMP = PREFIX + 'objdump'
# # OBJCPY = PREFIX + 'objcopy'


# # TARGET_PATH = 'build/'
# # TARGET_NAME = 'main'
# # TARGET_WITHOUT_SUFFIX = TARGET_PATH + TARGET_NAME


# # ###################################################################################
# # # C source dirs config
# # C_DIRS = []
# # #C_DIRS.append('src')

# # # C source files config
# # C_FILES = []
# # #C_FILES.append('main.c')

# # # Create c sources list
# # C_SRC_LIST = get_path_files(C_DIRS, '*.c') + C_FILES


# # ###################################################################################
# # # ASM source dirs config
# # AS_DIRS = []
# # #AS_DIRS.append('src')

# # # ASM source files config
# # AS_FILES = []
# # #AS_FILES.append('startup.s')

# # AS_SRC_LIST = get_path_files(AS_DIRS, '*.s') + AS_FILES


# # ###################################################################################
# # # -I, Include path config
# # CPP_PATH = []
# # #CPP_PATH.append('inc')

# # # -D, Preprocess Define
# # CPP_DEFINES = []
# # #CPP_DEFINES.append('CFG_TEST')

# # # C generate define
# # C_FLAGS = []
# # C_FLAGS.append('-O1')
# # C_FLAGS.append('-g')
# # C_FLAGS.append('-std=c99')

# # # C and C++ generate define
# # CC_FLAGS = []
# # CC_FLAGS.append('-Wall')


# # # ASM generate define
# # AS_FLAGS = []
# # AS_FLAGS.append('-g')


# # # Link Config
# # LINK_FLAGS = []
# # #LINK_FLAGS.append('-Wl,–gc-sections')


# # # lib path.
# # LIB_PATH = []
# # #LIB_PATH.append('lib')


# # # .lib, .a file
# # LIBS_FILES = []
# # #LIBS_FILES.append('test')

# # # spec ld flag. Arm spec.
# # SPEC_LD_FLAGS = []
# # if PLF == 'arm':
# #     SPEC_LD_FLAGS.append('-Map')
# #     SPEC_LD_FLAGS.append(TARGET_WITHOUT_SUFFIX + '.map')
# #     SPEC_LD_FLAGS.append('-T' + 'src/map_ram.txt')


# # env = Environment()

# # ###################################################################################
# # # Step0: toolchains setting.
# # if PLF == 'arm':
# #     env['CC'] = CC
# #     env['AS'] = AS
# #     env['AR'] = AR
# #     env['CXX'] = CXX
# #     env['LINK'] = LINK

# #     env['OBJSUFFIX'] = '.o'
# #     env['LIBPREFIX'] = 'lib'
# #     env['LIBSUFFIX'] = '.a'
# #     env['PROGSUFFIX'] = '.elf'

# # ###################################################################################
# # # Step1: C compile setting. use <print(env.Dump())> for details.
# # # 'CCCOM': '$CC -o $TARGET -c $CFLAGS $CCFLAGS $_CCCOMCOM $SOURCES'

# # # Step1.0: General options, like: optim/debug setting. $CFLAGS.
# # env.Append(CFLAGS=C_FLAGS)

# # # Step1.1: General options, other setting. $CCFLAGS.
# # env.Append(CCFLAGS=CC_FLAGS)

# # # Step1.2: -D, -I, setting. $_CCCOMCOM
# # # '_CCCOMCOM': '$CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS'
# # # StepX.2.0: CPPFLAGS setting. --- do nothing.
# # # StepX.2.1: -D setting.
# # # 'CPPDEFPREFIX': '-D'
# # # '_CPPDEFFLAGS': '${_defines(CPPDEFPREFIX, CPPDEFINES, CPPDEFSUFFIX, __env__, '
# # #                 'TARGET, SOURCE)}',
# # env.Append(CPPDEFINES=CPP_DEFINES)
# # # Step1.2.2: -I setting.
# # # 'INCPREFIX': '-I'
# # # '_CPPINCFLAGS': '${_concat(INCPREFIX, CPPPATH, INCSUFFIX, __env__, RDirs, '
# # #                 'TARGET, SOURCE, affect_signature=False)}',
# # env.Append(CPPPATH=CPP_PATH)

# # ###################################################################################
# # # Step2: ASM compile setting. use <print(env.Dump())> for details.
# # # 'ASCOM': '$AS $ASFLAGS -o $TARGET $SOURCES'

# # # Step2.0: General options. $ASFLAGS.
# # env.Append(ASFLAGS=AS_FLAGS)


# # ###################################################################################
# # # Step3: LINK setting. use <print(env.Dump())> for details.
# # # 'LINKCOM': '$LINK -o $TARGET $LINKFLAGS $__RPATH $SOURCES $_LIBDIRFLAGS '
# # #            '$_LIBFLAGS',

# # # Step3.0: General options. $LINKFLAGS.
# # env.Append(LINKFLAGS=LINK_FLAGS)

# # # Step3.1: Link path setting. $_LIBDIRFLAGS.
# # # '_LIBDIRFLAGS': '${_concat(LIBDIRPREFIX, LIBPATH, LIBDIRSUFFIX, __env__, '
# # #                 'RDirs, TARGET, SOURCE, affect_signature=False)}',
# # env.Append(LIBPATH=LIB_PATH)

# # # Step3.2: libs setting, like *.a, *.lib. $_LIBFLAGS.
# # #   '_LIBFLAGS': '${_stripixes(LIBLINKPREFIX, LIBS, LIBLINKSUFFIX, LIBPREFIXES, '
# # #                'LIBSUFFIXES, __env__)}',
# # env.Append(LIBS=LIBS_FILES)

# # # Step3.3: Add some spec params. must append at end.
# # env.Append(_LIBFLAGS=SPEC_LD_FLAGS)


# # ###################################################################################
# # # Step4: Compile Object files, use:
# # #        1. <$CCCOM>: For c code compile
# # #        2. <$ASCOM>: For asm code compile
# # c_objs = env.Object(C_SRC_LIST)
# # as_objs = env.Object(AS_SRC_LIST)


# # ###################################################################################
# # # Step5: Compile target <.elf>, use <$LINKCOM>.
# # target = env.Program(target = TARGET_WITHOUT_SUFFIX, source=[c_objs, as_objs])

# # # Other compile target.
# # env.Command(TARGET_WITHOUT_SUFFIX + '.bin', target, OBJCPY + ' -v -O binary $SOURCE $TARGET')
# # env.Command(TARGET_WITHOUT_SUFFIX + '.lst', target, OBJDUMP + ' --source --all-headers --demangle --line-numbers --wide $SOURCE > $TARGET')
# # env.Command(TARGET_WITHOUT_SUFFIX + '.size', target, SIZE + ' --format=berkeley $SOURCE')

# # # Dump() env params, if need.
# # #print(env.Dump())


from anytree import Node, RenderTree, findall
from anytree.iterators import PostOrderIter
import os
import sys
import json
import copy
import shutil
sdk_path = os.path.abspath(os.path.join(sys.path[0], '..', '..'))
PROJECT_NAME = os.path.basename(sys.path[0])
PROJECT_PATH = sys.path[0]

if not os.path.exists('.config'):
    shutil.copy('config_defaults.mk', '.config')


# print(sys.argv)
if 'menuconfig' in sys.argv:
    os.environ['SDK_PATH'] = sdk_path

    os.system(
        f'SDK_PATH="{sdk_path}" python3 {os.path.join(sdk_path, "tools", "kconfig", "Kconfiglib", "menuconfig.py")} {os.path.join(sdk_path, "Kconfig")}')
    exit(0)

if 'distclean' in sys.argv:
    try:
        shutil.rmtree('build')
    except:
        pass
    try:
        shutil.rmtree('dist')
    except:
        pass
    try:
        os.remove('.sconsign.dblite')
    except:
        pass
    try:
        os.remove('.config')
    except:
        pass
    try:
        os.remove('.config.old')
    except:
        pass
    
    exit(0)



if 'SET_CROSS' in sys.argv:
    for confs in sys.argv:
        if confs.startswith('CROSS_DIR'):
            with open('.config', 'a') as fs:
                fs.write("\n")
                fs.write(f'CONFIG_TOOLCHAIN_PATH="{confs.split("=")[1]}"\n')
        elif confs.startswith('CROSS'):
            with open('.config', 'a') as fs:
                fs.write("\n")
                fs.write(f'CONFIG_TOOLCHAIN_PREFIX="{confs.split("=")[1]}"\n')
    exit(0)

if os.path.exists('.config'):
    with open('.config', 'r') as f:
        for line in f.readlines():
            if not line.startswith('#') and line.strip():
                tmpstr = line.split('=')
                os.environ[tmpstr[0]] = tmpstr[1].replace(
                    '\n', '').replace('""', '')


env = Environment()
env['TARGET_ARCH'] = 'linux'

if os.environ.get('CONFIG_TOOLCHAIN_PREFIX') is not None:
    if os.environ['CONFIG_TOOLCHAIN_PREFIX']:
        env['CC'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'gcc'
        env['AS'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'as'
        env['AR'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'ar'
        env['CXX'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++'
        env['LINK'] = os.environ['CONFIG_TOOLCHAIN_PREFIX'].strip('"') + 'g++'

    if os.environ['CONFIG_TOOLCHAIN_PATH']:
        env['ENV']['PATH'] = os.environ['CONFIG_TOOLCHAIN_PATH'].strip(
            '"') + ':' + env['ENV']['PATH']


def get_prive_globals():
    return globals()


with open('../../tools/scons/sconstool.py') as f:
    exec(f.read())


def find_component():
    components = {}
    for dir_name in os.listdir('.'):
        component = {}
        if os.path.exists(os.path.join(dir_name, "Sconstruct.py")):
            # 二进制名字
            if 'main' in dir_name:
                if len(dir_name) > 5:
                    component['bin_name'] = dir_name[5:]
                else:
                    component['bin_name'] = PROJECT_NAME
            # 添加编译信息
            component['src_dir'] = dir_name
            os.environ['COMPONENT_SRC_DIR'] = dir_name
            with open(os.path.join(dir_name, "Sconstruct.py")) as f:
                exec(f.read())

            component['build_info'] = json.loads(os.environ['COMPONENT_INFO'])
            components[component['bin_name']] = component
    with open(os.path.join(sdk_path, 'components/components.py')) as f:
        exec(f.read())

    for dir_name in os.environ['COMPONENT_LIST'].replace(' ', '').split(','):
        component = {}
        if os.path.exists(os.path.join(sdk_path, 'components', dir_name, "Sconstruct.py")):
            component['src_dir'] = os.path.join(
                sdk_path, 'components', dir_name)
            component['bin_name'] = dir_name
            os.environ["COMPONENT_SRC_DIR"] = component['src_dir']
            with open(os.path.join(sdk_path, 'components', dir_name, "Sconstruct.py")) as f:
                exec(f.read())

            component['build_info'] = json.loads(os.environ['COMPONENT_INFO'])
            components[component['bin_name']] = component

    return components


def touch(file_path):
    # 获取文件夹路径
    folder_path = os.path.dirname(file_path)

    # 确保文件夹存在，如果不存在则创建它
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)

    # 使用 open() 函数以写入模式打开文件
    with open(file_path, 'w'):
        pass


add_task_list = {}


def _commpile_com(task):
    global env
    SCONS_SRCS = []
    SCONS_OBJS = []
    SCONS_LIBS = []
    SCONS_LIBS_PATH = []
    SCONS_INCLUDE = []
    SCONS_INCLUDE_PRIVATE = []
    SCONS_DEFINITIONS = []
    SCONS_LINK_FLAGS = []

    if task['bin_name'] in add_task_list:
        pass
    else:
        tmpenv = env.Clone()

        # 头文件
        for incd in task['build_info']['COMPONENT_ADD_INCLUDE']:
            SCONS_INCLUDE.append(os.path.join(task['src_dir'], incd))
        for incd in task['build_info']['COMPONENT_ADD_PRIVATE_INCLUDE']:
            SCONS_INCLUDE_PRIVATE.append(os.path.join(task['src_dir'], incd))

        # 编译选项
        SCONS_DEFINITIONS += task['build_info']['COMPONENT_ADD_DEFINITIONS']

        # 链接选项
        SCONS_LINK_FLAGS += task['build_info']['COMPONENT_ADD_LINK_FLAGS']

        # 库依赖
        for lib in task['build_info']['COMPONENT_ADD_REQUIREMENTS']:
            if lib in add_task_list:
                SCONS_LIBS += add_task_list[lib]['SCONS_LIBS']
                SCONS_LIBS_PATH += add_task_list[lib]['SCONS_LIBS_PATH']
                SCONS_INCLUDE += add_task_list[lib]['SCONS_INCLUDE']
            else:
                SCONS_LIBS.append(lib)

        # 源文件
        SCONS_SRCS = task['build_info']['COMPONENT_ADD_SRCS'].copy()
        # for srcf in task['build_info']['COMPONENT_ADD_SRCS']:
        #     SCONS_SRCS.append(os.path.join(task['src_dir'], srcf))

        # obj转换
        call_add_object_fun = None
        if task['build_info']['COMPONENT_REGISTER_COMPONENT'] == 'sharedlib':
            call_add_object_fun = tmpenv.SharedObject
            env.Command(os.path.join('dist', 'lib', 'lib' + task['bin_name'] + '.so'), os.path.join('build', os.path.basename(
                task['src_dir']), 'lib' + task['bin_name'] + '.so'), action=[Mkdir(os.path.join('dist', 'lib')), Copy("$TARGET", "$SOURCE")])
        else:
            call_add_object_fun = tmpenv.Object

        for srcf in SCONS_SRCS:
            if srcf.endswith('.cpp'):
                # print(os.path.join('build', os.path.basename(task_name['src_dir']), os.path.basename(srcf.replace('.cpp', '.cxx.o'))), srcf)
                SCONS_OBJS.append(call_add_object_fun(os.path.join('build', os.path.basename(
                    task['src_dir']), os.path.basename(srcf.replace('.cpp', '.cxx.o'))), srcf))
            elif srcf.endswith('.c'):
                # print(os.path.join('build', os.path.basename(task_name['src_dir']), os.path.basename(srcf.replace('.c', '.cc.o'))), srcf)
                SCONS_OBJS.append(call_add_object_fun(os.path.join('build', os.path.basename(
                    task['src_dir']), os.path.basename(srcf.replace('.c', '.cc.o'))), srcf))
            elif srcf.endswith('.S'):
                SCONS_OBJS.append(call_add_object_fun(os.path.join('build', os.path.basename(
                    task['src_dir']), os.path.basename(srcf.replace('.S', '.S.o'))), srcf))

        # 添加编译信息
        tmpenv.Append(LIBS=SCONS_LIBS)
        tmpenv.Append(LIBPATH=SCONS_LIBS_PATH)
        tmpenv.Append(CPPPATH=SCONS_INCLUDE)
        tmpenv.Append(CPPPATH=SCONS_INCLUDE_PRIVATE)
        tmpenv.Append(LINKFLAGS=SCONS_LINK_FLAGS)
        tmpenv.MergeFlags(SCONS_DEFINITIONS)

        # SCONS_DEFINITIONS

        if task['build_info']['COMPONENT_REGISTER_COMPONENT'] == 'sharedlib':
            tager = tmpenv.SharedLibrary(target=os.path.join('build', os.path.basename(
                task['src_dir']), task['bin_name']), source=SCONS_OBJS)
        else:
            tager = tmpenv.Library(target=os.path.join('build', os.path.basename(
                task['src_dir']), task['bin_name']), source=SCONS_OBJS)
            
        # 保留编译信息
        SCONS_LIBS.append(task['bin_name'])
        SCONS_LIBS_PATH.append(os.path.join(
            'build', os.path.basename(task['src_dir'])))
        add_task_list[task['bin_name']] = {'SCONS_LIBS': copy.deepcopy(SCONS_LIBS), 'SCONS_LIBS_PATH': copy.deepcopy(
            SCONS_LIBS_PATH), 'SCONS_INCLUDE': copy.deepcopy(SCONS_INCLUDE), 'TAGER': tager}

        # 依赖
        component_libs = []
        for lib in task['build_info']['COMPONENT_ADD_REQUIREMENTS']:
            if lib in add_task_list:
                component_libs.append(lib)
        if component_libs:
            Depends(tager, list(
                map(lambda item: add_task_list[item]['TAGER'], component_libs)))


def _commpile_exe(task):
    global env
    SCONS_SRCS = []
    SCONS_OBJS = []
    SCONS_LIBS = []
    SCONS_LIBS_PATH = []
    SCONS_INCLUDE = []
    SCONS_DEFINITIONS = []
    SCONS_LINK_FLAGS = []
    tmpenv = env.Clone()

    # 头文件
    for incd in task['build_info']['COMPONENT_ADD_INCLUDE']:
        SCONS_INCLUDE.append(os.path.join(task['src_dir'], incd))

    # 编译选项
    SCONS_DEFINITIONS += task['build_info']['COMPONENT_ADD_DEFINITIONS']

    # 库依赖
    for lib in task['build_info']['COMPONENT_ADD_REQUIREMENTS']:
        if lib in add_task_list:
            SCONS_LIBS += add_task_list[lib]['SCONS_LIBS']
            SCONS_LIBS_PATH += add_task_list[lib]['SCONS_LIBS_PATH']
            SCONS_INCLUDE += add_task_list[lib]['SCONS_INCLUDE']
        else:
            SCONS_LIBS.append(lib)

    # 源文件
        SCONS_SRCS = task['build_info']['COMPONENT_ADD_SRCS'].copy()
        # for srcf in task['build_info']['COMPONENT_ADD_SRCS']:
        #     SCONS_SRCS.append(os.path.join(task['src_dir'], srcf))

    # obj转换
    call_add_object_fun = None
    if task['build_info']['COMPONENT_REGISTER_COMPONENT'] == 'sharedlib':
        call_add_object_fun = tmpenv.SharedObject
    else:
        call_add_object_fun = tmpenv.Object

    for srcf in SCONS_SRCS:
        if '.cpp' in srcf:
            # print(os.path.join('build', os.path.basename(task_name['src_dir']), os.path.basename(srcf.replace('.cpp', '.cxx.o'))), srcf)
            SCONS_OBJS.append(call_add_object_fun(os.path.join('build', os.path.basename(
                task['src_dir']), os.path.basename(srcf.replace('.cpp', '.cxx.o'))), srcf))
        elif '.c' in srcf:
            # print(os.path.join('build', os.path.basename(task_name['src_dir']), os.path.basename(srcf.replace('.c', '.cc.o'))), srcf)
            SCONS_OBJS.append(call_add_object_fun(os.path.join('build', os.path.basename(
                task['src_dir']), os.path.basename(srcf.replace('.c', '.cc.o'))), srcf))
        elif '.S' in srcf:
            SCONS_OBJS.append(call_add_object_fun(os.path.join('build', os.path.basename(
                task['src_dir']), os.path.basename(srcf.replace('.S', '.S.o'))), srcf))

    # 添加编译信息
    tmpenv.Append(LIBS=SCONS_LIBS)
    tmpenv.Append(LIBPATH=SCONS_LIBS_PATH)
    tmpenv.MergeFlags(SCONS_DEFINITIONS)
    tmpenv.Append(CPPPATH=SCONS_INCLUDE)
    tmpenv.Append(LINKFLAGS=SCONS_LINK_FLAGS)
    tmp_file = os.path.join(
        'build', os.path.basename(task['src_dir']), 'tmp.cpp')
    touch(tmp_file)
    tager = tmpenv.Program(target=os.path.join('build', os.path.basename(
        task['src_dir']), task['bin_name']), source=[tmp_file, SCONS_OBJS])
    # 依赖
    component_libs = []
    for lib in task['build_info']['COMPONENT_ADD_REQUIREMENTS']:
        if lib in add_task_list:
            component_libs.append(lib)
    if component_libs:
        Depends(tager, list(
            map(lambda item: add_task_list[item]['TAGER'], component_libs)))
    env.Command(os.path.join('dist', task['bin_name']), os.path.join('build', os.path.basename(
        task['src_dir']), task['bin_name']), action=[Mkdir("dist"), Copy("$TARGET", "$SOURCE")])


def preconditioning(tasks):
    root = Node(PROJECT_NAME)
    for item in tasks:
        if tasks[item]['build_info']['COMPONENT_REGISTER_COMPONENT'] == 'project':
            child1 = Node(item, parent=root)
            for mkds in tasks[item]['build_info']['COMPONENT_ADD_REQUIREMENTS']:
                if mkds in tasks:
                    Node(mkds, parent=child1)
        else:
            for mkds in tasks[item]['build_info']['COMPONENT_ADD_REQUIREMENTS']:
                child1 = findall(
                    root, filter_=lambda node: node.name == "nanolog")
                if len(child1):
                    for mks in child1:
                        if mkds in tasks:
                            Node(mkds, parent=mks)
                else:
                    Node(mkds, parent=root)

    for node in PostOrderIter(root):
        if node.is_root:
            pass
        else:
            if node.parent.is_root:
                # 可执行程序
                _commpile_exe(tasks[node.name])
            else:
                # 组件
                _commpile_com(tasks[node.name])

    # for pre, fill, node in RenderTree(root):
    #     print("%s%s" % (pre, node.name))

    return task


task = find_component()
preconditioning(task)
