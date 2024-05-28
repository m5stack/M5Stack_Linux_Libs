#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
#
# SPDX-License-Identifier: MIT
import pexpect
import sys
def main(server):
    command = 'ssh %s -r %s %s@%s:%s' % (server['ssh_conf'], server['local_file'], server['username'], server['hostname'], server['remote_file'])
    process = pexpect.spawn(command, timeout=30)
    print(f'cmd: {command}')
    expect_list = ['yes/no', 'password:', pexpect.EOF, pexpect.TIMEOUT]
    index = process.expect(expect_list)
    print(f'match: {index} => {expect_list[index]}')
    if index == 0: 
        process.sendline("yes")
        expect_list = [
            'password:',
            pexpect.EOF,
            pexpect.TIMEOUT,
        ]
        index = process.expect(expect_list)
        print(f'match: {index} => {expect_list[index]}')
        if index == 0:
            process.sendline(server['password'])
        else:
            print('EOF or TIMEOUT')
    elif index == 1:
        process.sendline(server['password'])
    else:
        print('EOF or TIMEOUT')


def packkkkk_update(server):
    import os
    import paramiko
    def upload_folder(local_folder, remote_folder, ssh_client):
        with ssh_client.open_sftp() as sftp:
            try:
                sftp.listdir(remote_folder)
            except IOError:
                # 目录不存在，递归创建
                sftp.mkdir(remote_folder)

            for root, dirs, files in os.walk(local_folder):
                for filename in files:
                    local_path = os.path.join(root, filename)
                    relative_path = os.path.relpath(local_path, local_folder)
                    remote_path = os.path.join(remote_folder, relative_path)
                    print(local_path, remote_path)
                    sftp.put(local_path, remote_path)

    # SSH 连接配置
    hostname = server['hostname']
    username = server['username']
    password = server['password']

    # 本地文件夹和远程文件夹路径
    local_folder_path = server['local_file']
    remote_folder_path = server['remote_file']

    # 建立 SSH 连接    
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(hostname, username=username, password=password)

    # 执行文件夹上传
    upload_folder(local_folder_path, remote_folder_path, ssh)

    # 关闭 SSH 连接
    ssh.close()


if __name__ == '__main__':

    if len(sys.argv) < 6:
        print("Usage: expect push.exp host user passwd ssh_conf local_file remote_file")
        exit(1)

    server = {
        'hostname': sys.argv[1],
        'username': sys.argv[2],
        'password': sys.argv[3],
        'ssh_conf': sys.argv[4],
        'local_file': sys.argv[5],
        'remote_file': sys.argv[6],
    }
    print(server)
    # main(server)
    packkkkk_update(server)