#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <arpa/inet.h>
#include <chat_node.h>
#include <dbg.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct message
{
    int type;
    chat_node_t node;
    char note[MAX_NOTE_LENGTH];
} message_t;

message_t *new_message(int type, chat_node_t node, const char *note);
message_t *read_message(chat_node_t node);
void broadcast_message(chat_node_t *sender, chat_node_t *nodes, message_t msg);
int broadcast_once(int client_socket, message_t *msg);
#endif