#include "server.h"

fd_set master, read_fds;
int serv_sock_fd = -1, highestsocket = -1, opt = 1, server_curr_clientid = 0;

/**
 * Connection class to store client info
*/
class Connection {
    public:
        pthread_t p_tid;
        string username;
        bool loggged_in;
        struct Packet pkt;
        char sock_buf[MY_SOCK_BUFFER_LEN];
        int socket, clientid, sock_buf_byte_counter;
    
    Connection() {
        socket = -1;
        sock_buf_byte_counter = 0;
        clientid = -1;
        loggged_in = false;
        username = "";
    }

    Connection(int sock, int id) {
        socket = sock;
        sock_buf_byte_counter = 0;
        clientid = id;
        loggged_in = false;
        username = "";
    }

    void setUsername(string uname) {
        username = uname;
    }

    string getUsername(void) {
        if(username == "")
            return "user" + to_string(clientid);
        else
            return username;
    }
};

vector<Connection> activeconnections;

/**
 * SIGINT handler to close opened sockets
*/
void sigint_function(int signum) {
    cout << endl << "SIGINT received: Shutting down server" << endl;
    for(int conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
        close(activeconnections[conn_iter].socket);
        //pthread_kill(activeconnections[conn_iter].p_tid, 0);
    }
    close(serv_sock_fd);
    exit(0);
}

/**
 * read server configuration details
*/
void read_config(const char* configfile) {
    FILE* f = fopen(configfile,"r");
    if (f) {
        fscanf(f, "port: %d\n", &SERVERPORT);
        fclose(f);
    } else {
        perror("SERVERCONFIG");
        fflush(stdout);
        exit(1);
    }
}

/**
 * send data to client
*/
void send_token_to_client(string token, Connection* conn, bool broadcast) {
    struct Packet packet;
    memcpy(packet.data, token.c_str(), sizeof(token));
    if (!broadcast) {
        int send_result = send_packet_to_socket(conn->socket, &packet);
        if (send_result == -1) {
            perror("send");
            cerr << "Error sending packet to client - fd: " << conn->socket << endl;
        }
    } else {
        for(int conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
            if(conn->clientid != activeconnections[conn_iter].clientid) {
                int send_result = send_packet_to_socket(activeconnections[conn_iter].socket, &packet);
                if (send_result == -1)  {
                    perror("send");
                    cerr << "Error sending packet to client - fd: " << conn->socket << endl;
                }
            }
        }
    }
    
}

Connection* check_if_user_present(string username) {
    cout << "searching username " << username << endl;
    if(username.c_str()[0] == '@') {
        username = string(&username.c_str()[1]);
        for(int conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
            if(username == activeconnections[conn_iter].getUsername()) {
                return &activeconnections[conn_iter];
            }
        }
    }
    return NULL;
}

/**
 * process command received
*/
void process_command(string* tokens, Connection conn) {
    string command = tokens[0];
    if(command == "login") {
        cout << "logging in user" << endl;
        conn.setUsername(tokens[1]);
        conn.loggged_in = true;
        string response = "User \"" + conn.getUsername() + "\" logged in";
        cout << response << endl;
        send_token_to_client(response, &conn, false);
    } else if(command == "logout") {
        cout << "logging out user" << endl;
        conn.loggged_in = false;
        string response = "User \"" + conn.getUsername() + "\" logged out";
        cout << response << endl;
        send_token_to_client(response, &conn, false);
    } else if(command == "chat") {
        string chat_tokens[TOKEN_LIMIT];
        string tokenss = tokens[1];
        get_tokens(tokenss, chat_tokens);
        if(chat_tokens[1] != "NULL") {
            Connection* r_conn = check_if_user_present(chat_tokens[0]);
            if(r_conn != NULL) {
                send_token_to_client(chat_tokens[1], r_conn, false);
            } else {
                string response = "No user found with username " + chat_tokens[0];
                cerr << response << endl;
                send_token_to_client(response, &conn, false);
            }
        } else {
            send_token_to_client(chat_tokens[0], &conn, true);
        }
    }
}

/**
 * Process message received from client
*/
void process_client_message(Packet *packet, Connection conn) {
    string client_message(packet->data);
    cout << conn.getUsername() << " >> " << client_message << endl;
    string tokens[TOKEN_LIMIT];
    string tokenss = client_message;
    get_tokens(tokenss, tokens);
    process_command(tokens, conn);
}

/**
 * read data from clients
*/
void read_from_clients(void) {
    int nbytes;
    char buf[MAXBUFLEN];
    // run through the existing connections looking for data to read
    for(int conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
        if (FD_ISSET(activeconnections[conn_iter].socket, &read_fds)) {
            nbytes = recv(activeconnections[conn_iter].socket, buf, MAXBUFLEN, 0);
            if ( nbytes <= 0) {
                // got error or connection closed by client
                if (nbytes == 0) {
                    // connection closed
                    cerr << "Client closed connection, fd: " << activeconnections[conn_iter].socket << endl;
                } else {
                    cerr << "received err from client" << endl;
                }
                close(activeconnections[conn_iter].socket);
                FD_CLR(activeconnections[conn_iter].socket, &master);
                activeconnections.erase(activeconnections.begin()+conn_iter);
                conn_iter --;
            } else {
                memcpy(activeconnections[conn_iter].sock_buf + activeconnections[conn_iter].sock_buf_byte_counter, buf, nbytes);
                activeconnections[conn_iter].sock_buf_byte_counter += nbytes;
                int num_to_read = sizeof(Packet);
                while (num_to_read <= activeconnections[conn_iter].sock_buf_byte_counter) {
                    Packet* packet = (Packet*) (activeconnections[conn_iter].sock_buf);
                    process_client_message(packet, activeconnections[conn_iter]);
                    remove_read_from_buf(activeconnections[conn_iter].sock_buf, num_to_read);
                    activeconnections[conn_iter].sock_buf_byte_counter -= num_to_read;
                }
            }
        }
    }
}

/**
 * Thread: Read message from client
*/
void* read_from_client(void *arg) {
    int nbytes;
    char buf[MAXBUFLEN];
    Connection *client = (Connection *) arg;
    cout << "Client id: " << client->clientid << " - read thread running" << endl;
    while(1) {
        if (FD_ISSET(client->socket, &read_fds)) {
        nbytes = recv(client->socket, buf, MAXBUFLEN, 0);
        if ( nbytes <= 0) {
                // got error or connection closed by client
                if (nbytes == 0) {
                    // connection closed
                    cerr << "Client closed connection, id:" << client->clientid << " fd: " << client->socket << endl;
                } else {
                    cerr << "received err from client" << endl;
                }
                close(client->socket);
                FD_CLR(client->socket, &master);
                cout << "Client id: " << client->clientid << " fd: " << client->socket << " - read thread terminating" << endl;
                int conn_pos = 0;
                for(; conn_pos < activeconnections.size(); conn_pos++) {
                    if(activeconnections[conn_pos].clientid == client->clientid)
                        break;
                }
                activeconnections.erase(activeconnections.begin() + conn_pos);
                return 0;
            } else {
                memcpy(client->sock_buf + client->sock_buf_byte_counter, buf, nbytes);
                client->sock_buf_byte_counter += nbytes;
                int num_to_read = sizeof(Packet);
                while (num_to_read <= client->sock_buf_byte_counter) {
                    Packet* packet = (Packet*) client->sock_buf;
                    process_client_message(packet, *client);
                    remove_read_from_buf(client->sock_buf, num_to_read);
                    client->sock_buf_byte_counter -= num_to_read;
                }
            }
        } /*else {
            sleep(100);
        }*/
    }
    return 0;
}

/**
 * acept connections from clients
*/
void accept_connections(void) {
    struct sockaddr_in remoteaddr;
    socklen_t addrlen;
    int newfd;
    if (FD_ISSET(serv_sock_fd, &read_fds)) {
        addrlen = sizeof(remoteaddr);
        if ((newfd = accept(serv_sock_fd, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) {
            perror("accept");
        } else {
            FD_SET(newfd, &master);
            if (newfd > highestsocket) highestsocket = newfd;
            cout << "New client connected - " << inet_ntoa(remoteaddr.sin_addr) << ":" << remoteaddr.sin_port 
            << " localid : " << server_curr_clientid << " fd : " << newfd << endl;
            Connection newconn(newfd, server_curr_clientid);
            server_curr_clientid ++;
            activeconnections.push_back(newconn);
            /*pthread_t p_tid;
            pthread_create(&p_tid, NULL, &read_from_client, (void*) &newconn);
            newconn.p_tid = p_tid;*/
        }
    }
}

/**
 * start running server
*/
void server_run() {
    while (1) {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        read_fds = master; 
        if (select(highestsocket+1, &read_fds, NULL, NULL, &timeout) == -1) {
            if (errno == EINTR) {
                cout << "got the EINTR error in select" << endl;
            } else {
                cout << "select problem, server got errno " << errno << endl;   
                cerr << "Select problem .. exiting server" << endl;
                exit(1);
            }
        }
        accept_connections();
        read_from_clients();
    }
}

/**
 * create and setup server socket
*/
void server_init() {
    struct sockaddr_in serv_address;

    // Creating socket
    if ((serv_sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // avoid "address already in use" error message
    if (setsockopt(serv_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = INADDR_ANY;
    serv_address.sin_port = htons(SERVERPORT);

    // binding
    if (bind(serv_sock_fd, (struct sockaddr*)&serv_address, sizeof(serv_address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(serv_sock_fd, SERVERBACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(fileno(stdin), &master);
    if (fileno(stdin) > highestsocket) highestsocket = fileno(stdin);
    FD_SET(serv_sock_fd, &master);
    if (serv_sock_fd > highestsocket) highestsocket = serv_sock_fd;

    cout << "Server started on port " << SERVERPORT << endl;
}

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    signal(SIGINT, sigint_function);
    read_config(SERVERCONFIG);
    server_init();
    server_run();
    return 0;
}
