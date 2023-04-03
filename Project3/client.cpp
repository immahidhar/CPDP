#include "client.h"

int cl_sock_fd = -1, highestsocket = -1, server_sock_buf_byte_counter = 0;
char server_sock_buf[MY_SOCK_BUFFER_LEN];
fd_set master, read_fds;
pthread_t cl_sock_tid;

/**
 * exit client
*/
void exit_client(void) {
    //FD_CLR(cl_sock_fd, &master);
    //FD_CLR(cl_sock_fd, &read_fds);
    pthread_kill(cl_sock_tid, 0);
    close(cl_sock_fd);
    cl_sock_fd = -1;
    exit(0);
}

/**
 * SIGINT handler to close opened sockets
*/
void sigint_function(int signum) {
    cout << endl << "SIGINT received: Shutting down client" << endl;
    exit_client();
}

/**
 * read server configuration details
*/
void read_config(const char* configfile) {
    FILE* f = fopen(configfile,"r");
    if (f) {
        fscanf(f, "servhost: %s\n", SERVERHOST);
        fscanf(f, "servport: %d\n", &SERVERPORT);
        fclose(f);
    } else {
        perror("SERVERCONFIG");
        exit(1);
    }
}

/**
 * Send token to server
*/
void sendTokenToServer(string token) {
    struct Packet packet;
    memcpy(packet.data, token.c_str(), sizeof(token));
    int send_result = send_packet_to_socket(cl_sock_fd, &packet);
    if (send_result == -1)  {
        perror("send");
        cerr << "Error sending packet to server!" << endl;
    }
}

void printUsage(void) {
    cerr << "incorrect command entered!" << endl;
    cerr << "usage:\n\tlogin username\n\tlogout username\n\tchat [@username] message\n\texit" << endl;
}

/**
 * process command entered
*/
void process_command(string line, string* tokens) {
    string command = tokens[0];
    if(command == "exit") {
        cout << "exiting client" << endl;
        exit_client();
    } else if(command == "login") {
        if(tokens[1] == "NULL") {
            printUsage();
            return;
        }
        cout << "logging in user \"" << tokens[1] << "\"" << endl;
        sendTokenToServer(line);
    } else if(command == "logout") {
        if(tokens[1] == "NULL") {
            printUsage();
            return;
        }
        cout << "logging out user \"" << tokens[1] << "\"" << endl;
        sendTokenToServer(line);
    } else if(command == "chat") {
        if(tokens[1] == "NULL") {
            printUsage();
            return;
        }
        cout << "sending chat" << endl;
        sendTokenToServer(line);
    } else {
       printUsage();
    }
}


/**
 * Initialize client by opening socket and connecting to server
*/
void client_init(void) {
    struct hostent *he_server;
    if ((he_server = gethostbyname(SERVERHOST)) == NULL) {
        perror("gethostbyname");
        cerr << "error resolving hostname for server" << SERVERHOST << endl;
        exit(1);
    }

    struct sockaddr_in  server;
    memcpy(&server.sin_addr, he_server->h_addr_list[0], he_server->h_length);
    strcpy(SERVERIP, inet_ntoa(server.sin_addr));

    cout << "Connecting to " << SERVERHOST << ":" << SERVERPORT << " ..." << endl;
    if ((cl_sock_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    inet_aton(SERVERIP, (struct in_addr *)&server_addr.sin_addr.s_addr);
    memset(&(server_addr.sin_zero), '\0', 8);

    // connect
    if (connect(cl_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        cerr << "Error connecting to the server " << SERVERIP << " on port " << SERVERPORT << endl;
        close(cl_sock_fd);
        cl_sock_fd = -1;
        exit(1);
    }
    cout << "Connected to " << SERVERIP << ":" << SERVERPORT << endl;

    // clear the FD sets
    FD_ZERO(&master);    
    FD_ZERO(&read_fds);

    FD_SET(fileno(stdin), &master);
    if (fileno(stdin) > highestsocket)
        highestsocket = fileno(stdin);
    if (cl_sock_fd != -1)  {
        FD_SET(cl_sock_fd, &master);
        if (highestsocket <= cl_sock_fd) {
            highestsocket = cl_sock_fd;
        } 
    }
}

/**
 * Process message received from server
*/
void process_server_message(Packet *packet) {
    cout << packet->data;
}

/**
 * Read any message received from server
*/
void readFromServer(void) {
    int nbytes;
    unsigned char buf[10000];	
    if ((cl_sock_fd != -1) && FD_ISSET(cl_sock_fd, &read_fds) ) {
        nbytes = recv(cl_sock_fd, buf, MAXBUFLEN, 0);
        // handle server response or data         
        if ( nbytes <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
                cout << "server closed connection!" << endl;
            } else {
                cout << "client recv error from server!" << endl;
            }
            //sleep(10000);
            close(cl_sock_fd);
            FD_CLR(cl_sock_fd, &master);
            cl_sock_fd = -1;
            exit(1);
        } else {
            memcpy(server_sock_buf + server_sock_buf_byte_counter, buf, nbytes);
            server_sock_buf_byte_counter += nbytes;
            int num_to_read = sizeof(Packet);
            //cout << endl;
            while (num_to_read <= server_sock_buf_byte_counter) {
                Packet* packet = (Packet*) (server_sock_buf);     
                process_server_message(packet);
                remove_read_from_buf(server_sock_buf, num_to_read);
                server_sock_buf_byte_counter -= num_to_read;
            }
            cout << endl;
        }
    }
}

/**
 * Client read from server thread
*/
void* client_run(void *arg) {
    pthread_detach(pthread_self());
    cout << "Server socket read thread running" << endl;
    // infinite loop
    while(1) {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        read_fds = master; 
        read_fds = master;
        if (select(highestsocket+1, &read_fds, NULL, NULL, &timeout) == -1) {
            if (errno == EINTR) {
                cerr << "Select for client interrupted by interrupt..." << endl;
            } else {
                cerr << "Select problem .. client exiting iteration" << endl;
                exit(1);
            }
        }
        readFromServer();
    }
}

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    signal(SIGINT, sigint_function);
    read_config(CLIENTCONFIG);
    client_init();
    pthread_create(&cl_sock_tid, NULL, &client_run, NULL);
    // infinite loop
    while (1) {
        //cout << "$ ";
        string line;
        getline(cin, line);
        string tokens[TOKEN_LIMIT];
        string tokenss = line;
        get_tokens(tokenss, tokens);
        process_command(line, tokens);
    }
    return 0;
}
