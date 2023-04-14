#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define DEFAULT_SERVER_PORT 9000
#define DEFAULT_BUFFER_SIZE 50000
#define DEFAULT_MAX_CLIENTS 10

int server_sock;                        // server socket descriptor
struct sockaddr_in server_addr;         // server address structure
int client_socks[DEFAULT_MAX_CLIENTS];  // client socket descriptors
char* client_usernames[DEFAULT_MAX_CLIENTS]; // usernames of clients

int SERVER_PORT = DEFAULT_SERVER_PORT;      // server port number
int BUFFER_SIZE = DEFAULT_BUFFER_SIZE;      // buffer size for messages
int MAX_CLIENTS = DEFAULT_MAX_CLIENTS;      // maximum number of clients

void cleanup()
{
    // close all client sockets
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_socks[i] != -1)
        {
            close(client_socks[i]);
            client_usernames[i] = "";
        }
    }
    // close server socket
    close(server_sock);
}

// Signal handler function
void signalHandler(int signal) {
    // Handle the signal here
    printf("Received signal: %d\n", signal);
    cleanup();
    exit(0);
}

void handle_client_request(char* client_request, int client_index)
{
    // Extract the first token
    char* command = strtok(client_request, " ");
    char** token;
    int j = 0;
    while( token != NULL ) {
      token[j] = strtok(NULL, " ");
      j++;
   }
    if(strcmp(command, "login") == 0)
    {
        
        printf("logging in client id:%d, socket:%d username: %s\n", 
        client_index, client_socks[client_index], token[0]);
    }
    else if(strcmp(command, "logout") == 0) 
    {
        printf("logging out client username: %s id:%d, socket:%d\n", 
        client_usernames[client_index], client_index, client_socks[client_index]);
        client_usernames[client_index] = "";
    }
    else if(strcmp(command, "chat") == 0) 
    {
        printf("chat yet to be implemented");
    }
}

void setup_server()
{
    // create socket
    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    int options = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options))) {
        perror("Error setting SO_REUSEADDR to socket");
        exit(EXIT_FAILURE);
    }

    // setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // bind socket to server address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket to server address");
        exit(1);
    }

    // listen for incoming connections
    if (listen(server_sock, MAX_CLIENTS) < 0)
    {
        perror("Error listening for incoming connections");
        exit(1);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // initialize client_socks array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_socks[i] = -1;
    }
}

void handle_clients()
{   
    // infinite loop
    while (1)
    {
        fd_set read_fds;
        int max_fd, activity;
        struct timeval timeout;
        char buffer[BUFFER_SIZE];
        int i, valread;

        FD_ZERO(&read_fds);
        FD_SET(server_sock, &read_fds);
        max_fd = server_sock;

        // add client sockets to read_fds
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_socks[i];
            if (sd != -1)
            {
                FD_SET(sd, &read_fds);
                if (sd > max_fd)
                {
                    max_fd = sd;
                }
            }
        }

        // set timeout
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;

        // wait for activity on any of the sockets i.e. select()
        activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0)
        {
            perror("Error selecting sockets");
            exit(1);
        }

        if (activity == 0)
        {
            // timeout occurred, do something if needed
            continue;
        }

        // check for new connection i.e. accept()
        if (FD_ISSET(server_sock, &read_fds))
        {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
            if (new_sock < 0)
            {
                perror("Error accepting connection");
                exit(1);
            }

            // add new client socket to the array
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_socks[i] == -1)
                {
                    client_socks[i] = new_sock;
                    printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    break;
                }
            }

            // check if maximum clients reached
            if (i == MAX_CLIENTS)
            {
                printf("Maximum clients reached. Closing new connection...\n");
                close(new_sock);
            }
        }

        // check for incoming data from clients
        for (i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_socks[i];
            if (FD_ISSET(sd, &read_fds))
            {
                valread = recv(sd, buffer, BUFFER_SIZE, 0);
                if (valread == 0)
                {
                    // client closed socket
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    getpeername(sd, (struct sockaddr *)&client_addr, &client_addr_len);
                    printf("Client disconnected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(sd);
                    client_socks[i] = -1;
                    client_usernames[i] = "";
                } 
                else if (valread < 0)
                {
                    // client socket error
                    perror("Error receiving data from client");
                    fprintf(stderr, "Disconnecting client id: %d, socket: %d", i, sd);
                    close(sd);
                    client_socks[i] = -1;
                    client_usernames[i] = "";
                } 
                else
                {
                    // process received data
                    buffer[valread] = '\0';
                    printf("Received data from client %d: %s\n", sd, buffer);
                    //char* client_request;
                    //memcpy(client_request, buffer, sizeof(buffer));
                    handle_client_request(buffer, i);
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        fprintf(stderr, "usage: ./server server_config_filename");
        exit(1);
    }
    // read server config
    FILE* f = fopen(argv[1],"r");
    if (f) {
        fscanf(f, "port=%d\n", &SERVER_PORT);
        fclose(f);
    } else {
        fprintf(stderr, "Error reading server configuration file");
        exit(1);
    }

    signal(SIGINT, signalHandler);

    // setup server
    setup_server();

    // handle clients
    handle_clients();

    // cleanup and exit
    cleanup();
    return 0;
}