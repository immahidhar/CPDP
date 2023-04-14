#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define DEFAULT_SERVER_PORT 9000
#define DEFAULT_BUFFER_SIZE 1024
#define DEFAULT_MAX_CLIENTS 10

int server_sock;                            // server socket descriptor
struct sockaddr_in server_addr;             // server address structure
int client_socks[DEFAULT_MAX_CLIENTS];      // client socket descriptors
char* client_usernames[DEFAULT_MAX_CLIENTS];// usernames of clients

int SERVER_PORT = DEFAULT_SERVER_PORT;      // server port number
int BUFFER_SIZE = DEFAULT_BUFFER_SIZE;      // buffer size for messages
int MAX_CLIENTS = DEFAULT_MAX_CLIENTS;      // maximum number of clients

void printUsernames() 
{
    printf("\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        printf("%s\n", client_usernames[i]);
    }
    printf("\n");
}

void cleanup(void)
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
void signalHandler(int signal) 
{
    // Handle the signal here
    printf("Received signal: %d\n", signal);
    cleanup();
    exit(0);
}

void chat(char* chat_token, int client_index)
{
    //printUsernames();
    //printf("chat token: %s\n", chat_token);

    // check if user is logged in
    if(strcmp(client_usernames[client_index], "") == 0)
    {
        // user isn't logged in
        // send back response
        char* response = "server >> User is not logged in to chat.";
        printf("%s\n", response);
        int sr = send(client_socks[client_index], response, strlen(response), 0);
        if(sr < 0)
        {
            fprintf(stderr, "Error broadcasting data to client id: %d, socket: %d\n", 
            client_index, client_socks[client_index]);
            return;
        }
        return;
    }

    // check if chat or broadcast
    if(chat_token[0] == '@')
    {
        // private chat
        chat_token = &chat_token[1];
        int ci;
        for(ci = 0; ci <= strlen(chat_token); ci++) 
        {
            if(chat_token[ci] == ' ') 
            {
                break;
            }
        }
        char* username = (char*)malloc(sizeof(char)*(ci+1));
        memcpy(username, chat_token, ci+1);
        username[ci] = '\0';
        printf("chat need to be sent to %s, searching ...\n", username);

        // search for username
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if(strcmp(client_usernames[i], "") != 0)
            {
                if(strcmp(client_usernames[i], username) == 0)
                {
                    printf("found user %s\n", username);
                    char* message = malloc(sizeof(char)*(strlen(chat_token)-ci));
                    message = realloc(message, sizeof(message) + strlen(client_usernames[client_index]) + 3);
                    strcpy(message, client_usernames[client_index]);
                    strcat(message, " >> ");
                    strcat(message, &chat_token[ci+1]);
                    //printf("chat message: %s\n", message);
                    // send chat message
                    int sr = send(client_socks[i], message, strlen(message), 0);
                    if(sr < 0)
                    {
                        fprintf(stderr, "Error sending chat data to client id: %d, socket: %d\n", 
                        i, client_socks[i]);
                    }
                    return;
                }
            }
        }

        // no user found
        // send back response
        char* response = "server >> No user by that username to chat.";
        printf("%s\n", response);
        int sr = send(client_socks[client_index], response, strlen(response), 0);
        if(sr < 0)
        {
            fprintf(stderr, "Error broadcasting data to client id: %d, socket: %d\n", 
            client_index, client_socks[client_index]);
            return;
        }

    }
    else
    {
        // broadcast to all clients except the sender
        char* username = client_usernames[client_index];
        char* broadcast = malloc(strlen(username) + 1);
        strcpy(broadcast, username);
        broadcast = realloc(broadcast, sizeof(broadcast) + sizeof(chat_token) + 3);
        strcat(broadcast, " >> ");
        strcat(broadcast, chat_token);
        printf("%s\n", broadcast);
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_socks[i];
            if(sd == -1 || i == client_index)
            {
                 continue;
            }
            else if(strcmp(client_usernames[i], "") != 0)
            {
                // send broadcast message
                int sr = send(client_socks[i], broadcast, strlen(broadcast), 0);
                if(sr < 0)
                {
                    fprintf(stderr, "Error broadcasting data to client id: %d, socket: %d\n", 
                    i, client_socks[i]);
                }
            }

        }
    }
}

void handle_client_request(char* client_request, int client_index)
{
    // Extract the first token
    int ci;
    for(ci = 0; ci <= strlen(client_request); ci++) 
    {
        if(client_request[ci] == ' ') 
        {
            break;
        }
    }
    char command[ci+1];
    memcpy(command, client_request, ci+1);
    command[ci] = '\0';
    //printf("%s\n", command);
    if(strcmp(command, "login") == 0)
    {
        char* username = &client_request[ci+1];
        //printf("%s\n", username);
        client_usernames[client_index] = username;
        printf("logging in client id:%d, socket:%d username: %s\n", 
        client_index, client_socks[client_index], client_usernames[client_index]);
        //printUsernames();
        // send back response
        char* response = "server >> login successful";
        int sr = send(client_socks[client_index], response, strlen(response), 0);
        if(sr < 0)
        {
            fprintf(stderr, "Error broadcasting data to client id: %d, socket: %d\n", 
            client_index, client_socks[client_index]);
        }
    }
    else if(strcmp(command, "logout") == 0) 
    {
        printf("logging out client username: %s id:%d, socket:%d\n", 
        client_usernames[client_index], client_index, client_socks[client_index]);
        client_usernames[client_index] = "";
        // send back response
        char* response = "server >> logout successful";
        int sr = send(client_socks[client_index], response, strlen(response), 0);
        if(sr < 0)
        {
            fprintf(stderr, "Error broadcasting data to client id: %d, socket: %d\n", 
            client_index, client_socks[client_index]);
        }
    }
    else if(strcmp(command, "chat") == 0) 
    {
        char* chat_token = &client_request[ci+1];
        chat(chat_token, client_index);
        // send back response
    }
    else
    {
        fprintf(stderr, "unknown command received on client username: %s id:%d, socket:%d\n", 
        client_usernames[client_index], client_index, client_socks[client_index]);
        // send back response
        char* response = "server >> Unknown command received.";
        int sr = send(client_socks[client_index], response, strlen(response), 0);
        if(sr < 0)
        {
            fprintf(stderr, "Error broadcasting data to client id: %d, socket: %d\n", 
            client_index, client_socks[client_index]);
        }
    }
}

void setup_server(void)
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
        client_usernames[i] = "";
    }
}

void handle_clients(void)
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
                    printf("Client disconnected: %s:%d id: %d socket: %d\n", inet_ntoa(client_addr.sin_addr), 
                    ntohs(client_addr.sin_port), i, client_socks[i]);
                    close(sd);
                    client_socks[i] = -1;
                    client_usernames[i] = "";
                } 
                else if (valread < 0)
                {
                    // client socket error
                    fprintf(stderr, "Error receiving data from client\n");
                    fprintf(stderr, "Disconnecting client id: %d, socket: %d\n", i, sd);
                    close(sd);
                    client_socks[i] = -1;
                    client_usernames[i] = "";
                } 
                else
                {
                    // process received data
                    buffer[valread] = '\0';
                    printf("Received data from client %d: %s\n", sd, buffer);
                    char* client_request = (char*) malloc(sizeof(char)*valread);
                    strcpy(client_request, buffer);
                    //printf("%s\n", client_request);
                    handle_client_request(client_request, i);
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    if(argc != 2) 
    {
        fprintf(stderr, "usage: ./server.x server_config_filename\n");
        exit(1);
    }
    // read server config
    FILE* f = fopen(argv[1],"r");
    if (f) 
    {
        fscanf(f, "port=%d\n", &SERVER_PORT);
        fclose(f);
    } 
    else 
    {
        fprintf(stderr, "Error reading server configuration file\n");
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