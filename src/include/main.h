//Ibrahim Hmood and Nuria García Carpintero

#ifndef _MAIN_H_
#define _MAIN_H_
#define DBG
#define JOIN            0
#define LEAVE           1
#define MAXLINE         MAX_NOTE_LENGTH
#define NODE_BACKLOG 10
#define NOTE            -1
#define SHUTDOWN        2
#define SHUTDOWN_ALL    3

#include <arpa/inet.h>
#include <chat_node.h>
#include <dbg.h>
#include <message.h>
#include <netdb.h>
#include <netinet/in.h>
#include <properties.h>
#include <pthread.h>
#include <receiver_handler.h>
#include <sender_handler.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


typedef struct s_client_info
{
    char hostname[INET_ADDRSTRLEN];
    char receiver_addr[INET_ADDRSTRLEN];
    short receiver_port;
    struct addrinfo *sender_addr;
    short sender_port;
    char name[MAX_NAME_LENGTH];
    chat_node_t *array;
} client_info_t;

struct addrinfo *resolve(const char *hostname, short port);
int main(int argc, char *argv[]);
#endif