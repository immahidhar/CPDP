# Project 3 - A Simple Chat Application

Class: COP 5570

Professor: Zhenhai Duan

Name: Mahidhar Reddy Narala (MN22L)

This is a simple chat application programmed in C++ language with TCP socket connections. The application consists of two programs - server and client. To use the application, compile all programs and run server program. Then run as many client programs as needed and enter client commands to chat as described below.


**To compile all programs at a time, execute the following in terminal**
```
make
```

**To cleanup the compiled executables, execute the following in terminal**
```
make clean
```

## Server

The chat server program opens up a socket and binds it to the port number provided in the configuration file and waits for client connections on it. When clients login, they can provide username and the server mantains this information so that clients can chat with each other.

The server creates a separate thread for each client to handle the connection using pthread library. Each client command request is handled by the corresponding thread only. In addition, select function is used to let the program know when there is data ready on the socket to read.

**To compile chat server program, execute the following in terminal**
```
make server.x
```

**To run chat server program, execute the following in terminal**
```
./server.x chat_server_configuration_file
```

The "chat_server_configuration_file" consists of only one configuration i.e. the port number on which the server listens to. Provide the port number you want the server to use here.

## Client

The chat client program opens up a socket and connects to the chat server based on the configuration provided in the config file. The user can enter the commands described below and chat with other users.

The client program creates a separate thread to read data from server and print to console using pthread library. The main thread waits for user input on console and processes/sends the command entered to server. In addition, select function is used to let the program know when there is data read on the socket to read.

**To compile chat client program, execute the following in terminal**
```
make client.x
```

**To run chat client program, execute the following in terminal**
```
./client.x chat_client_configuration_file
```

The "chat_client_configuration_file" consists of two configurations required to connect to chat server i.e. the hostname/ipaddress and the port number of the chat server.

**Client Commands:**

***Login:***
```
$ login username
```

***Chat:***

Chat with a particular user:
```
$ chat @username message
```
Broadcast a chat message to all users:
```
$ chat broadcast_message
```

***Logout:***
```
$ logout
```

***Exit:***
```
$ exit
```
