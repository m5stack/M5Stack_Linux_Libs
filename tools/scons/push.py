#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
#
# SPDX-License-Identifier: MIT

import sys
import os
from pathlib import Path
import paramiko
from scp import SCPClient 
import sys
import hashlib
import configparser
import shlex


BATCH_SIZE = 100

def create_ssh_client(hostname, port, username, password):
    client = paramiko.SSHClient()
    client.load_system_host_keys()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    scpclient = None
    try:
        if password != 'None':
            client.connect(hostname=hostname, port=port, username=username, password=password, timeout=15, compress=True)
        else:
            client.connect(hostname=hostname, port=port, username=username, timeout=15, compress=True)
        scpclient = SCPClient(client.get_transport(),socket_timeout=15.0)
        # print("SSH connection established")
    except :
        # print(f"Failed to connect: {e}")
        client = None
    return client, scpclient


def split_batches(items, batch_size=BATCH_SIZE):
    for index in range(0, len(items), batch_size):
        yield items[index:index + batch_size]

def get_file_md5(file_path):
    """Calculate MD5 checksum of a file."""
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()


def get_remote_md5_group(ssh, remote_files):
    remote_md5_group = {}
    if ssh is None:
        return remote_md5_group

    for batch in split_batches(remote_files):
        files = ' '.join(shlex.quote(remote_file) for remote_file in batch)
        try:
            stdin, stdout, stderr = ssh.exec_command(f'md5sum -- {files} 2>/dev/null || true')
            for line in stdout.read().decode('utf-8', errors='ignore').splitlines():
                parts = line.strip().split(None, 1)
                if len(parts) == 2:
                    remote_md5_group[parts[1]] = parts[0].lstrip('\\')
        except Exception as e:
            print("Could not retrieve remote file checksums:", e)

    return remote_md5_group


def ensure_remote_dirs(ssh, remote_dirs):
    if ssh is None or not remote_dirs:
        return

    for batch in split_batches(sorted(remote_dirs)):
        dirs = ' '.join(shlex.quote(remote_dir) for remote_dir in batch)
        stdin, stdout, stderr = ssh.exec_command(f'mkdir -p -- {dirs}')
        stdout.channel.recv_exit_status()

def ssh_push_file(file_group, remote_host, remote_port, username, password, after_cmd='', before_cmd=''):
    ssh, scpclient = create_ssh_client(remote_host, remote_port, username, password)

    def ensure_ssh():
        nonlocal ssh, scpclient
        if ssh is not None:
            transport = ssh.get_transport()
            if transport is not None and transport.is_active():
                return True
        ssh, scpclient = create_ssh_client(remote_host, remote_port, username, password)
        return ssh is not None

    if before_cmd != '':
        try:
            sys.stdout.flush()
            print(f'run before_cmd: {before_cmd}')
            if not ensure_ssh():
                print("run before_cmd error: SSH connection is not available")
                return
            stdin, stdout, stderr = ssh.exec_command(f'''{before_cmd}''')
            print('before_cmd stdout:',stdout.read().decode('utf-8', errors='ignore')) 
            print('before_cmd stderr:',stderr.read().decode('utf-8', errors='ignore')) 
        except Exception as e:
            print("run before_cmd error", e)

    remote_md5_group = get_remote_md5_group(ssh, [remote_file for local_file, remote_file in file_group])
    changed_file_group = []
    for local_file, remote_file in file_group:
        print("Checking", local_file)
        sys.stdout.flush()

        local_md5 = get_file_md5(local_file)
        remote_md5 = remote_md5_group.get(remote_file, "no_remote_md5")
        if local_md5 != remote_md5:
            changed_file_group.append((local_file, remote_file))
        else:
            print("No changes detected for", local_file, ", skipping upload.")

    ensure_remote_dirs(ssh, {Path(remote_file).parent.as_posix() for local_file, remote_file in changed_file_group})
    for local_file, remote_file in changed_file_group:
        print("push", local_file, remote_file, '...')
        sys.stdout.flush()
        if ssh is not None:
            for cout in range(3):
                try:
                    scpclient.put(local_file, remote_file)
                    print("push", local_file, remote_file, 'success!')
                    sys.stdout.flush()
                    break
                except paramiko.SSHException:
                    if cout != 2:
                        print("push", local_file, remote_file, 'error, will be retry ...')
                    else:
                        print("push", local_file, remote_file, 'error!')
                    ssh, scpclient = create_ssh_client(remote_host, remote_port, username, password)
    if after_cmd != '':
        try:
            sys.stdout.flush()
            print(f'run after_cmd: {after_cmd}')
            if not ensure_ssh():
                print("run after_cmd error: SSH connection is not available")
                return
            stdin, stdout, stderr = ssh.exec_command(f'''{after_cmd}''')
            print('after_cmd stdout:',stdout.read().decode('utf-8', errors='ignore')) 
            print('after_cmd stderr:',stderr.read().decode('utf-8', errors='ignore')) 
        except Exception as e:
            print("run after_cmd error", e)


if __name__ == '__main__':

    if len(sys.argv) < 2:
        print("Usage: push.py setup.ini")
        exit(1)
    config = configparser.ConfigParser()
    config.read(sys.argv[1])
    local_file_path = config['ssh']['local_file_path']
    remote_file_path = config['ssh']['remote_file_path']
    remote_host = config['ssh']['remote_host']
    remote_port = config['ssh']['remote_port']
    username = config['ssh']['username']
    password = config['ssh']['password']
    after_cmd = ''
    before_cmd = ''
    if 'after_cmd' in config['ssh']:
        after_cmd = config['ssh']['after_cmd'].strip('"').strip("'")
    if 'before_cmd' in config['ssh']:
        before_cmd = config['ssh']['before_cmd'].strip('"').strip("'")

    file_group = []
    for root, dirs, files in os.walk(local_file_path):
        for file in files:

            _remote_file_path = ''
            if Path(root).as_posix() == local_file_path:
                _remote_file_path = (Path(remote_file_path)/file).as_posix()
            elif Path(root).as_posix().startswith(local_file_path):
                _remote_file_path = Path(root).as_posix().replace(local_file_path, "", 1)
                if _remote_file_path.startswith('/'):
                    _remote_file_path = _remote_file_path[1:]
                _remote_file_path = (Path(remote_file_path)/_remote_file_path/file).as_posix()
            else:
                _remote_file_path = (Path(remote_file_path)/file).as_posix()
            # print(remote_file_path, root, file, local_file_path)
            file_group.append([str(Path(root)/file), _remote_file_path])
    if file_group:
        ssh_push_file(file_group, remote_host, remote_port, username, password, after_cmd, before_cmd)
