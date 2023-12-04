//Ibrahim Hmood and Nuria García Carpintero

#ifndef _MAIN_H_
#define _MAIN_H_
#define DBG
#define NODE_BACKLOG 10
#define JOIN            0
#define LEAVE           1
#define NOTE            -1
#define SHUTDOWN        2
#define SHUTDOWN_ALL    3

#include <arpa/inet.h>
#include <chat_node.h>
#include <client_handler.h>
#include <dbg.h>
#include <message.h>
#include <netdb.h>
#include <properties.h>
#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct server_info
{
    int server_sock;
    short port;
    chat_node_t *array;
    chat_node_t current_node;
    pthread_mutex_t lock;
} server_info_t;

void cleanup_server(server_info_t *info);
void *client_thread(void *arg);
void handle_client_join(server_info_t *info, chat_node_t client);
void handle_client_leave(server_info_t *info, chat_node_t client);
void handle_client_shutdown(server_info_t *info, chat_node_t client);
void handle_client_shutdown_all(server_info_t *info);
void init_server(server_info_t *info);
int main(int argc, char *argv[]);
server_info_t *new_server(short port);
#endif