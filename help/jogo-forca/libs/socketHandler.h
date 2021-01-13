#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

int initSocketServer(int argc, char **argv, int proto);

int connectToServer(int argc, char **argv);

int acceptConnection(int s);
