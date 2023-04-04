#include "common.h"

/**
 * split line based on space character and return two strings if there is a space
*/
void get_tokens(string line, string* tokens) {
    int i = 0;
    for(i = 0; i < TOKEN_LIMIT; i++)
        tokens[i] = "NULL";
    string delimiter = " ";
    size_t pos = line.find(delimiter);
    if(pos == string::npos) {
        tokens[0] = line;
    } else{
        tokens[0] = line.substr(0, pos);
        line.erase(0, pos + delimiter.length());
        tokens[1] = line;
    }
}

/**
 * sends packet to socket
*/
int send_packet_to_socket(int sockfd, Packet *packet) {
    unsigned char buf[MY_SOCK_BUFFER_LEN];
    memcpy(&(buf[0]), (char *)packet, sizeof(Packet));
    int send_result = send(sockfd, (void*)(buf), sizeof(Packet), 0);
    return send_result;
}
