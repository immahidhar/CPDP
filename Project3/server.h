#ifndef __SERVER_H

#define __SERVER_H

#include "common.h"

/**
 * Connection class to store client info
*/
class Connection {
    public:
        pthread_t p_tid;
        string username;
        bool loggged_in, exiting;
        int socket, clientid;

    Connection(int sock, int id) {
        socket = sock;
        clientid = id;
        loggged_in = false;
        exiting = false;
        username = "";
    }
};

int SERVERPORT = 51000, SERVERBACKLOG = 10, serv_sock_fd = -1, 
highestsocket = -1, opt = 1, server_curr_clientid = 0;
const char* SERVERCONFIG = "chat_server_configuration_file";
fd_set master, read_fds;
struct timeval tv;

pthread_mutex_t mutx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

vector<Connection*> activeconnections;

void server_init(void);
void server_run(void);
void accept_connections(void);
void* read_from_client(void* arg);
void process_client_message(Packet *packet, Connection* conn);
void process_command(string* tokens, Connection* conn);
void login(string* tokens, Connection* conn);
void logout(string* tokens, Connection* conn);
void chat(string* tokens, Connection* conn);
void send_token_to_client(string token, Connection* conn, bool broadcast, bool ignore_login);
bool check_if_username_present(string username);
Connection* get_user_connection(string username);
void read_config(const char* configfile);
void sigint_function(int signum);
void exit_server(int exit_num);

#endif
