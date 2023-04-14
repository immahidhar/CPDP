#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <libconfig.h>

#define DEFAULT_SERVER_PORT 9000
#define DEFAULT_BUFFER_SIZE 1024
#define DEFAULT_MAX_CLIENTS 10
#define DEFAULT_LOG_FILE "server.log"

int server_sock;                        // server socket descriptor
struct sockaddr_in server_addr;         // server address structure
int client_socks[DEFAULT_MAX_CLIENTS];  // client socket descriptors
char client_usernames[DEFAULT_MAX_CLIENTS][20]; // usernames of clients

int SERVER_PORT = DEFAULT_SERVER_PORT;      // server port number
int BUFFER_SIZE = DEFAULT_BUFFER_SIZE;      // buffer size for messages
int MAX_CLIENTS = DEFAULT_MAX_CLIENTS;      // maximum number of clients
char LOG_FILE[256] = DEFAULT_LOG_FILE;      // log file path

void read_config_file()
{
    config_t cfg;
    config_setting_t *setting;
    const char *str;

    // Initialize the configuration object
    config_init(&cfg);

    // Read the configuration file
    if (!config_read_file(&cfg, "server.cfg"))
    {
        printf("Error reading configuration file: %s\n", config_error_text(&cfg));
        config_destroy(&cfg);
        exit(1);
    }

    // Extract the values
    setting = config_lookup(&cfg, "server.port");
    if (setting != NULL && config_setting_type(setting) == CONFIG_TYPE_INT)
    {
        SERVER_PORT = config_setting_get_int(setting);
    }

    setting = config_lookup(&cfg, "server.buffer_size");
    if (setting != NULL && config_setting_type(setting) == CONFIG_TYPE_INT)
    {
        BUFFER_SIZE = config_setting_get_int(setting);
    }

    setting = config_lookup(&cfg, "server.max_clients");
    if (setting != NULL && config_setting_type(setting) == CONFIG_TYPE_INT)
    {
        MAX_CLIENTS = config_setting_get_int(setting);
    }

    setting = config_lookup(&cfg, "server.log_file");
    if (setting != NULL && config_setting_type(setting) == CONFIG_TYPE_STRING)
    {
        str = config_setting_get_string(setting);
        strncpy(LOG_FILE, str, sizeof(LOG_FILE));
        LOG_FILE[sizeof(LOG_FILE) - 1] = '\0';
    }

    // Destroy the configuration object
    config_destroy(&cfg);
}

// Signal handler function
void signalHandler(int signal) {
    // Handle the signal here
    printf("Received signal: %d\n", signal);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_socks[i] != -1) {
            close(client_socks[i]);
        }
    }
    close(server_sock);
    exit(0);
}

void setup_server()
{
    // create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Error creating socket");
        exit(1);
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
}

void accept_connections()
{
struct sockaddr_in client_addr;
socklen_t client_addr_len = sizeof(client_addr);
int new_sock;
// accept incoming connections
while (1)
{
    new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (new_sock < 0)
    {
        perror("Error accepting connection");
        exit(1);
    }

    // add new client socket to the array
    int i;
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
}
void handle_clients()
{
fd_set read_fds;
int max_fd, activity;
struct timeval timeout;
char buffer[BUFFER_SIZE];
int i, j, valread;
while (1)
{
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
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    // wait for activity on any of the sockets
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

    // check for new connection
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
                // client disconnected
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                getpeername(sd, (struct sockaddr *)&client_addr, &client_addr_len);
                printf("Client disconnected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                close(sd);
                client_socks[i] = -1;
            } else if (valread < 0)
            {
                perror("Error receiving data from client");
            } else
            {
                // process received data
                buffer[valread] = '\0';
                printf("Received data from client %d: %s\n", sd, buffer);
                // send response back to client
                char response[BUFFER_SIZE] = "Server response: ";
                strcat(response, buffer);
                send(sd, response, strlen(response), 0);
            }
        }
    }
}

void cleanup()
{
    // close all client sockets
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_socks[i] != -1)
        {
            close(client_socks[i]);
        }
    }
    // close server socket
    close(server_sock);
}

int main()
{
    // initialize client_socks array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_socks[i] = -1;
    }
    // setup server
    setup_server();

    // accept incoming connections
    accept_connections();

    // handle clients
    handle_clients();

    // cleanup and exit
    cleanup();
return 0