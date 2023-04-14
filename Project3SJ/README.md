//Chat Server and Client

This is a simple chat server and client implemented in C using sockets and pthread library for multithreading. The server and client communicate over TCP/IP protocol. The server can handle multiple clients simultaneously using threads, and each client can send and receive messages to and from the server.

//Server

The server code is implemented in the server.c file. The select function is used to handle multiple client connections efficiently in a single thread.
The signal() function is used to register a signal handler function, signalHandler(), for handling signals in the code. The signalHandler() function is called when a signal is received by the program. SignalHandler() is used to handle the SIGINT signal, which is sent to the program when the user presses Ctrl+C in the terminal.

//How to run the server
To compile and run the server, use the following commands:

<gcc server.c -o server -lpthread>
<./server>

The server reads the configuration parameters from a configuration file (config.txt) which contains the server address, server port, buffer size for messages, and maximum number of clients. You can modify these parameters in the configuration file according to your requirements.

//Client

The client code is implemented in the client.c file. The client connects to the server using TCP/IP protocol and sends and receives messages to and from the server.

//How to run the client

To compile and run the client, use the following commands:

<gcc client.c -o client -lpthread>
< ./client <username> >

The client takes a username as a command-line argument, which is sent to the server as part of the login process. Once connected to the server, the client can send and receive messages to and from other clients through the server.

//Makefile

A Makefile is provided with the code to compile the server and client programs. To compile the server, client, and clean the generated files, you can use the following commands:

make            # compile server and client
make server     # compile server only
make client     # compile client only
make clean      # clean generated files

Note: The Makefile assumes that the gcc compiler is installed on your system. If you are using a different compiler, you may need to modify the Makefile accordingly.

//usage of pthread and select functions

Server code

 The select function is used indirectly through the handle_clients function, which waits for activity on the server socket and all connected client sockets using the select function.
 <activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);>

Client code

The pthread library is used to create and manage threads for concurrent execution of different parts of the program. Specifically, the pthread_create() function is used to create two threads: one for sending messages (send_thread) and another for receiving messages (receive_thread).



 