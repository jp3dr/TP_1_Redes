#include "libs/common.h"
#include "libs/socketHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define IPv 4
#define WORD "arara"

int main(int argc, char **argv) {
    int s = initSocketServer(argc, argv, IPv);
    unsigned int tam = strlen(WORD);
    
    while(1){
        int csock = acceptConnection(s);
        send1(csock, tam);

        int charsFound = 0; // Contador de caracters totais encontrados
        int triesCounter = 0; // Contador de palpites tentados (sem repetição)
        char charsTried[100]; // Registro de palpites tentados
        memset(charsTried, ' ', 100);
        int pos[tam], count; // Posições encontradas e contador
        unsigned char palpite;

        while(1){
            memset(pos, -1, tam); // Reseta as posições
            palpite = (char)recvByte(csock); // Recebe o novo palpite
            count = charFind(palpite, WORD, tam, pos); // Calcula quantas ocorrências e preenche as posições

            if(count > 0 && strchr(charsTried, palpite) == NULL) { // Se encontrou algum novo caracter
                charsTried[triesCounter] = palpite;
                triesCounter ++;
                charsFound += count;
                if (charsFound >= tam) break; // Se encontrou todos, finaliza
            }

            send3(csock, count, pos);
        }
        send4(csock);
        close(csock);
        printf("[log] connection closed. Waiting new connection\n");

    }
    exit(EXIT_SUCCESS);
    return 0;
}