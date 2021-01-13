all:
	gcc -Wall -c common.c
	gcc -Wall cliente.c common.o -o cliente
	gcc -Wall servidor-mt.c common.o -lpthread -o servidor-mt

clean:
	rm common.o cliente servidor servidor-mt
