#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

void usage(int argc, char **argv) {
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv) {
    int s = connectToServer(argc, argv);
    connectionUp(s);

    char buf[BUFSZ];
	unsigned total = 0;
    memset(buf, 0, BUFSZ);
	while(1){
		char*mensagem = send2(s);
		size_t count = recv(s, buf + total, BUFSZ - total, 0);
        // if (count == 0) {
        //     // Connection terminated.
        //     break;
        // }
        total += count;
	}

    // memset(buf, 0, BUFSZ);
    // unsigned total = 0;
    // while (1) {
    //     count = recv(s, buf + total, BUFSZ - total, 0);
    //     if (count == 0) {
    //         // Connection terminated.
    //         break;
    //     }
    //     total += count;
    // }
    close(s);

    printf("received %u bytes\n", total);
    puts(buf);

    exit(EXIT_SUCCESS);
}