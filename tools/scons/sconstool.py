import copy
import os
import fnmatch
import json
def creat_build_info():
    _info = {'ADD_INCLUDE': [], 'ADD_PRIVATE_INCLUDE': [], 'ADD_SRCS': [], 'ADD_ASM_SRCS': [], 'ADD_REQUIREMENTS': [], 'ADD_STATIC_LIB': [], 'ADD_DYNAMIC_LIB': [], 'ADD_DEFINITIONS': [], 'C_LINK_FLAGS': [], 'REGISTER_COMPONENT': ''}
    return copy.deepcopy(_info)

def get_config(config):
    try:
        cons = os.environ.get(config)
    except:
        cons = ""
    return cons

def register_component(build_info):
    os.environ["COMPONENT_INFO"] = json.dumps(build_info)


def aux_source_directory(root_dir):
    _src_dir = os.environ["COMPONENT_SRC_DIR"]
    files_src = []
    for root, _, files in os.walk(os.path.join(_src_dir, root_dir)):
        for name in files:
            if name.endswith('.cpp') or name.endswith('.cc') or name.endswith('.c'):
                files_src.append(name.replace(_src_dir, ""))

def remove_item():
    pass




def append_srcs_dir(dir):
    _src_dir = os.path.join(os.environ["COMPONENT_SRC_DIR"], dir)        
    files_src = []
    if isinstance(dir, tuple) or isinstance(dir, list):
        for _dir in dir:
            for file in os.listdir(os.path.join(_src_dir, _dir)):
                if file.endswith('.cpp') or file.endswith('.cc') or file.endswith('.c'):
                    files_src.append(os.path.join(dir, file))
    elif isinstance(dir, str):
        for file in os.listdir(_src_dir):
            if file.endswith('.cpp') or file.endswith('.cc') or file.endswith('.c'):
                files_src.append(os.path.join(dir, file))
    return files_src


def find_files(root_dir, pattern):
    c_files = []

    for root, _, files in os.walk(root_dir):
        for filename in fnmatch.filter(files, pattern):
            c_files.append(os.path.join(root, filename))

    return c_files


def append_srcs_dir_all(dir):
    _src_dir = os.environ["COMPONENT_SRC_DIR"]
    files = []
    if isinstance(dir, tuple):
        pass
    elif isinstance(dir, list):
        pass
    elif isinstance(dir, str):
        files = find_files(os.path.join(_src_dir, dir), "*.c")
        files += find_files(os.path.join(_src_dir, dir), "*.cc")
        files += find_files(os.path.join(_src_dir, dir), "*.cpp")
    return files

def append_srcs(src_file):
    _src_dir = os.environ["COMPONENT_SRC_DIR"]
    return os.path.join(_src_dir, src_file)