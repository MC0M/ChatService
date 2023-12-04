#include <client_handler.h>

void talk_to_client(void *arg)
{
    server_info_t *info = (server_info_t *) arg;
    message_t *msg = read_message(info->current_node);

    while(msg != NULL)
    {
        char server_msg[MAX_NOTE_LENGTH];
        int type = msg->type;
        char *note = msg->note;
        char *address = msg->node.address;
        char *name = msg->node.name;
        short port = msg->node.port;
        chat_node_t *found_node;

        printf("Message received: \n\t");
        printf("Note: %s\n\t", note);
        printf("Name: %s\n\t", name);
        printf("Address: %s\n\t", address);
        printf("Port: %hi\n", port);

        switch(type)
        {
            case JOIN:
                pthread_mutex_lock(&(info->lock));
                printf("User joined...");
                found_node = find_node(info->array, address, name, port);
                msg->node.socket = info->current_node.socket;
                if(found_node)
                {
                    snprintf(msg->note, MAX_NOTE_LENGTH, "User rejoined: %s\n", name);
                }
                else
                {
                    info->array = add_node(info->array, &(msg->node));
                    snprintf(msg->note, MAX_NOTE_LENGTH, "User joined: %s\n", name);
                }
                handle_client_join(info, msg->node);
                broadcast_message(&(msg->node), info->array, *msg);
                pthread_mutex_unlock(&(info->lock));
                break;
                
            case LEAVE:
                pthread_mutex_lock(&(info->lock));
                found_node = find_node(info->array, address, name, port);
                if(found_node)
                {
                    if(found_node->joined == 1)
                    {
                        snprintf(msg->note, MAX_NOTE_LENGTH, "User left: %s\n", name);
                        broadcast_message(&(msg->node), info->array, *msg);
                    }
                    handle_client_leave(info, msg->node);
                }
                pthread_mutex_unlock(&(info->lock));
                break;
                
            case SHUTDOWN:
                pthread_mutex_lock(&(info->lock));
                found_node = find_node(info->array, address, name, port);
                handle_client_shutdown(info, msg->node);
                remove_node(info->array, msg->node);
                pthread_mutex_unlock(&(info->lock));
                break;
                
            case SHUTDOWN_ALL:
                pthread_mutex_lock(&(info->lock));
                handle_client_shutdown_all(info);
                close(info->server_sock);
                pthread_mutex_unlock(&(info->lock));
                break;
            
            case NOTE:
                pthread_mutex_lock(&(info->lock));
                found_node = find_node(info->array, address, name, port);
                if(found_node)
                {
                    if(snprintf(msg->note, (MAX_NOTE_LENGTH + MAX_NAME_LENGTH), "%s: %s", name, note) < 0)
                    {
                        log_err("Failed to create response message, not sending.");
                        break;
                    }

                    broadcast_message(&(msg->node), info->array, *msg);
                }
                pthread_mutex_unlock(&(info->lock));
                break;
                
        }
        //And read another
        msg = read_message(info->current_node);
    }
}