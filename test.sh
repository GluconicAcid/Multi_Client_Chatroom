#!/bin/bash

SERVER_IP="127.0.0.1"

g++ -o server server.cpp
g++ -o client client.cpp

./server &
SERVER_PID=$!

echo "Server started with PID $SERVER_PID"

for i in $(seq 1 3); do
    gnome-terminal --title="Client $i" -- bash -c "./client $SERVER_IP; exec bash" &
done

echo "3 clients started"
