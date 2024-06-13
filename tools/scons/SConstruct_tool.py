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

def check_component(component_name):
    if component_name in env['GIT_REPO_LISTS']:
        if not os.path.exists(env['GIT_REPO_LISTS'][component_name]['path']):
            if 'CONFIG_REPO_AUTOMATION' in os.environ:
                down = 'y'
            else:
                down = input('{} does not exist. Please choose whether to download it automatically? Y/N :'.format(component_name))
                down = down.lower()
            if down == 'y':
                # from git import Repo
                import requests
                import parse
                import zipfile
                import shutil
                try:
                    # Downloading via HTTP (more common)
                    repo = parse.parse("{}://{}/{}/{}.git", env['GIT_REPO_LISTS'][component_name]['url'])
                    zip_file = "{}-{}.zip".format(env['GIT_REPO_LISTS'][component_name]['path'], env['GIT_REPO_LISTS'][component_name]['commit'])
                    zip_file_extrpath = "{}-{}".format(env['GIT_REPO_LISTS'][component_name]['path'], env['GIT_REPO_LISTS'][component_name]['commit'])
                    zip_file_next_path = os.path.join(zip_file_extrpath, "{}-{}".format(repo[3], env['GIT_REPO_LISTS'][component_name]['commit']))
                    down_url = "https://github.com/{}/{}/archive/{}.zip".format(repo[2], repo[3], env['GIT_REPO_LISTS'][component_name]['commit'])
                    if not os.path.exists(zip_file):
                        response = requests.get(down_url)
                        if response.status_code == 200:
                            with open(zip_file, 'wb') as file:
                                file.write(response.content)
                        else:
                            env.Fatal("{} down failed".format(down_url))
                    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
                        for file_info in zip_ref.infolist():
                            try:
                                zip_ref.extract(file_info, zip_file_extrpath)
                            except Exception as e:
                                pass
                    shutil.move(zip_file_next_path, env['GIT_REPO_LISTS'][component_name]['path'])
                    shutil.rmtree(zip_file_extrpath)
                    # The way to download Git is to download the Git software package.
                    # Repo.clone_from(env['GIT_REPO_LISTS'][component_name]['url'], env['GIT_REPO_LISTS'][component_name]['path'])
                    # repo = Repo(env['GIT_REPO_LISTS'][component_name]['path'])
                    # repo.git.checkout(env['GIT_REPO_LISTS'][component_name]['commit'])
                    print("The {} download successful.".format(down_url))
                except Exception as e:
                    print('Please manually download {} to {}.'.format(env['GIT_REPO_LISTS'][component_name]['url'], env['GIT_REPO_LISTS'][component_name]['path']))
                    env.Fatal("Cloning failed.: {}".format(e))
            else:
                env.Fatal('Please manually download {} to {}.'.format(env['GIT_REPO_LISTS'][component_name]['url'], env['GIT_REPO_LISTS'][component_name]['path']))

def CC_cmd_execute(cmd):
    import os
    import subprocess
    try:
        new_env = os.environ.copy()
        new_env['PATH'] = env['ENV']['PATH']
        result = subprocess.run([env['CC']] + cmd, env=new_env, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True, text=True)
        out = result.stdout.strip()
    except:
        out = None
    return out