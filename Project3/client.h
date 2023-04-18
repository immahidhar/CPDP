#ifndef __CLIENT_H

#define __CLIENT_H

#include "common.h"

const char* CLIENTCONFIG = "chat_client_configuration_file";
char SERVERHOST[100] = "localhost", SERVERIP[40];
int SERVERPORT = 51000, cl_sock_fd = -1, highestsocket = -1;
bool logged_in = false, exiting = false;
fd_set master, read_fds;
struct timeval timeout;
pthread_t cl_sock_tid;

void client_init(void) ;
void* client_run(void *arg);
void process_command(string line, string* tokens);
void sendTokenToServer(string token);
void readFromServer(void);
void process_server_message(Packet *packet);
void check_log_status(char* response);
void printUsage(void);
void read_config(const char* configfile);
void sigint_function(int signum);
void exit_client(int exit_num);

#endif
