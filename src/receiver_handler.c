#include <receiver_handler.h>

void handle_server_response(int receiver_sock, int server_sock)
{
    //Try and read messages from the server
    chat_node_t server_response_node;
    server_response_node.socket = server_sock;
    message_t *msg = (message_t *) read_message(server_response_node);
    while(msg != NULL)
    {
        //Check message types and run appropriate operations
        switch(msg->type)
        {
            case JOIN:
                //Print the server's message
                printf("%s", msg->note);
                break;
            
            case LEAVE:
                //Print leave message
                printf("%s", msg->note);
                break;

            case NOTE:
                //Print note
                printf("%s", msg->note);
                break;

        }
        //Read the message again
        msg = (message_t *) read_message(server_response_node);
    }
}

void *handle_receiver(void *arg)
{
    //Get client information
    client_info_t *info = (client_info_t *) arg;

    //Socket address for server
    struct sockaddr_in server_addr;
    //Create socket for receiving server
    int receiver_sock = socket(AF_INET, SOCK_STREAM, 0);
    //Make sure that the socket was opened
    if(receiver_sock <= 0)
    {
        //Socket creation failed, print error
        log_err("Socket creation failed.");
        pthread_exit(NULL);
    }

    //Make sure the receiver address can be reused
    if((setsockopt(receiver_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) < 0)
    {
        //Unable to make address reusable, print error
        log_err("Cannot make receiver address reusable.");
        pthread_exit(NULL);
    }

    //Wait until receiver port and address are set
    while(info->receiver_port <= 0);
    while(strncmp("", info->receiver_addr, INET_ADDRSTRLEN) == 0);

    //Setup server address information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(info->receiver_port);
    server_addr.sin_addr.s_addr = inet_addr(info->receiver_addr);

    //Attempt to bind the socket to the receiver address
    if((bind(receiver_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    {
        //Failed to bind to socket, print error
        log_err("Socket bind failed.");
        pthread_exit(NULL);
    }

    //Start listening for connections from server
    if(listen(receiver_sock, NODE_BACKLOG) < 0)
    {
        //Failed to listen for connection, print error
        log_err("Listen failed.");
        pthread_exit(NULL);
    }

    //Loop to get data from server
    while(info->receiver_port > 0)
    {
        struct sockaddr_in server_address;
        socklen_t server_address_length = sizeof(server_address);

        //Accept a connection from the server
        int server_sock = accept(receiver_sock, (struct sockaddr *)&server_address, &server_address_length);

        //Check if accept was successful
        if(server_sock <= 0)
        {
            //Failed to accept, print error and continue
            log_err("Accept failed.");
            continue;
        }

        printf("Accepted connection.\n");
        //Client accepted server connection, run response handler
        handle_server_response(receiver_sock, server_sock);
    }

    //Shutting server down
    pthread_exit(NULL);
}
