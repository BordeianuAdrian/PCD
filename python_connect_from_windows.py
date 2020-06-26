import socket
import time
import sys

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
port = 49999
s.connect(('localhost', port))

while True:
	msg = input("Command To Send: ")
	if msg == "D":
	   s.close()
	   sys.exit(0)

	if(msg.split(" ")[0] == "G"):
		msg = msg + "\n"

		f = open('torecv.jpg','wb')
		while (True):
		    print ("Receiving...")
		    l = s.recv(4096)
		    f.write(l)
		    if len(l) < 4096:
		        break

		f.close()
	else:
		msg = msg + "\n"
		s.send(msg.encode('utf-8'))
		msg = s.recv(4096)
		print (msg)