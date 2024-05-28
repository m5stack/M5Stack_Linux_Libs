# SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
#
# SPDX-License-Identifier: MIT
def ADir(path):
    return Dir(os.path.join(env['component_dir'], path))
def AFile(file):
    return File(os.path.join(env['component_dir'], file))

def AGlob(path):
    return Glob(os.path.join(env['component_dir'], path))

def append_srcs_dir(directories):
    source_files = []
    import os
    from collections.abc import Iterable

    supported_extensions = ['.c', '.cc', '.cpp', '.S']
    # if isinstance(sfile, list):
    def _find_file(path):
        directory = str(path)
        for root, dirs, files in os.walk(directory):
            for file in files:
                _, file_extension = os.path.splitext(file)
                if file_extension in supported_extensions:
                    source_files.append(File(os.path.join(root, file)))
                    
    if isinstance(directories, Iterable):
        for directory in directories:
            _find_file(directory)
    else:
        _find_file(directories)

    return source_files

def register_component(SRCS=[], INCLUDE=[], PRIVATE_INCLUDE=[], REQUIREMENTS=[], STATIC_LIB=[], DYNAMIC_LIB=[], DEFINITIONS=[], DEFINITIONS_PRIVATE=[], LDFLAGS=[], LINK_SEARCH_PATH=[], project='static', target='main'):
    component_info = {'SRCS': SRCS,
                      'INCLUDE': INCLUDE,
                      'PRIVATE_INCLUDE': PRIVATE_INCLUDE,
                      'REQUIREMENTS': REQUIREMENTS,
                      'STATIC_LIB': STATIC_LIB,
                      'DYNAMIC_LIB': DYNAMIC_LIB,
                      'DEFINITIONS': DEFINITIONS,
                      'DEFINITIONS_PRIVATE': DEFINITIONS_PRIVATE,
                      'LDFLAGS': LDFLAGS,
                      'LINK_SEARCH_PATH': LINK_SEARCH_PATH,
                      'target': target,
                      'project': project
                      }
    env['COMPONENTS'].append(component_info)

def calculate_sha256(file_path):
    import hashlib
    sha256_hash = hashlib.sha256()
    with open(file_path, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

def compare_and_copy(file1, file2):
    import os
    if not os.path.exists(file1):
        return
    if not os.path.exists(file2):
        with open(file1, "rb") as f1, open(file2, "wb") as f2:
            f2.write(f1.read())
    file1_sha256 = calculate_sha256(file1)
    file2_sha256 = calculate_sha256(file2)
    if file1_sha256 != file2_sha256:
        with open(file1, "rb") as f1, open(file2, "wb") as f2:
            f2.write(f1.read())

