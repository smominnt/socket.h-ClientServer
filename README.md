# socket.h-ClientServer
Client and Server project Spring 2018

This program is a simple implementation of socket.h to create a client and server.

Compile client and server using c++11

ex:
g++ -std=c++11 client.cpp -o client
g++ -std=c++11 server.cpp -o server

Use:

./Server

There is a prompt to load input file and choose a port.

./Client

There is a prompt to enter server host name (localhost) and port.

Once a connection is established, there will be a prompt to input a college major. 
The program will query the server for the input and will send a response back to the client. 
If server is closed while the client is running, it will state failure to connect and terminate.
