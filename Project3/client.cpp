#include "client.h"

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

/**
 * entry point for server
*/
int main(int argc, char** argv) {
    // TODO: connect to server

    // infinite loop
    while (1) {
        cout << "$ ";
        string line;
        getline(std::cin, line);
        //cout << line << endl;
        string tokens[TOKEN_LIMIT];
        get_tokens(line, tokens);
        process_command(tokens);
    }
    return 0;
}
