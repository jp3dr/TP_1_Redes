#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define IPv 4
#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

typedef struct{
    char* name;
    char *clients[];
} Tag;

struct client_data {
    int csock;
    struct sockaddr_storage storage;
    Tag* tags;
};

void *client_thread(void *data) {
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);
    char caddrstr[BUFSZ];
    int clientPort =
        addrtostr(caddr, caddrstr, BUFSZ); // maneira de diferenciar cada client

    char buf[BUFSZ];
    while (1) {
        memset(buf, 0, BUFSZ);
        size_t count = recv(cdata->csock, buf, BUFSZ - 1, 0);
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);
        int i;
        for(i=0; i < strlen(buf); i++){
            if(buf[i] == '+'){
                if((i == 1) || (buf[i-1] == ' ') || (buf[i+4] == '\n')){
                    //TODO
                    //conferir se existe ja a tag de tal nome, caso não exista, criar e colocar client
                    // caso exista, checar o primeiro espaço que não tiver client e setar
                    int j;
                    for(j=0; j<500; j++){
                        if(cdata->tags[j].name)
                       if(cdata->tags[j].clients[0] == NULL){
                           int k;
                           for(k=i; buf[k] != ' '; k++){

                           }
                       }
                    }

                    if(strlen(cdata->tags->clients) > 0){
                        cdata->tags->clients[]
                    }
                } else{
                    logexit("BadMsg: + invalid");
                }
            } else if(buf[i] == '-'){
                if((i == 1) || (buf[i-1] == ' ') || (buf[i+4] == '\n')){
                    
                } else{
                    logexit("BadMsg: - invalid");
                }
            } else if(buf[i] == '#'){
                if((i == 1) || (buf[i-1] == ' ') || (buf[i+4] == '\n')){
                    
                } else{
                    logexit("BadMsg: # invalid");
                }
            }
        }
        memset(buf, 0, BUFSZ);
        buf[0]=2;
        // sprintf(buf, "remote endpoint: %.100s\n", caddrstr);
        count = send(cdata->csock, buf, strlen(buf), 0);
        if (count != strlen(buf)) {
            logexit("send");
        }
    }

    close(cdata->csock);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    int s = initSocketServer(argc, argv, IPv);
    Tag *tags[500];

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        struct client_data *cdata = malloc(sizeof(*cdata));
        if (!cdata) {
            logexit("malloc");
        }
        cdata->csock = csock;
        cdata->tags = &tags;
        
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        send1(csock);

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }

    exit(EXIT_SUCCESS);
}
