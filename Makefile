all:
	gcc -Wall libs/common.c -c
	gcc -Wall libs/socketHandler.c -c
	gcc -Wall client.c common.o  socketHandler.o -o client
	gcc -Wall server.c common.o  socketHandler.o -o server
	gcc -Wall server-mt.c common.o  socketHandler.o -lpthread -o server-mt

clean:
	rm common.o socketHandler.o client server server-mt
