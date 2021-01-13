#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <inttypes.h>
#include <arpa/inet.h>

#define TAM 2

void logexit(const char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

char _upper(char c){
    if(c<96) return c;
    else return c-32;
}

void send1(int s, unsigned int tam) {
    unsigned char buf[TAM]; 
    buf[0] = 1;
    buf[1] = (unsigned char)tam;
    if(send(s, buf, TAM, 0) != TAM) logexit("send");
}

char send2(int s) {
    printf("\nPalpite> ");
    unsigned char buf[TAM]; 
    buf[0] = 2;
    buf[1] = getchar();
    getchar(); // must be here due to the '\n'
    if(send(s, buf, TAM, 0) != TAM) logexit("send");
    return buf[1];
}

void send3(int s, int count, int* pos) {
    unsigned char buf[2+count]; 
    buf[0] = 3;
    buf[1] = count;
    for(int i=0; i<count; i++){
        buf[2+i]=pos[i];
    }
    if(send(s, buf, 2+count, 0) != 2+count) logexit("send");
}

void send4(int s) {
    unsigned char buf[1]; 
    buf[0] = 4;
    if(send(s, buf, 1, 0) != 1) logexit("send");
}

unsigned char recvByte(int s){
    unsigned char buf[TAM]; 
    memset(buf, 0, TAM);
    size_t count = recv(s, buf, TAM, 0);
    if(TAM != (int)count) logexit("recvByte");
    return buf[1];
}

int recvAnswer(int s, int tam, char* word, char palpite) {
    unsigned char buf[tam+2];
    
    if(recv(s, buf, 1, 0) != 1) logexit("recvByte");
    if((int)buf[0] == 4) {
        for(int i = 0; i <= tam; i++)
  		    if(word[i] == '_')
  			    word[i] = _upper(palpite);
        return 1;
    }

    if(recv(s, buf+1, 1, 0) != 1) logexit("recvByte");
    int occ = (int)buf[1];
    if(occ == 0) return 0;

    if(recv(s, buf+2, occ, 0) != occ) logexit("recvByte");
    for(int i=0; i<occ; i++){
        int atIndex = (int)buf[2+i];
        word[atIndex] = _upper(palpite);
    }
    return 0;
}

int charFind(char c, char* s, int len, int* pos){
    int count = 0;
    for(int i=0; i<len; i++) {
        if(_upper(s[i]) == _upper(c)) {
            pos[count]=i;
            count++;
        }
    }
    return count;
}
