#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *str);

void send1(int s, unsigned int tam);

char send2(int s);

void send3(int s, int count, int* pos);

void send4(int s);

unsigned char recvByte(int s);

int recvAnswer(int s, int max, char* word, char palpite);

int charFind(char c, char* s, int len, int* pos);
