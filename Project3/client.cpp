#include "client.h"

int cl_sock_fd;

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
        perror("SERVERCONFIG:");
        exit(1);
    }
}

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
    /*for(i = 0; i < TOKEN_LIMIT; i++)
        cout << tokens[i] << endl;*/
}

/**
 * process command entered
*/
void process_command(string* tokens) {
    if(tokens[0] == "exit") {
        cout << "exiting" << endl;
        // TODO: close connection
        exit(0);
    } else if(tokens[0] == "login") {
        cout << "logging in user" << endl;
        // TODO: implementation
    } else if(tokens[0] == "logout") {
        cout << "logging out user" << endl;
        // TODO: implementation
    } else if(tokens[0] == "chat") {
        cout << "chat impl yet to be done!" << endl;
        // TODO: implementation
    } else {
        fprintf(stderr, "incorrect command entered\n");
    }
}

void client_init(void) {
    struct hostent *he_server;
    if ((he_server = gethostbyname(SERVERHOST)) == NULL) {
        perror("gethostbyname:");
        cerr << "error resolving hostname for server" << SERVERHOST << endl;
        exit(1);
    }

    struct sockaddr_in  server;
    memcpy(&server.sin_addr, he_server->h_addr_list[0], he_server->h_length);
    strcpy(SERVERIP, inet_ntoa(server.sin_addr));

    cout << "Connecting to " << SERVERHOST << ":" << SERVERPORT << " ..." << endl;
    if ((cl_sock_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket:");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    inet_aton(SERVERIP, (struct in_addr *)&server_addr.sin_addr.s_addr);
    memset(&(server_addr.sin_zero), '\0', 8);

    // connect
    if (connect(cl_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect:");
        cerr << "Error connecting to the server " << SERVERIP << " on port " << SERVERPORT << endl;
        cl_sock_fd = -1;
        exit(1);
    }
    cout << "Connected to " << SERVERIP << ":" << SERVERPORT << " ..." << endl;
}

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    read_config(CLIENTCONFIG);
    // TODO: connect to server
    client_init();
    // infinite loop
    while (1) {
        cout << "$ ";
        string line;
        getline(cin, line);
        string tokens[TOKEN_LIMIT];
        get_tokens(line, tokens);
        process_command(tokens);
    }
    return 0;
}
