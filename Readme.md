# Multi Client Chatroom

A simple multi-client chatroom built using **C++ sockets** and `select()`.  
It allows multiple clients to connect to a server and exchange messages in real-time.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Multi_Client_Chatroom.git
   cd Multi_Client_Chatroom

2. Compile the server and client:
    ```bash
    g++ server.cpp -o server
    g++ client.cpp -o client

    or 
    
    make

## Usage

1. Start the server:
    ```bash
    ./server

The server will listen on port 8080.

2. Start clients:
    Provide the server IP address when starting a client:
    ```bash
    ./client 127.0.0.1

Type a message in the client terminal and press Enter to send.

All connected clients will receive it.

If the server closes, clients will see Server Terminated.

## Notes
Default port: 8080

Maximum clients: Limited by FD_SETSIZE (usually 1024)

To exit a client: Press Ctrl+C

## Testing

To test the server and client run the shell script test.sh