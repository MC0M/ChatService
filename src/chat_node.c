#include <chat_node.h>

chat_node_t *new_node(const char *address, const char *name, short port, size_t size)
{
    //Create a new node and add the provided parameters to it
    chat_node_t *node = (chat_node_t *) malloc(size * sizeof(chat_node_t));
    strncpy(node[0].address, address, INET_ADDRSTRLEN);
    strncpy(node[0].name, name, MAX_NAME_LENGTH);
    node[0].port = port;
    node[0].size = size;
    return node;
}


chat_node_t *add_node(chat_node_t *array, chat_node_t *node)
{
    if(array == NULL)
    {
        log_err("Provided node array was not set.");
        return NULL;
    }

    size_t size = array[0].size + 1;
    if(array != NULL && (sizeof(array) < size))
        array = (chat_node_t *) realloc(array, size * sizeof(chat_node_t));
    strcpy(array[size].address, node->address);
    strcpy(array[size].name, node->name);
    array[size].joined = node->joined;
    array[size].socket = node->socket;
    array[size].port = node->port;
    array[size].size = size;

    for(int i = 0; i <= size; i++)
    {
        if(array[i].port <= 0)
        {
            remove_node(array, array[i]);
        }
        
        array[i].size = size;
    }

    return array;
}


bool does_node_match(chat_node_t node, const char *address, const char *name, short port)
{
    return ((strncmp(node.address, address, INET_ADDRSTRLEN)) == 0)
    && ((strncmp(node.name, name, MAX_NAME_LENGTH)) == 0)
    && (node.port == port);
}


chat_node_t *find_node(chat_node_t *array, const char *address, const char *name, short port)
{
    size_t size = array[0].size;
    for(int i = 0; i < size; i++)
    {
        if(does_node_match(array[i], address, name, port))
        {
            return &array[i];
        }
    }

    return NULL;
}


int remove_node(chat_node_t *array, chat_node_t node)
{
    if(array == NULL)
    {
        log_err("Failed to remove chat node, provided array is empty.");
        return FAIL;
    }
    
    size_t size = array[0].size;
    for(int i = 0; i < size; i++)
    {
        chat_node_t current = array[i];
        if(does_node_match(current, node.address, node.name, node.port))
        {
            array[i].joined = -1;
        }

        return SUCCESS;
    }

    return FAIL;
}

int get_index(chat_node_t *array, chat_node_t node)
{
    int idx = 0;
    size_t size = array[0].size;
    for(idx = 0; idx < size; idx++)
    {
        if(does_node_match(array[idx], node.address, node.name, node.port))
        {
            return idx;
        }
    }

    return FAIL;
}