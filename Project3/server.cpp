#include "server.h"

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "usage: server.x chat_server_config_filename" << endl;
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, sigint_function);
    read_config(argv[1]);
    server_init();
    pthread_mutex_init(&mutx, NULL);
    server_run();
    return EXIT_SUCCESS;
}

/**
 * create and setup server socket
*/
void server_init(void) {
    struct sockaddr_in serv_address;

    // Creating socket
    if ((serv_sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // avoid "address already in use" error message
    if (setsockopt(serv_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(serv_sock_fd);
        exit(EXIT_FAILURE);
    }

    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = INADDR_ANY;
    serv_address.sin_port = htons(SERVERPORT);

    // binding
    if (bind(serv_sock_fd, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0) {
        perror("bind");
        close(serv_sock_fd);
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(serv_sock_fd, SERVERBACKLOG) < 0) {
        perror("listen");
        close(serv_sock_fd);
        exit(EXIT_FAILURE);
    }


    socklen_t len = sizeof(serv_address);
    if(getsockname(serv_sock_fd, (struct sockaddr *)&serv_address, &len) < 0) {
        perror("getsockname");
        close(serv_sock_fd);
        exit(EXIT_FAILURE);
    }
    
    cout << "Server started on port " << htons(serv_address.sin_port) << endl;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    //FD_SET(fileno(stdin), &master);
    if (fileno(stdin) > highestsocket) highestsocket = fileno(stdin);
    FD_SET(serv_sock_fd, &master);
    if (serv_sock_fd > highestsocket) highestsocket = serv_sock_fd;

    tv.tv_sec = 0;
    tv.tv_usec = THREAD_WAIT;
}

/**
 * start running server
*/
void server_run(void) {
    while (1) {
        struct timeval stv;
        stv.tv_sec = 0;
        stv.tv_usec = SELECT_WAIT;
        read_fds = master; 
        if (select(highestsocket+1, &read_fds, NULL, NULL, &stv) == -1) {
            if (errno == EINTR) {
                cout << "got the EINTR error in select" << endl;
            } else {
                perror("select");
                cerr << "server got errno " << errno << ", exiting server" << endl;
                exit_server(EXIT_FAILURE);
            }
        }
        accept_connections();
        //sleep(THREAD_WAIT);
        //read_from_clients();
    }
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
            
            /*if (setsockopt(newconn->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }*/

            // execute client read thread
            pthread_create(&(newconn->p_tid), NULL, &read_from_client, newconn);
        }
    }
}

/**
 * Thread: Read message from client
*/
void* read_from_client(void* arg) {
    
    while(1) {

        //pthread_mutex_lock(&mutx);

        int nbytes;
        unsigned char buf[MAXBUFLEN];
        Connection* client = (Connection*) arg;
        
        if (FD_ISSET(client->socket, &read_fds)) {
            
            // read from client port: blocking call until data is available
            nbytes = recv(client->socket, buf, MAXBUFLEN, 0);
            
            if ( nbytes <= 0) {

                /*if ((errno == EAGAIN) && (errno == EWOULDBLOCK)) {
                    cout << pthread_self() << " recv timeout, client id:" << client->clientid 
                    << " fd:" << client->socket << endl;
                    //pthread_mutex_unlock(&mutx);
                    usleep(THREAD_WAIT); // sleep 100 ms
                    continue;
                }*/

                // if exiting, break from loop
                if(client->exiting) break;

                // got error or connection closed by client
                if (nbytes == 0) {
                    // connection closed
                    cerr << "Client closed connection, id:" << client->clientid 
                    << " fd: " << client->socket << endl;
                } else {
                    cerr << "received err from client, id:" << client->clientid 
                    << " fd: " << client->socket << " , server got errno " << errno << endl;
                }

                FD_CLR(client->socket, &master);
                // sleep before closing socket to avoid select error - this wait has to be more than select wait
                usleep(THREAD_WAIT); 
                close(client->socket);

                // remove connection
                size_t conn_pos = 0;
                for(; conn_pos < activeconnections.size(); conn_pos++) {
                    if(activeconnections[conn_pos]->clientid == client->clientid)
                        break;
                }
                activeconnections.erase(activeconnections.begin() + conn_pos);

                // send disconnected response to all
                string response = "User \"" + client->username + "\" is disconnected from chat.";
                response = "server >> " + response;
                if (client->username != "") send_token_to_client(response, client, true, true);

                //pthread_mutex_unlock(&mutx);
                return EXIT_SUCCESS;

            } else {

                process_client_message((Packet*) (buf), client);
                //pthread_mutex_unlock(&mutx);
                usleep(THREAD_WAIT); // sleep 100 ms

            }

        } else {

            //pthread_mutex_unlock(&mutx);
            usleep(THREAD_WAIT); // sleep 100 ms

        }

        //pthread_mutex_unlock(&mutx);

    }
    
    return EXIT_SUCCESS;
}

/**
 * Process message received from client
*/
void process_client_message(Packet *packet, Connection* conn) {
    string client_message(packet->data);
    if (conn->username != "") cout << conn->username << " >> " << client_message << endl;
    else cout << "user" << conn->clientid << " >> " << client_message << endl;
    string tokens[TOKEN_LIMIT];
    string tokenss = client_message;
    get_tokens(tokenss, tokens);
    process_command(tokens, conn);
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
    string username = conn->username;
    conn->username = "";
    response = "server >> " + response;
    cout << response << endl;
    send_token_to_client(response, conn, false, true);
    response = "User \"" + username + "\" left the chat.";
    response = "server >> " + response;
    send_token_to_client(response, conn, true, true);
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
 * send token to client
*/
void send_token_to_client(string token, Connection* conn, bool broadcast, bool ignore_login) {
    struct Packet packet;
    strcpy(packet.data, token.c_str());
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
 * read server configuration details
*/
void read_config(const char* configfile) {
    FILE* f = fopen(configfile,"r");
    if (f) {
        fscanf(f, "port: %d\n", &SERVERPORT);
        fclose(f);
    } else {
        perror("SERVERCONFIG");
        exit(EXIT_FAILURE);
    }
}

/**
 * SIGINT handler to close opened sockets
*/
void sigint_function(int signum) {
    cout << endl << "SIGINT received: Closing sockets and shutting down server" << endl;
    exit_server(EXIT_SUCCESS);
}

/**
 * exit server - close socket
*/
void exit_server(int exit_num) {
    for(size_t conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
        activeconnections[conn_iter]->exiting = true;
        FD_CLR(activeconnections[conn_iter]->socket, &master);
    }
    // sleep before closing socket to avoid select error - this wait has to be more than select wait
    usleep(THREAD_WAIT);
    for(size_t conn_iter = 0; conn_iter < activeconnections.size(); conn_iter++) {
        close(activeconnections[conn_iter]->socket);
    }
    close(serv_sock_fd);
    exit(exit_num);
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
