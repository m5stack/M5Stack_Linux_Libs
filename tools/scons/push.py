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

def create_ssh_client(hostname, port, username, password):
    client = paramiko.SSHClient()
    client.load_system_host_keys()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
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

def get_file_md5(file_path):
    """Calculate MD5 checksum of a file."""
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

def ssh_push_file(file_group, remote_host, remote_port, username, password):
    ssh, scpclient = create_ssh_client(remote_host, remote_port, username, password)
    for local_file, remote_file in file_group:
        print("Checking", local_file)
        sys.stdout.flush()
        remote_dir = Path(Path(remote_file).parent).as_posix()
        
        try:
            # Retrieve remote file MD5 checksum if it exists
            stdin, stdout, stderr = ssh.exec_command(f'md5sum {remote_file} || echo "no_remote_md5"')
            remote_md5 = stdout.read().strip().split()[0].decode('utf-8')
        except Exception as e:
            print("Could not retrieve remote file checksum:", e)
            remote_md5 = "no_remote_md5"

        local_md5 = get_file_md5(local_file)
        if local_md5 != remote_md5:
            print("push", local_file, remote_file, '...')
            sys.stdout.flush()
            if ssh is not None:
                for cout in range(3):
                    try:
                        stdin, stdout, stderr = ssh.exec_command('[ -d "{}" ] || mkdir -p "{}"'.format(remote_dir, remote_dir))
                        exit_status = stdout.channel.recv_exit_status()
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
        else:
            print("No changes detected for", local_file, ", skipping upload.")



if __name__ == '__main__':

    if len(sys.argv) < 6:
        print("Usage: push.py local_file_path remote_file_path remote_host remote_port username password")
        exit(1)
    local_file_path = sys.argv[1]
    remote_file_path = sys.argv[2]
    remote_host = sys.argv[3]
    remote_port = sys.argv[4]
    username = sys.argv[5]
    password = sys.argv[6]

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
        ssh_push_file(file_group, remote_host, remote_port, username, password)

