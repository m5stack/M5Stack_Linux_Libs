#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
#
# SPDX-License-Identifier: MIT
import pexpect
import sys
def main(server):
    command = 'ssh %s %s@%s' % (server['ssh_conf'], server['username'], server['hostname'])
    process = pexpect.spawn(command, timeout=30)
    print(f'cmd: {command}')
    expect_list = ['yes/no', 'password:', '\$', '#', pexpect.EOF, pexpect.TIMEOUT]
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
            process.interact()
        else:
            print('EOF or TIMEOUT')
    elif index == 1:
        process.sendline(server['password'])
        process.interact()
    elif index == 2:
        process.interact()
    elif index == 3:
        process.interact()
    else:
        print('EOF or TIMEOUT')


if __name__ == '__main__':

    if len(sys.argv) < 4:
        print("Usage: expect ssh.exp host user password ssh_conf")
        exit(1)

    server = {
        'hostname': sys.argv[1],
        'username': sys.argv[2],
        'password': sys.argv[3],
        'ssh_conf': sys.argv[4],
    }
    print(server)
    main(server)