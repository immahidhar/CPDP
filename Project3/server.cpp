#include "server.h"

fd_set master, read_fds;
int serv_sock_fd = -1, highestsocket = -1, opt = 1, server_curr_clientid = 0;

pthread_mutex_t mutx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct timeval tv;

/**
 * Connection class to store client info
*/
class Connection {
    public:
        pthread_t p_tid;
        string username;
        bool loggged_in;
        int socket, clientid;

    Connection(int sock, int id) {
        socket = sock;
        clientid = id;
        loggged_in = false;
        username = "";
    }
};

vector<Connection*> activeconnections;

/**
 * exit server - close socket
*/
void exit_server(int exit_num) {
    for(size_t conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++)
        close(activeconnections[conn_iter]->socket);
    close(serv_sock_fd);
    exit(exit_num);
}

/**
 * SIGINT handler to close opened sockets
*/
void sigint_function(int signum) {
    cout << endl << "SIGINT received: Shutting down server" << endl;
    exit_server(0);
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
        exit(1);
    }
}

/**
 * send token to client
*/
void send_token_to_client(string token, Connection* conn, bool broadcast, bool ignore_login) {
    struct Packet packet;
    strcpy(packet.data, token.c_str());
    //cout << conn->p_tid << " : send_token_to_client: " << packet.data << endl;
    if (!broadcast) {
        if(conn->loggged_in || ignore_login) {
            int send_result = send_packet_to_socket(conn->socket, &packet);
            if (send_result == -1) {
                perror("send");
                cerr << "Error sending packet to client - id: " << conn->clientid << " fd: " << conn->socket << endl;
            }
        } else {
            cerr << "Client not logged in - id: " << conn->clientid << " fd: " << conn->socket << " username:"
            << conn->username << endl;
        }
    } else {
        for(size_t conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
            if(conn->clientid != activeconnections[conn_iter]->clientid) {
                if(activeconnections[conn_iter]->loggged_in || ignore_login) {
                    int send_result = send_packet_to_socket(activeconnections[conn_iter]->socket, &packet);
                    if (send_result == -1)  {
                        perror("send");
                        cerr << "Error sending packet to client - id: " << conn->clientid << " fd: " << conn->socket << endl;
                    }
                } else {
                    cerr << "Client not logged in - id: " << conn->clientid << " fd: " << conn->socket << " username:"
                    << conn->username << endl;
                }
            }
        }
    }
    
}

/**
 * see if token has @ at the beginning
*/
bool check_if_username_present(string username) {
    if(username.c_str()[0] == '@') return true;
    else return false;
}

/**
 * Search for user by username among activeconnections i.e. clients
*/
Connection* get_user_connection(string username) {
    cout << "searching username " << username << endl;
    username = string(&username.c_str()[1]);
    for(size_t conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
        if(username == activeconnections[conn_iter]->username) {
            return activeconnections[conn_iter];
        }
    }
    return NULL;
}

/**
 * chat with clients
*/
void chat(string* tokens, Connection* conn) {

    if(!conn->loggged_in) {
        string response = "You're not logged in to chat.";
        response = "server >> " + response;
        cout << response << endl;
        send_token_to_client(response, conn, false, true);
        return;
    }

    string chat_tokens[TOKEN_LIMIT];
    string chat_tokenss = tokens[1];
    get_tokens(chat_tokenss, chat_tokens);

    if(check_if_username_present(chat_tokens[0])) {
        Connection* r_conn = get_user_connection(chat_tokens[0]);
        if(r_conn != NULL) {
            if(r_conn->loggged_in) {
                string chat = conn->username + " >> " + chat_tokens[1];
                send_token_to_client(chat, r_conn, false, false);
                string response = "chat sent to " + chat_tokens[0];
                response = "server >> " + response;
                cout << response << endl;
                send_token_to_client(response, conn, false, true);
            } else {
                string response = "User " + chat_tokens[0] + " is not logged in to chat.";
                response = "server >> " + response;
                cerr << response << endl;
                send_token_to_client(response, conn, false, true);
            }
        } else {
            string response = "No user found with username " + chat_tokens[0];
            response = "server >> " + response;
            cerr << response << endl;
            send_token_to_client(response, conn, false, true);
        }
    } else {
        string chat = conn->username + " >> " + chat_tokenss;
        send_token_to_client(chat, conn, true, false);
        string response = "chat broadcasted";
        response = "server >> " + response;
        cout << response << endl;
        send_token_to_client(response, conn, false, true);
    }

}

/**
 * logout user
*/
void logout(string* tokens, Connection* conn) {
    if(!conn->loggged_in) {
        string response = "You're not logged in to logout.";
        response = "server >> " + response;
        cout << response << endl;
        send_token_to_client(response, conn, false, true);
        return;
    }
    cout << "logging out user " << conn->username << endl;
    string response = "User \"" + conn->username + "\"" + LOGOUT_SUCCESS;
    conn->loggged_in = false;
    //conn->setUsername("");
    response = "server >> " + response;
    cout << response << endl;
    send_token_to_client(response, conn, false, true);
    response = "User \"" + conn->username + "\" left the chat.";
    response = "server >> " + response;
    send_token_to_client(response, conn, true, true);
}

/**
 * login user
*/
void login(string* tokens, Connection* conn) {
    if(conn->loggged_in) {
        string response = "You're already logged in. Username: " + conn->username;
        response = "server >> " + response;
        cout << response << endl;
        send_token_to_client(response, conn, false, true);
        return;
    }
    cout << "logging in user " << tokens[1] << endl;

    if(get_user_connection("@" + tokens[1]) != NULL) {
        string response = "User \"" + tokens[1] + "\" already present. Please choose another username.";
        response = "server >> " + response;
        cout << response << endl;
        send_token_to_client(response, conn, false, true);
    } else {
        conn->username = tokens[1];
        conn->loggged_in = true;
        string response = "User \"" + conn->username + "\"" + LOGIN_SUCCESS;
        response = "server >> " + response;
        cout << response << endl;
        send_token_to_client(response, conn, false, true);
        response = "User \"" + conn->username + "\" has joined the chat.";
        response = "server >> " + response;
        send_token_to_client(response, conn, true, true);
    }
}

/**
 * process command received from client
*/
void process_command(string* tokens, Connection* conn) {
    string command = tokens[0];
    if(command == "login") login(tokens, conn);
    else if(command == "logout") logout(tokens, conn);
    else if(command == "chat") chat(tokens,conn);
}

/**
 * Process message received from client
*/
void process_client_message(Packet *packet, Connection* conn) {
    string client_message(packet->data);
    if (conn->username != "")
        cout << conn->username << " >> " << client_message << endl;
    else
        cout << "user" << conn->clientid << " >> " << client_message << endl;
    string tokens[TOKEN_LIMIT];
    string tokenss = client_message;
    get_tokens(tokenss, tokens);
    process_command(tokens, conn);
}

/**
 * read data from clients
*/
/*void read_from_clients(void) {
    int nbytes;
    unsigned char buf[MAXBUFLEN];
    // run through the existing clients looking for data to read
    for(size_t conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
        if (FD_ISSET(activeconnections[conn_iter]->socket, &read_fds)) {
            nbytes = recv(activeconnections[conn_iter]->socket, buf, MAXBUFLEN, 0);
            if ( nbytes <= 0) {
                // got error or connection closed by client
                if (nbytes == 0) {
                    // connection closed
                    cerr << "Client closed connection, id: " <<activeconnections[conn_iter]->clientid
                    << " fd: " << activeconnections[conn_iter]->socket << endl;
                } else {
                    cerr << "received err from client, id: " <<activeconnections[conn_iter]->clientid
                    << " fd: " << activeconnections[conn_iter]->socket << endl;
                }
                FD_CLR(activeconnections[conn_iter]->socket, &master);
                close(activeconnections[conn_iter]->socket);
                activeconnections.erase(activeconnections.begin()+conn_iter);
                conn_iter --;
            } else {
                Packet* packet = (Packet*) (buf);
                process_client_message(packet, activeconnections[conn_iter]);
            }
        }
    }
}*/

/**
 * Thread: Read message from client
*/
void* read_from_client(void* arg) {

    /*cout << "Client id: " << ((Connection*)arg)->clientid << " fd: " << ((Connection*)arg)->socket 
    << " - read thread " << ((Connection*)arg)->p_tid << " running" << endl;*/
    
    while(1) {

        pthread_mutex_lock(&mutx);

        int nbytes;
        unsigned char buf[MAXBUFLEN];
        Connection* client = (Connection*) arg;

        /*cout << "tid: " << pthread_self() << " has lock - oid: " << client << " id: " << client->clientid 
        << " fd:" << client->socket << " logged:" << client->loggged_in << " username:" 
        << client->username << endl;*/
        
        if (FD_ISSET(client->socket, &read_fds)) {
            
            nbytes = recv(client->socket, buf, MAXBUFLEN, 0);
            
            if ( nbytes <= 0) {

                if ((errno == EAGAIN) && (errno == EWOULDBLOCK)) {
                    pthread_mutex_unlock(&mutx);
                    usleep(THREAD_WAIT); // sleep 100 ms
                    continue;
                }

                // got error or connection closed by client
                if (nbytes == 0) {
                    // connection closed
                    cerr << "Client closed connection, id:" << client->clientid 
                    << " fd: " << client->socket << endl;
                } else {
                    cerr << "received err from client, id:" << client->clientid 
                    << " fd: " << client->socket << endl;
                }

                FD_CLR(client->socket, &master);
                close(client->socket);
                /*cout << "Client id: " << client->clientid << " fd: " << client->socket 
                << " - read thread terminating" << endl;*/
                size_t conn_pos = 0;
                for(; conn_pos < activeconnections.size(); conn_pos++) {
                    if(activeconnections[conn_pos]->clientid == client->clientid)
                        break;
                }
                activeconnections.erase(activeconnections.begin() + conn_pos);

                string response = "User \"" + client->username + "\" is disconnected from chat.";
                response = "server >> " + response;
                send_token_to_client(response, client, true, true);

                pthread_mutex_unlock(&mutx);
                return 0;

            } else {

                process_client_message((Packet*) (buf), client);
                pthread_mutex_unlock(&mutx);
                usleep(THREAD_WAIT); // sleep 100 ms

            }

        } else {

            pthread_mutex_unlock(&mutx);
            usleep(THREAD_WAIT); // sleep 100 ms

        }

        //pthread_mutex_unlock(&mutx);

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
            << " id: " << server_curr_clientid << " fd: " << newfd << endl;

            Connection* newconn = new Connection(newfd, server_curr_clientid);
            server_curr_clientid ++;
            activeconnections.push_back(newconn);
            
            if (setsockopt(newconn->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }

            // execute client read thread
            pthread_create(&(newconn->p_tid), NULL, &read_from_client, newconn);
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
            } else if ((errno == EAGAIN) && (errno == EWOULDBLOCK)) {
                continue;
            } else {
                cout << "select problem, server got errno " << errno << endl;   
                //cerr << "Select problem .. exiting server" << endl;
                //exit_server(1);
            }
        }
        accept_connections();
        //sleep(THREAD_WAIT);
        //read_from_clients();
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
    //if (setsockopt(serv_sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
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
    //FD_SET(fileno(stdin), &master);
    if (fileno(stdin) > highestsocket) highestsocket = fileno(stdin);
    FD_SET(serv_sock_fd, &master);
    if (serv_sock_fd > highestsocket) highestsocket = serv_sock_fd;

    cout << "Server started on port " << SERVERPORT << endl;

    tv.tv_sec = 0;
    tv.tv_usec = THREAD_WAIT;
}

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    signal(SIGINT, sigint_function);
    if(argc < 2) {
        cout << "usage: server.x chat_server_config_filename" << endl;
        exit(1);
    }
    read_config(argv[1]);
    server_init();
    pthread_mutex_init(&mutx, NULL);
    server_run();
    return 0;
}
