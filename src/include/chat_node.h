#ifndef _CHAT_NODE_H_
#define _CHAT_NODE_H_
#define FAIL -1
#define MAX_NAME_LENGTH 64
#define MAX_NOTE_LENGTH 1024
#define SUCCESS 0
#include <arpa/inet.h>
#include <dbg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct chat_node
{
    char address[INET_ADDRSTRLEN];
    char name[MAX_NAME_LENGTH];
    int joined, socket;
    short port;
    size_t size;
} chat_node_t;

chat_node_t *new_node(const char *address, const char *name, short port, size_t size);
chat_node_t *add_node(chat_node_t *array, chat_node_t *node);
bool does_node_match(chat_node_t node, const char *address, const char *name, short port);
chat_node_t *find_node(chat_node_t *array, const char *address, const char *name, short port);
int remove_node(chat_node_t *array, chat_node_t node);
void debug_array(chat_node_t *array);
int get_index(chat_node_t *array, chat_node_t node);
#endif