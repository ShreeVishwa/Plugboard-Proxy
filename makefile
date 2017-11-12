CC = gcc
hellomake: pbproxy.c
	$(CC) -o pbproxy pbproxy.c -lcrypto
