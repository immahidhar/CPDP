#include "client.h"

int cl_sock_fd = -1, highestsocket = -1;
bool logged_in = false, exiting = false;
fd_set master, read_fds;
struct timeval timeout;
pthread_t cl_sock_tid;

/**
 * exit client
*/
void exit_client(int exit_num) {
    exiting = true;
    FD_CLR(cl_sock_fd, &master);
    FD_CLR(cl_sock_fd, &read_fds);
    usleep(THREAD_WAIT);
    close(cl_sock_fd);
    cl_sock_fd = -1;
    exit(exit_num);
}

/**
 * SIGINT handler to close opened sockets
*/
void sigint_function(int signum) {
    cout << endl << "SIGINT received: Shutting down client" << endl;
    exit_client(0);
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
    strcpy(packet.data, token.c_str());
    int send_result = send_packet_to_socket(cl_sock_fd, &packet);
    if (send_result == -1)  {
        perror("send");
        cerr << "Error sending packet to server!" << endl;
    }
}

/**
 * check server response for login and logout success
*/
void check_log_status(char* response) {
    if(strstr(response, LOGIN_SUCCESS) != NULL) {
        logged_in = true;
    } else if(strstr(response, LOGOUT_SUCCESS) != NULL) {
        logged_in = false;
    }
}

/**
 * Process message received from server
*/
void process_server_message(Packet *packet) {
    check_log_status(packet->data);
    cout << packet->data << endl;
    cout << "$ ";
    fflush(stdout);
}

/**
 * Read any message received from server
*/
void readFromServer(void) {
    int nbytes;
    unsigned char buf[MAXBUFLEN];	
    if ((cl_sock_fd != -1) && FD_ISSET(cl_sock_fd, &read_fds) ) {
        nbytes = recv(cl_sock_fd, buf, MAXBUFLEN, 0);
        // handle server response or data         
        if ( nbytes <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
                cout << "server closed connection!" << endl;
            } else {
                cout << "client recv error from server!, got errno " << errno << endl;
            }
            exit_client(1);
        } else {
            Packet* packet = (Packet*) (buf);
            process_server_message(packet);
        }
    }
}

/**
 * Client read from server thread
*/
void* client_run(void *arg) {
    pthread_detach(pthread_self());
    //cout << "Server socket select read thread running" << endl;
    // infinite loop
    while(1) {
        if(exiting) {
            //cout << "stopping select read thread" << endl;
            pthread_exit(NULL);
        }
        read_fds = master;
        if (select(highestsocket+1, &read_fds, NULL, NULL, &timeout) == -1) {
            if (errno == EINTR) {
                cerr << "Select for client interrupted by interrupt..." << endl;
            } else{
                perror("select");
                cerr << "client got errno " << errno << ", exiting client" << endl;
                exit_client(1);
            }
        }
        readFromServer();
    }
    return 0;
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
    timeout.tv_sec = 0;
    timeout.tv_usec = SELECT_WAIT;
}

void printUsage(void) {
    cerr << "incorrect command entered!" << endl;
    cerr << "usage:\n\tlogin username\n\tchat [@username] message\n\tlogout\n\texit" << endl;
}

/**
 * process command entered
*/
void process_command(string line, string* tokens) {
    string command = tokens[0];

    if(command == "exit") {

        if(!logged_in) {
            cout << "exiting client" << endl;
            exit_client(0);
        } else {
            cout << "User isn't logged out. Logout before exiting." << endl;
        }

    } else if(command == "login") {

        if(tokens[1] == "NULL") {
            printUsage();
            return;
        }

        // check if server is connected
        /*if(cl_sock_fd == -1) {
            client_init();
            pthread_create(&cl_sock_tid, NULL, &client_run, NULL);
        }*/

        cout << "logging in user \"" << tokens[1] << "\"" << endl;
        sendTokenToServer(line);

    } else if(command == "logout") {

        cout << "logging out" << endl;
        sendTokenToServer(line);
        
        /*usleep(THREAD_WAIT);
        FD_CLR(cl_sock_fd, &master);
        FD_CLR(cl_sock_fd, &read_fds);
        usleep(THREAD_WAIT);
        close(cl_sock_fd);
        cl_sock_fd = -1;*/

    } else if(command == "chat") {

        if(logged_in) {
            if(tokens[1] == "NULL") {
                printUsage();
                return;
            }
            sendTokenToServer(line);
        } else {
            cout << "Yor're not logged in. Login before trying to chat." << endl;
        }

    } else printUsage();
}

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    signal(SIGINT, sigint_function);
    if(argc < 2) {
        cout << "usage: client.x chat_server_config_filename" << endl;
        exit(1);
    }
    read_config(argv[1]);
    client_init();
    pthread_create(&cl_sock_tid, NULL, &client_run, NULL);
    cout << "$ ";
    fflush(stdout);
    string line;
    // infinite loop
    while (getline(cin, line)) {
        string tokens[TOKEN_LIMIT];
        string tokenss = line;
        get_tokens(tokenss, tokens);
        process_command(line, tokens);
        cout << "$ ";
        fflush(stdout);
    }
    cout << "EOF detected, exiting client" << endl;
    exit_client(0);
    return 0;
}
