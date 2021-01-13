#include "libs/common.h"
#include "libs/socketHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char **argv) {
    int s = connectToServer(argc, argv);

    int tam = (int)recvByte(s);
    printf("\nNúmero de caracteres: %d\n", tam);
    char word[tam];
    memset(word,'_',tam);

    while(1){
        char palpite = (char)send2(s);
        if(recvAnswer(s, tam, word, palpite) == 1) break;
        printf("%s\n", word);
    }
    printf("\nVocê conseguiu!\nA palavra é: %s\n", word);
    close(s);
    exit(EXIT_SUCCESS);
    return 0;
}