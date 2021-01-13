all:
	gcc -Wall libs/common.c -c
	gcc -Wall libs/socketHandler.c -c
	gcc -Wall common.o socketHandler.o cliente.c -o cliente 
	gcc -Wall common.o socketHandler.o servidor.c -o servidor

clean:
	rm -f cliente servidor common.o socketHandler.o
