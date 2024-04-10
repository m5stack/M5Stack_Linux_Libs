#!/usr/bin/env python3
import paramiko
import sys
import os
from pathlib import Path

def ssh_push_file(file_group, remote_host, remote_port, username, password):
    # 创建SSH客户端实例
    ssh = paramiko.SSHClient()
    
    # 自动添加不在known_hosts文件的主机名和新的主机密钥
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    
    # 连接到远程服务器
    if password != 'None':
        ssh.connect(hostname=remote_host, port=remote_port, username=username, password=password, timeout=15)
    else:
        ssh.connect(hostname=remote_host, port=remote_port, username=username, timeout=15)
    
    # 使用SSH客户端的open_sftp方法打开SFTP会话
    sftp = ssh.open_sftp()
    
    # 将本地文件推送到远程服务器
    for local_file, remote_file in file_group:
        print("push", local_file, remote_file, '...')
        sys.stdout.flush()
        sftp.put(local_file, remote_file)
        local_file_stat = os.stat(local_file)
        mode = local_file_stat.st_mode
        atime = local_file_stat.st_atime
        mtime = local_file_stat.st_mtime
        sftp.chmod(remote_file, mode)
        sftp.utime(remote_file, (atime, mtime))
        sys.stdout.write("\033[A\033[K")
        print("push", local_file, remote_file, 'success!')
        sys.stdout.flush()
    
    # 关闭SFTP会话和SSH客户端
    sftp.close()
    ssh.close()
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
            file_group.append([str(Path(root)/file), str(Path(remote_file_path)/file)])
    if file_group:
        ssh_push_file(file_group, remote_host, remote_port, username, password)

