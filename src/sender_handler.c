#include <sender_handler.h>

void *handle_sender(void *arg)
{
    //Get client information
    client_info_t *info = (client_info_t *) arg;

    //Socket for sending data
    int server_socket = 0;
    struct addrinfo *addr = info->sender_addr;

    //Try and make a socket
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_err("Failed to create socket for connection on %s: %hi", info->hostname, info->sender_port);
        pthread_exit(NULL);
    }

    //Now try to connect to the server
    if((connect(server_socket, addr->ai_addr, addr->ai_addrlen)) != 0)
    {
        log_err("Failed to connect to server on %s: %hi\n", info->hostname, info->sender_port);
        pthread_exit(NULL);
    }

    //For getting receiver address from client
    char address[INET_ADDRSTRLEN];

    //Loop until a disconnect occurrs
    while(server_socket > 0)
    {
        //Create an empty message
        message_t *msg = (message_t *) malloc(sizeof(message_t));
        //Prompt the user for input
        printf("%s: ", info->name);
        fgets(msg->note, MAX_NOTE_LENGTH, stdin);

        //Check if the user wants to join a server
        if(sscanf(msg->note, "JOIN %s %hi", address, &info->receiver_port) == 2)
        {
            //Copy client name into message and define message type
            snprintf(msg->node.name, MAX_NAME_LENGTH, "%s", info->name);
            msg->type = JOIN;
            msg->node.port = info->receiver_port;

            //Copy receiver address into message's node
            snprintf(msg->node.address, INET_ADDRSTRLEN, "%s", address);
            snprintf(info->receiver_addr, INET_ADDRSTRLEN, "%s", address);

            //And send the message to the server
            broadcast_once(server_socket, msg);
        }
        //Otherwise check if the user wants to leave the chat
        else if((strncmp(msg->note, "LEAVE", strlen("LEAVE"))) == 0)
        {
            //Copy the name into the message and define the message type
            snprintf(msg->node.name, MAX_NAME_LENGTH, "%s", info->name);
            msg->type = LEAVE;
            msg->node.port = info->receiver_port;

            //Store the receiver's address in message node
            snprintf(msg->node.address, INET_ADDRSTRLEN, "%s", address);
            snprintf(info->receiver_addr, INET_ADDRSTRLEN, "%s", address);

            //And send the message to the server
            broadcast_once(server_socket, msg);
        }
        //Check if the user wants to shut down their connection
        else if((strncmp(msg->note, "SHUTDOWN", strlen("SHUTDOWN"))) == 0)
        {
            //Copy the name into the message and define the message type
            snprintf(msg->node.name, MAX_NAME_LENGTH, "%s", info->name);
            msg->type = SHUTDOWN;
            msg->node.port = info->receiver_port;

            //Store the receiver's address in message node
            snprintf(msg->node.address, INET_ADDRSTRLEN, "%s", address);
            snprintf(info->receiver_addr, INET_ADDRSTRLEN, "%s", address);

            //And send the message to the server
            broadcast_once(server_socket, msg);
        }
        //Check if the user wants to shut down all instances
        else if((strncmp(msg->note, "SHUTDOWN ALL", strlen("SHUTDOWN ALL"))) == 0)
        {
            //Copy the name into the message and define the message type
            snprintf(msg->node.name, MAX_NAME_LENGTH, "%s", info->name);
            msg->type = SHUTDOWN_ALL;
            msg->node.port = info->receiver_port;

            //Store the receiver's address in message node
            snprintf(msg->node.address, INET_ADDRSTRLEN, "%s", address);
            snprintf(info->receiver_addr, INET_ADDRSTRLEN, "%s", address);

            //And send the message to the server
            broadcast_once(server_socket, msg);

            //Now close the socket
            close(server_socket);
        }
        //The user wants to send a note
        else
        {
            //Copy the name into the message and define the message type
            snprintf(msg->node.name, MAX_NAME_LENGTH, "%s", info->name);
            msg->type = NOTE;
            msg->node.port = info->receiver_port;

            //Store the receiver's address in message node
            snprintf(msg->node.address, INET_ADDRSTRLEN, "%s", address);
            snprintf(info->receiver_addr, INET_ADDRSTRLEN, "%s", address);

            //And send the message to the server
            broadcast_once(server_socket, msg);
        }
    }
}