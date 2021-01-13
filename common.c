#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#define BUFSZ 501

void logexit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void _usage(char *name) {
    printf("usage: %s <ip-servidor> <porta-servidor>\n", name);
    printf("example: %s: 127.0.0.1 51511\n", name);
    exit(EXIT_FAILURE);
}

void _usage2(char *name) {
    printf("usage: %s <porta>\n", name);
    printf("example: %s: 51511\n", name);
    exit(EXIT_FAILURE);
}

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));

    struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_addr.s_addr = INADDR_ANY;
    addr4->sin_port = port;
    return 0;
}

int initSocketServer(int argc, char **argv, int proto) {
    if (argc < 2)
        _usage2(argv[0]);

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(proto, argv[1], &storage))
        _usage2(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
        logexit("socket");

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        logexit("setsockopt");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage)))
        logexit("bind");
    if (0 != listen(s, 10))
        logexit("listen");

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    // printf("bound to %s, waiting connection\n", addrstr);
    printf("[log] waiting connection\n");
    return s;
}

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) {
        return -1;
    }

    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    struct in_addr inaddr4; // 32-bit IP address
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    version = 4;
    struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
    if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr,
                   INET6_ADDRSTRLEN + 1)) {
        logexit("ntop");
    }
    port = ntohs(addr4->sin_port); // network to host short

    if (str) {
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }


int connectToServer(int argc, char **argv) {
    if (argc < 3)
        _usage(argv[0]);
    struct sockaddr_storage storage;
    if (0 != addrparse(argv[1], argv[2], &storage))
        _usage(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
        logexit("socket");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != connect(s, addr, sizeof(storage)))
        logexit("connect");

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("Connected to %s\n", addrstr);
    return s;
}