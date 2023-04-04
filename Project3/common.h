#ifndef __COMMON_H

#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>

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
#define MY_PACKET_LEN 1000
#define MY_SOCK_BUFFER_LEN 3000

using namespace std;

struct Packet {
    char data[MY_PACKET_LEN];
};

void get_tokens(string line, string* tokens);
int send_packet_to_socket(int sockfd, Packet *packet);
void remove_read_from_buf(unsigned char *buf, int num);

#endif
