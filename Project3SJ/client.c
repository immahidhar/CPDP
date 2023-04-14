#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>

#define CONFIG_FILE "client.conf" // configuration file name
#define BUFFER_SIZE 1024         // buffer size for messages
#define MAX_MESSAGE_SIZE 1024    // maximum size of a message

int sock;                       // socket descriptor
struct sockaddr_in server_addr; // server address structure
char username[20];              // variable to store username
pthread_t send_thread, receive_thread;
pthread_mutex_t mutex;

void parse_config_file(const char* filename, char* server_addr, int* server_port) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening configuration file");
        exit(1);
    }
    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }
        char key[20], value[20];
        sscanf(line, "%[^=]=%s", key, value);
        if (strcmp(key, "SERVER_ADDR") == 0) {
            strncpy(server_addr, value, strlen(value));
        } else if (strcmp(key, "SERVER_PORT") == 0) {
            *server_port = atoi(value);
        } else {
            printf("Unknown configuration parameter: %s\n", key);
        }
    }
    fclose(fp);
}

void sendThreadFunction(void* arg) {
    char message[MAX_MESSAGE_SIZE];
    while (1) {
        printf("Enter message to send (type 'exit' to quit): ");
        fgets(message, MAX_MESSAGE_SIZE, stdin);
        if (strcmp(message, "exit\n") == 0) {
            break;
        }
        send(sock, message, strlen(message), 0);
    }
}

void receiveThreadFunction(void* arg) {
    char message[MAX_MESSAGE_SIZE];
    while (1) {
        int bytes_received = recv(sock, message, MAX_MESSAGE_SIZE, 0);
        if (bytes_received > 0) {
            message[bytes_received] = '\0';
            printf("Received message: %s", message);
        } else if (bytes_received == 0) {
            printf("Disconnected from server.\n");
            break;
        } else {
            printf("Error receiving message.\n");
            break;
        }
    }
}

void connect_server() {
    // create socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // setup server address structure
    char server_addr_str[20];
    int server_port = 0;
    parse_config_file(CONFIG_FILE, server_addr_str, &server_port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_addr_str);
    server_addr.sin_port = htons(server_port);

    // connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }
}

void login(char *user) {
    snprintf(username, sizeof(username) - 1, "%s", user);
    username[sizeof(username) - 1] = '\0';
    // send login command to server
    char login_msg[BUFFER_SIZE];
    snprintf(login_msg, sizeof(login_msg) - 1, "login %s", user);
    int bytes_sent = send(sock, login_msg, strlen(login_msg), 0);
    if (bytes_sent < 0) {
    perror("Error sending login message");
    exit(1);
    } else {
    printf("Logged in as %s\n", user);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
    printf("Usage: %s <client_config_file>\n", argv[0]);
    exit(1);
    }
    char *username = argv[1];
    printf("Connecting to server...\n");
    connect_server();
    printf("Connected.\n");

    // login to server
    //login(username);

    // create send and receive threads
    pthread_create(&send_thread, NULL, (void *)sendThreadFunction, NULL);
    pthread_create(&receive_thread, NULL, (void *)receiveThreadFunction, NULL);

    // wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    // close socket
    close(sock);

    return 0;
}







