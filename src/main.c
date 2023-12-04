#include <main.h>

struct addrinfo *resolve(const char *hostname, short port)
{
    char service[MAXLINE];
    struct addrinfo hints, *res;

    //Convert port to string
    sprintf(service, "%i", port);

    //Prepare hints to be used in DNS query
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    //Query DNS for address, return null if unavailable
    if((getaddrinfo(hostname, service, &hints, &res)) != 0)
    {
        log_err("Failed to resolve address %s:%i\n", hostname, port);
        exit(EXIT_FAILURE);
    }

    //Return the resolved address
    return res;
}

int main(int argc, char *argv[])
{
    char *hostname;
    client_info_t *info;
    short port;
    pthread_t receiver_thread, sender_thread;

    //Check if enough arguments have been passed
    if(argc != 2)
    {
        //Not enough or too much arguments, print error
        log_err("Invalid arguments. Usage: %s <properties file>.", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Read server connection data from properties file
    Properties *properties = property_read_properties(argv[1]);

    //Read the hostname and port from properties
    hostname = property_get_property(properties, "hostname");
    port = atoi(property_get_property(properties, "port"));

    //Now, initialize our client information instance
    info = (client_info_t *) malloc(sizeof(client_info_t));

    //And add client's name to it
    snprintf(info->name, MAX_NAME_LENGTH, property_get_property(properties, "name"));

    //Now, resolve the hostname and port
    info->sender_addr = resolve(hostname, port);

    //And copy hostname and port over
    snprintf(info->hostname, INET_ADDRSTRLEN, hostname);
    info->sender_port = port;

    //Handle messages from the server using receiver thread
    if((pthread_create(&receiver_thread, NULL, handle_receiver, (void *) info)) != 0)
    {
        log_err("Failed to create receiving thread.");
        exit(EXIT_FAILURE);
    }

    //Handle sending messages to server using sender thread
    if((pthread_create(&sender_thread, NULL, handle_sender, (void *) info)) != 0)
    {
        log_err("Failed to create sending thread.");
        exit(EXIT_FAILURE);
    }

    //Prevent exiting main thread
    pthread_join(receiver_thread, NULL);
    pthread_join(sender_thread, NULL);
}
