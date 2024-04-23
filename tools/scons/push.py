#!/usr/bin/env python3

import sys
import os
from pathlib import Path
import paramiko
from scp import SCPClient 
import sys
def ssh_push_file(file_group, remote_host, remote_port, username, password):
    ssh=paramiko.SSHClient()
    ssh.load_system_host_keys()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    
    if password != 'None':
        ssh.connect(hostname=remote_host, port=remote_port, username=username, password=password, timeout=15, compress=True)
    else:
        ssh.connect(hostname=remote_host, port=remote_port, username=username, timeout=15, compress=True)

    scpclient = SCPClient(ssh.get_transport(),socket_timeout=15.0)
    for local_file, remote_file in file_group:
        print("push", local_file, remote_file, '...')
        sys.stdout.flush()
        # sftp.put(local_file, remote_file)
        scpclient.put(local_file, remote_file)
        local_file_stat = os.stat(local_file)
        # mode = local_file_stat.st_mode
        # atime = local_file_stat.st_atime
        # mtime = local_file_stat.st_mtime
        # sftp.chmod(remote_file, mode)
        # sftp.utime(remote_file, (atime, mtime))
        sys.stdout.write("\033[A\033[K")
        print("push", local_file, remote_file, 'success!')
        sys.stdout.flush()



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
            file_group.append([str(Path(root)/file), (Path(remote_file_path)/file).as_posix()])
    if file_group:
        ssh_push_file(file_group, remote_host, remote_port, username, password)

