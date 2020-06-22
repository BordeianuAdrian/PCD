all: client server

clean:
	rm client server

client: client.c
	gcc `python2.7-config --cflags` client.c -o client `python2.7-config --ldflags`

server: server.c
	gcc `python2.7-config --cflags` server.c -o server `python2.7-config --ldflags`	
