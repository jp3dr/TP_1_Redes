#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

void logexit(const char *msg);

int initSocketServer(int argc, char **argv, int proto);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);

int connectToServer(int argc, char **argv);