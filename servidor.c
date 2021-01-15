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

typedef struct reg {
    char *name;
    char *clients[100]; // numero limitado de clients por tag so pra não fazer
                        // duas listas encadeadas
    struct reg *prox;
} Tag;

struct client_data {
    int csock;
    struct sockaddr_storage storage;
    Tag *tags;
};

Tag *busca(char *x, Tag *le) {
    Tag *p;
    p = le;
    while (p != NULL && strcmp(p->name, x))
        p = p->prox;
    return p;
}

void insere(char *x, Tag *p) {
    Tag *nova;
    Tag *aux;
    aux = p;
    nova = malloc(sizeof(Tag));
    nova->name = x;
    nova->prox = NULL;

    int i;
    for (i = 0; i < 100; i++) {
        nova->clients[i] = "0";
    }

    if (p->name != " ") {
        p = nova;
    } else {
        while (aux->prox != NULL)
            aux = aux->prox;
        aux->prox = nova;
    }
    printf("p: %s\n", aux->clients[5]);
    printf("p: %s\n", aux->name);
}

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}



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

        int j;
        int l;

        size_t tam = strlen(buf);
        char tagAux[tam];
        printf("tam: %d\n", (int)tam);
        Tag *TagAux;

        if (buf[0] == '+') {
            // checa se está no início ou precedido de espaço
            memset(tagAux, 0, tam);

            // verifica se tag é válida e copia para tagAux
            for (j = 0; j < tam; j++) {

                tagAux[j] = buf[j];
                if (buf[j] == '#' || buf[j] == '-' ||
                    (buf[j] == '+' && j != 0)) {
                    logexit("BadMsg: put space between new tag call");
                }
            }

            // ve se tag já foi criada
            TagAux = busca(tagAux, cdata->tags);

            memset(buf, 0, BUFSZ);
            char *msg;

            if (TagAux != NULL) {
                // ve todos os clientes cadastrados na tag
                for (l = 0; l < 100; l++) {
                    // se elemento está vazio
                    if (strcmp(TagAux->clients[l], "0")) {
                        TagAux->clients[l] = (char *)clientPort;
                        printf("%s\n", TagAux->clients[l]);
                        msg = "subscribed ";
                        break;
                    }
                    // se client já está nessa tag
                    else if (strcmp(TagAux->clients[l], (char *)clientPort)) {
                        printf("strcmp: %s\n", TagAux->clients[l]);
                        msg = "already subscribed ";
                        break;
                    } else if (l == 99) {
                        // tag lotada
                        msg = "tag lotada ";
                    }
                }
            } else {
                
                printf("name T0: %s\n", cdata->tags->name);
                insere(tagAux + 1, &cdata);
                printf("p1: %s\n", cdata->tags->clients[5]);
                printf("p1: %s\n", cdata->tags->name);
                msg = "subscribed ";
            }

            snprintf(buf, BUFSZ, "%d%s%s", 3, msg, tagAux);
            printf("%s\n", buf);
        }
        //  else if (buf[i] == '-') {
        //     if ((i == 1) || (buf[i - 1] == ' ') || (buf[i + 4] == '\n'))
        //     {

        //     } else {
        //         logexit("BadMsg: - invalid");
        //     }
        // } else if (buf[i] == '#') {
        //     if ((i == 1) || (buf[i - 1] == ' ') || (buf[i + 4] == '\n'))
        //     {
        //         if ((i == 0) || (buf[i - 1] == ' ')) {
        //         }
        //     } else {
        //         logexit("BadMsg: # invalid");
        //     }
        // }

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
    Tag* tags;
    tags->name = "aids";
    tags->prox = NULL;
    // printf("name T0: %s\n", tags->name);
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
        cdata->tags = tags;
        memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));

        send1(csock);

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, &cdata);
    }

    exit(EXIT_SUCCESS);
}
