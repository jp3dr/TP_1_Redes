all:
	gcc -Wall -c common.c
	gcc -Wall cliente.c common.o -o cliente
	gcc -Wall servidor.c common.o -lpthread -o servidor

clean:
	rm common.o cliente servidor
