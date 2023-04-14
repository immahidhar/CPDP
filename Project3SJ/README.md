//Chat Server and Client

This is a simple chat server and client implemented in C using sockets and pthread library for multithreading and select for multiplexing. The server and client communicate over TCP/IP protocol. The server can handle multiple clients simultaneously, and each client can send and receive messages to and from the server.

To compile all programs at a time, use the following command:
<make>


//Server

The server code is implemented in the server.c file. The select function is used to handle multiple client connections efficiently in a single thread.
The signal() function is used to register a signal handler function, signalHandler(), for handling signals in the code. The signalHandler() function is called when a signal is received by the program. SignalHandler() is used to handle the SIGINT signal, which is sent to the program when the user presses Ctrl+C in the terminal.

//How to use the server

To compile, use the following command:
<make server.x>

To run the server, use the following command:
<./server.x server.conf>

The server reads the configuration parameters from a configuration file (server.conf) which contains the server port. It opens a socket and listens on this port for client communications.
You can modify this parameter in the configuration file according to your requirements.

// Server Limitation

Server program is coded to handle only 10 clients at a time. If you want to use more clients, change the MAX_CLIENTS value in server.c file.


//Client

The client code is implemented in the client.c file. The client connects to the server using TCP/IP protocol and sends and receives messages to and from the server. The server can receive only 1024 bytes of data at a time. If you want to work with more data, change the BUFFER_SIZE value in server.c file.

//How to run the client

To compile the client, use the following command:
<make client.x>

To run the client, use the following command:
<./client.x client.conf>

The client takes a configuration file argument. It opens a socket and connects to server based on the host and port information given in the configuration file. Once connected to the server, the client can send and receive messages to and from other clients through the server.

// Client Limitation

Client program is coded to send and receive only 1024 bytes of data at a time. If you want to work with more data, change the BUFFER_SIZE and MAX_MESSAGE_SIZE values in client.c file.


//Makefile

A Makefile is provided with the code to compile the server and client programs. To compile the server, client, and clean the generated files, you can use the following commands:

make            # compile server and client
make server.x   # compile server only
make client.x   # compile client only
make clean      # clean generated files

Note: The Makefile assumes that the gcc compiler is installed on your system. If you are using a different compiler, you may need to modify the Makefile accordingly.


//Usage of "pthread" and "select" functions

Server

 The select function is used indirectly through the handle_clients function, which waits for activity on the server socket and all connected client sockets using the select function.
 <activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);>

Client

The pthread library is used to create and manage threads for concurrent execution of different parts of the program. Specifically, the pthread_create() function is used to create a separate thread: one for reading messages from server(read_thread) and main thread takes in user command input and sends to server.


 