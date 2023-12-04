#include <message.h>


message_t *new_message(int type, chat_node_t node, const char *note)
{
    message_t *msg = (message_t *) malloc(sizeof(message_t));
    msg->type = type;
    strncpy(msg->node.address, node.address, INET_ADDRSTRLEN);
    strncpy(msg->node.name, node.name, MAX_NAME_LENGTH);
    msg->node.joined = node.joined;
    msg->node.socket = node.socket;
    msg->node.port = node.port;
    msg->node.size = node.size;
    strncpy(msg->note, note, MAX_NOTE_LENGTH);

    return msg;
}

message_t *read_message(chat_node_t node)
{
    if(node.socket <= 0)
    {
        log_err("Invalid socket descriptor.");
        return NULL;
    }

    message_t *msg = (message_t *) malloc(sizeof(message_t));
    if((recv(node.socket, &(msg->type), sizeof(msg->type), 0)) < 0)
    {
        log_err("Failed to read message type from socket.");
        return NULL;
    }

    if((recv(node.socket, &(msg->node.address), INET_ADDRSTRLEN, 0)) < 0)
    {
        log_err("Failed to read address from socket.");
        return NULL;
    }

    if((recv(node.socket, &(msg->node.name), MAX_NAME_LENGTH, 0)) < 0)
    {
        log_err("Failed to read name from socket.");
        return NULL;
    }

    if((recv(node.socket, &(msg->node.port), sizeof(msg->node.port), 0)) < 0)
    {
        log_err("Failed to read port from socket.");
        return NULL;
    }

    if((recv(node.socket, &(msg->note), MAX_NOTE_LENGTH, 0)) < 0)
    {
        log_err("Failed to read note from socket.");
        return NULL;
    }

    return msg;
}

void broadcast_message(chat_node_t *sender, chat_node_t *array, message_t msg)
{
    size_t size = array[0].size;
    for(int i = 0; i <= size; i++)
    {
        if(!(does_node_match(array[i], msg.node.address, msg.node.name, msg.node.port)))
        {
            //Send this message once
            broadcast_once(0, &msg);
        }
    }
}

int broadcast_once(int client_socket, message_t *msg)
{
    int sock;
    short port = msg->node.port;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(msg->node.address);

    if(client_socket > 0)
    {
        sock = client_socket;
    }
    else
    {
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            log_err("Failed to create socket, cannot respond to %s:%hi", msg->node.address, msg->node.port);
            return FAIL;
        }
        if(connect(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
        {
            log_err("Failed to connect to %s:%hi", msg->node.address, msg->node.port);
            return FAIL;
        }
    }

    //type, address, name, port, note
    if((send(sock, &(msg->type), sizeof(msg->type), 0)) < 0)
    {
        log_err("Failed to send message type.");
        return FAIL;
    }

    if((send(sock, &(msg->node.address), INET_ADDRSTRLEN, 0)) < 0)
    {
        log_err("Failed to send address.");
        return FAIL;
    }

    if((send(sock, &(msg->node.name), MAX_NAME_LENGTH, 0)) < 0)
    {
        log_err("Failed to send name.");
        return FAIL;
    }

    if((send(sock, &(msg->node.port), sizeof(msg->node.port), 0)) < 0)
    {
        log_err("Failed to send port.");
        return FAIL;
    }

    if((send(sock, &(msg->note), MAX_NOTE_LENGTH, 0)) < 0)
    {
        log_err("Failed to send note.");
        return FAIL;
    }

    return SUCCESS;
}
