#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>

#define CONFIG_FILE "client.conf" // configuration file name
#define BUFFER_SIZE 1024         // buffer size for messages
#define MAX_MESSAGE_SIZE 1024    // maximum size of a message

char SERVER_HOST[100] = "localhost";
char SERVER_IP[40];
int SERVER_PORT;
bool exitFlag = false;
int sock;                       // socket descriptor
struct sockaddr_in server_addr; // server address structure
char username[20];              // variable to store username
pthread_t receive_thread;

void cleanup() {
    exitFlag = true;
    close(sock);
}

// Signal handler function
void signalHandler(int signal) 
{
    printf("Received signal %d, exiting client\n", signal);
    cleanup();
    exit(0);
}

void sendCommands() 
{
    char message[MAX_MESSAGE_SIZE];
    while (1) {
        //printf("> ");
        fgets(message, MAX_MESSAGE_SIZE, stdin);
        //printf("%s\n", message);
        int pos = strlen(message);
        message[pos-1] = '\0';
        //printf("%s\n", message);
        if (strcmp(message, "exit") == 0) {
            printf("exiting client ...\n");
            cleanup();
            exit(0);
        }
        int sr = send(sock, message, strlen(message), 0);
        if(sr < 0)
        {
            fprintf(stderr, "Error sending data on socket\n");
            cleanup();
            exit(1);
        }
    }
}

void receiveThreadFunction(void* arg) 
{
    while (1) 
    {
        char message[MAX_MESSAGE_SIZE];
        int bytes_received = recv(sock, message, MAX_MESSAGE_SIZE, 0);
        if (bytes_received > 0) 
        {
            message[bytes_received] = '\0';
            printf("%s\n", message);
        } 
        else if(exitFlag) 
        {
            break;
        }
        else if (bytes_received == 0) 
        {
            fprintf(stderr, "Server disconnected.\n");
            exit(1);
        } 
        else 
        {
            fprintf(stderr, "Error receiving message.\n");
            exit(1);
        }
    }
}

void connect_server() 
{
    // create socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("Error creating socket");
        exit(1);
    }

    struct hostent *he_server;
    he_server = gethostbyname(SERVER_HOST);
    if (he_server  == NULL) 
    {
        perror("gethostbyname");
        fprintf(stderr, "error resolving hostname for server\n");
        exit(1);
    }
    memcpy(&server_addr.sin_addr, he_server->h_addr_list[0], he_server->h_length);
    strcpy(SERVER_IP, inet_ntoa(server_addr.sin_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_aton(SERVER_IP, (struct in_addr *)&server_addr.sin_addr.s_addr);
    memset(&(server_addr.sin_zero), '\0', 8);

    // connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Error connecting to server");
        exit(1);
    }
}

void login(char *user) 
{
    snprintf(username, sizeof(username) - 1, "%s", user);
    username[sizeof(username) - 1] = '\0';
    // send login command to server
    char login_msg[BUFFER_SIZE];
    snprintf(login_msg, sizeof(login_msg) - 1, "login %s", user);
    int bytes_sent = send(sock, login_msg, strlen(login_msg), 0);
    if (bytes_sent < 0) 
    {
        perror("Error sending login message");
        exit(1);
    } 
    else 
    {
        printf("Logged in as %s\n", user);
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        printf("Usage: ./client.x client_config_file");
        exit(1);
    }
    // read server config
    FILE* f = fopen(argv[1],"r");
    if (f) 
    {
        fscanf(f, "servhost=%s\n", SERVER_HOST);
        fscanf(f, "servport=%d\n", &SERVER_PORT);
        fclose(f);
    } 
    else 
    {
        fprintf(stderr, "Error reading server configuration file");
        exit(1);
    }
    signal(SIGINT, signalHandler);

    printf("Connecting to server...\n");
    connect_server();
    printf("Connected\n");

    // create receive thread
    pthread_create(&receive_thread, NULL, (void *)receiveThreadFunction, NULL);

    sendCommands();

    // wait for thread to finish
    pthread_join(receive_thread, NULL);

    // close socket
    close(sock);

    return 0;
}







