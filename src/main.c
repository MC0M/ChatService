#include <main.h>

void cleanup_server(server_info_t *info)
{
    // Close the server socket
    close(info->server_sock);
}

void *client_thread(void *arg)
{
    server_info_t *info = (server_info_t *) arg;
    debug("Accepted connection from client.");
    while(info->current_node.socket > 0)
    {
        talk_to_client(info);
    }

    pthread_exit(NULL);
}

void handle_client_join(server_info_t *info, chat_node_t client)
{
    if(client.joined == 0)
    {
        log_info("User joined: %s", client.name);
        client.joined = 1;
        info->array = add_node(info->array, &client);
    }
    else if(client.joined == -1)
    {
        int idx = get_index(info->array, client);
        log_info("User rejoined: %s", client.name);
        client.joined = 1;
        strcpy(info->array[idx].address, client.address);
        strcpy(info->array[idx].name, client.name);
        info->array[idx].joined = client.joined;
        info->array[idx].socket = client.socket;
        info->array[idx].port = client.port;
    }
}

void handle_client_leave(server_info_t *info, chat_node_t client)
{
    chat_node_t *found_node = find_node(info->array, client.address, client.name, client.port);
    if(found_node)
    {
        if(found_node->joined == 1)
        {
            int idx = get_index(info->array, client);
            log_info("User left: %s", client.name);
            info->array[idx].joined = -1;
        }
    }
}

void handle_client_shutdown(server_info_t *info, chat_node_t client)
{
    handle_client_leave(info, client);
}

void handle_client_shutdown_all(server_info_t *info)
{
    size_t size = info->array[0].size;
    for(int i = 0; i < size; i++)
    {
        chat_node_t node = info->array[i];
        if(node.joined > 0)
        {
            handle_client_shutdown(info, node);
        }
    }
}

void init_server(server_info_t *info)
{
    pthread_t *client_thread_ids = (pthread_t *) malloc(NODE_BACKLOG * sizeof(pthread_t));
    int thread_idx = 0;
    if((listen(info->server_sock, NODE_BACKLOG)) < 0)
    {
        log_err("Listen failed.");
        exit(EXIT_FAILURE);
    }

    while(info->server_sock > 0)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(info->server_sock, (struct sockaddr *) &client_addr, &client_addr_len);
        if(client_socket <= 0)
        {
            log_warn("Accept failed.");
            continue;
        }

        info->current_node.socket = client_socket;
        pthread_t client_thread_id = client_thread_ids[thread_idx];
        if((pthread_create(&client_thread_id, NULL, client_thread, (void *) info)) != 0)
        {
            log_warn("Failed to create thread, continuing...");
            continue;
        }

        thread_idx += 1;
        if(thread_idx > NODE_BACKLOG)
        {
            client_thread_ids = realloc(client_thread_ids, (thread_idx + NODE_BACKLOG) * sizeof(pthread_t));
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        log_err("Not enough arguments. Usage: %s <properties file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    Properties *properties = property_read_properties(argv[1]);
    short port = atoi(property_get_property(properties, "port"));

    server_info_t *info = new_server(port);

    init_server(info);

    cleanup_server(info);
}

server_info_t *new_server(short port)
{
    struct sockaddr_in server_addr;
    server_info_t *info = (server_info_t *) malloc(sizeof(server_info_t));
    chat_node_t *server_node = new_node("127.0.0.1", "Server", port, 1);
    server_node->joined = -1;
    info->array = server_node;

    info->server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(info->server_sock < 0)
    {
        log_err("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    if((setsockopt(info->server_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) < 0)
    {
        log_err("Failed to make address reusable.");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if((bind(info->server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0)
    {
        log_err("Socket bind failed.");
        exit(EXIT_FAILURE);
    }

    if((pthread_mutex_init(&info->lock, NULL)) != 0)
    {
        log_err("Mutex init failed.");
        exit(EXIT_FAILURE);
    }

    printf("Server created successfully on port %hi\n", port);
    return info;
}
