#ifndef __COMMON_H

#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <errno.h>

#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <list>

#include <pthread.h>

#define TOKEN_LIMIT 2
#define MAXBUFLEN 100000
#define MY_PACKET_LEN 10000
#define MY_SOCK_BUFFER_LEN 50000
// microseconds
#define THREAD_WAIT 100000 
#define SELECT_WAIT 1000

#define LOGIN_SUCCESS " login successful."
#define LOGOUT_SUCCESS " logout successful."

using namespace std;

struct Packet {
    char data[MY_PACKET_LEN];
};

void get_tokens(string line, string* tokens);
int send_packet_to_socket(int sockfd, Packet *packet);

#endif
