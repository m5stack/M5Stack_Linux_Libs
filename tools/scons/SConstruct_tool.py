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

    supported_extensions = ['.c', '.cc', '.cpp', '.S', '.s']
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

def sample_wget(down_url, file_path):
    if not os.path.exists(file_path):
        import requests
        from tqdm import tqdm
        
        response = requests.get(down_url, stream=True)
        if response.status_code == 200:
            # 获取文件总大小
            total_size = int(response.headers.get('content-length', 0))
            
            with open(file_path, 'wb') as file:
                with tqdm(total=total_size, unit='B', unit_scale=True, desc=os.path.basename(file_path)) as pbar:
                    for chunk in response.iter_content(chunk_size=8192):
                        if chunk:
                            file.write(chunk)
                            pbar.update(len(chunk))
        else:
            env.Fatal("{} down failed".format(down_url))
    return file_path

def wget_tar_xz(url, file_name):
    file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
    path  = file_path[:-7]
    if not os.path.exists(path):
        sample_wget(url, file_path)
        import  tarfile
        with tarfile.open(file_path, 'r:xz') as tar:
            tar.extractall(path=path)
    return path

def wget_tar_gz(url, file_name):
    file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
    path  = file_path[:-7]
    if not os.path.exists(path):
        sample_wget(url, file_path)
        import  tarfile
        with tarfile.open(file_path, 'r:gz') as tar:
            tar.extractall(path=path)
    return path

def wget_zip(url, file_name):
    file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
    path  = file_path[:-4]
    if not os.path.exists(path):
        sample_wget(url, file_path)
        import  zipfile
        with zipfile.ZipFile(file_path, 'r') as zip_ref:
            for file_info in zip_ref.infolist():
                try:
                    zip_ref.extract(file_info, path)
                except Exception as e:
                    pass
    return path

def wget_github_commit(url, commit):
    import parse
    import shutil
    repo = parse.parse("{}://{}/{}/{}.git", url)
    github_url = url
    if github_url.endswith('.git'):
        github_url = github_url[:-4]
    down_url = github_url + "/archive/{}.zip".format(commit)
    zip_file_name = '{}-{}.zip'.format(repo[3], commit)
    file_path = wget_zip(down_url, zip_file_name)
    shutil.move(os.path.join(file_path, zip_file_name[:-4]), os.path.join(os.environ['GIT_REPO_PATH'], repo[3]))
    shutil.rmtree(file_path)
    return down_url

def check_component(component_name):
    if component_name in env['GIT_REPO_LISTS']:
        if not os.path.exists(env['GIT_REPO_LISTS'][component_name]['path']):
            if 'CONFIG_REPO_AUTOMATION' in os.environ:
                down = 'y'
            else:
                down = input('{} does not exist. Please choose whether to download it automatically? Y/N :'.format(component_name))
                down = down.lower()
            if down == 'y':
                try:
                    down_url = wget_github_commit(env['GIT_REPO_LISTS'][component_name]['url'], env['GIT_REPO_LISTS'][component_name]['commit'])
                    print("The {} download successful.".format(down_url))
                except Exception as e:
                    print('Please manually download {} to {}.'.format(env['GIT_REPO_LISTS'][component_name]['url'], env['GIT_REPO_LISTS'][component_name]['path']))
                    env.Fatal("Cloning failed.: {}".format(e))
            else:
                env.Fatal('Please manually download {} to {}.'.format(env['GIT_REPO_LISTS'][component_name]['url'], env['GIT_REPO_LISTS'][component_name]['path']))

def check_wget_down(url, file_name):
    if file_name.endswith('.zip'):
        file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
        path  = file_path[:-4]
        if not os.path.exists(path):
            if 'CONFIG_REPO_AUTOMATION' in os.environ:
                down = 'y'
            else:
                down = input('{} does not exist. Please choose whether to download it automatically? Y/N :'.format(file_path))
                down = down.lower()
            if down == 'y':
                return wget_zip(url, file_name)
        return path
    elif file_name.endswith('.tar.gz'):
        file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
        path  = file_path[:-7]
        if not os.path.exists(path):
            if 'CONFIG_REPO_AUTOMATION' in os.environ:
                down = 'y'
            else:
                down = input('{} does not exist. Please choose whether to download it automatically? Y/N :'.format(file_path))
                down = down.lower()
            if down == 'y':
                return wget_tar_gz(url, file_name)
        return path
    elif file_name.endswith('.tar.xz'):
        file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
        path  = file_path[:-7]
        if not os.path.exists(path):
            if 'CONFIG_REPO_AUTOMATION' in os.environ:
                down = 'y'
            else:
                down = input('{} does not exist. Please choose whether to download it automatically? Y/N :'.format(file_path))
                down = down.lower()
            if down == 'y':
                return wget_tar_xz(url, file_name)
        return path
    else:
        file_path = os.path.join(os.environ['GIT_REPO_PATH'], file_name)
        if not os.path.exists(file_path):
            if 'CONFIG_REPO_AUTOMATION' in os.environ:
                down = 'y'
            else:
                down = input('{} does not exist. Please choose whether to download it automatically? Y/N :'.format(file_path))
                down = down.lower()
            if down == 'y':
                return sample_wget(url, file_path)
        return file_path

def CC_cmd_execute(cmd):
    import os
    env.ParseConfig('${{CC}} {} > gcc_out.txt 2>&1'.format(cmd))
    with open('gcc_out.txt', 'r') as conf_file:
        out = conf_file.read()
    os.remove('gcc_out.txt')
    return out


def DefineProject(target, SRCS=[], INCLUDE=[], PRIVATE_INCLUDE=[], REQUIREMENTS=[], STATIC_LIB=[], DYNAMIC_LIB=[], DEFINITIONS=[], DEFINITIONS_PRIVATE=[], LDFLAGS=[], LINK_SEARCH_PATH=[], STATIC_FILES = []):
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
                      'STATIC_FILES': STATIC_FILES,
                      'target': target,
                      'project': 'project'
                      }
    env['COMPONENTS'].append(component_info)

def DefineStatic(target, SRCS=[], INCLUDE=[], PRIVATE_INCLUDE=[], REQUIREMENTS=[], STATIC_LIB=[], DYNAMIC_LIB=[], DEFINITIONS=[], DEFINITIONS_PRIVATE=[], LDFLAGS=[], LINK_SEARCH_PATH=[], STATIC_FILES = []):
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
                      'STATIC_FILES': STATIC_FILES,
                      'target': target,
                      'project': 'static'
                      }
    env['COMPONENTS'].append(component_info)

def DefineShared(target, SRCS=[], INCLUDE=[], PRIVATE_INCLUDE=[], REQUIREMENTS=[], STATIC_LIB=[], DYNAMIC_LIB=[], DEFINITIONS=[], DEFINITIONS_PRIVATE=[], LDFLAGS=[], LINK_SEARCH_PATH=[], STATIC_FILES = []):
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
                      'STATIC_FILES': STATIC_FILES,
                      'target': target,
                      'project': 'shared'
                      }
    env['COMPONENTS'].append(component_info)
