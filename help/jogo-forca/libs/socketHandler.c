#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TAM 2
#define BUFSZ 128

void _logexit(const char *str) {
	perror(str);
	exit(EXIT_FAILURE);
}

void _usage(char* name) {
    printf("usage: %s <ip-servidor> <porta-servidor>\n", name);
    printf("example: %s: 127.0.0.1 51511\n", name);
    exit(EXIT_FAILURE);
}

void _usage2(char *name) {
    printf("usage: %s <porta>\n", name);
    printf("example: %s: 51511\n", name);
    exit(EXIT_FAILURE);
}

int addrparse(const char*addrstr, const char *portstr, struct sockaddr_storage *storage) {
    if(addrstr == NULL || portstr == NULL) return -1;
    
    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0) return -1;
    port = htons(port);

    struct in_addr inaddr4;
    if(inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6;
    if(inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN+1] = "";
    uint16_t port;
    if(addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if(!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1)) _logexit("ntop");
        port = ntohs(addr4->sin_port);

    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if(!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1)) _logexit("ntop");
        port = ntohs(addr6->sin6_port);

    } else _logexit("addrtostr unkown protocol family");

    if(str) snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
}

int server_sockaddr_init(const int proto, const char *portstr, struct sockaddr_storage *storage) {

    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0) return -1;
    port = htons(port);

    memset(storage, 0, sizeof(*storage));
    if (proto == 4){
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;

    } else if (proto == 6){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    }
    return -1;
}

int checkIpAddress(char *ipAddress)
{
    struct sockaddr_in6 sa6;
    int result = inet_pton(AF_INET, ipAddress, &(sa6.sin6_addr));
    if (result != 0) return 6;

    struct sockaddr_in sa;
    result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0) return 4;

    return 0;
}

int initSocketServer(int argc, char **argv, int proto){
    if (argc < 2) _usage2(argv[0]);

    struct sockaddr_storage storage;
    if(0 != server_sockaddr_init(proto, argv[1], &storage)) _usage2(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) _logexit("socket");

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        _logexit("setsockopt");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != bind(s, addr, sizeof(storage))) _logexit("bind");
    if(0 != listen(s, 10)) _logexit("listen");
    
    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    // printf("bound to %s, waiting connection\n", addrstr);
    printf("[log] waiting connection\n");
    return s;
}

int acceptConnection(int s) {
    struct sockaddr_storage *cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int csock = accept(s, caddr, &caddrlen);
    if(csock == -1) _logexit("accept");

    char caddrstr[BUFSZ];
    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log] connection from %s\n", caddrstr);
    return csock;
}

int connectToServer(int argc, char **argv) {
    if (argc < 3) _usage(argv[0]);
    struct sockaddr_storage storage;
    if(0 != addrparse(argv[1], argv[2], &storage)) _usage(argv[0]);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) _logexit("socket");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(0 != connect(s, addr, sizeof(storage))) _logexit("connect");

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    // printf("Connected to %s\n", addrstr);
    return s;
}