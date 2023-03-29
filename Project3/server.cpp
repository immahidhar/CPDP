#include "server.h"

int serv_sock_fd, opt = 1;

/**
 * read server configuration details
*/
void read_config(const char* configfile) {
    FILE* f = fopen(configfile,"r");
    if (f) {
        fscanf(f, "port: %d\n", &SERVERPORT);
        fclose(f);
    } else {
        perror("SERVERCONFIG:");
        fflush(stdout);
        exit(1);
    }
}

/**
 * create and setup server socket
*/
void server_init() {
    struct sockaddr_in serv_address;

    // Creating socket
    if ((serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket:");
        exit(EXIT_FAILURE);
    }

    // avoid "address already in use" error message
    if (setsockopt(serv_sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt:");
        exit(EXIT_FAILURE);
    }

    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = INADDR_ANY;
    serv_address.sin_port = htons(SERVERPORT);

    // binding
    if (bind(serv_sock_fd, (struct sockaddr*)&serv_address, sizeof(serv_address)) < 0) {
        perror("bind:");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(serv_sock_fd, SERVERBACKLOG) < 0) {
        perror("listen:");
        exit(EXIT_FAILURE);
    }
}

/**
 * start running server
*/
void server_run() {
    while (1) {
        // TODO: implementation pending
    }
}

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    read_config(SERVERCONFIG);
    server_init();
    //server_run();
    return 0;
}
