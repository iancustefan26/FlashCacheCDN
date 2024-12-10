#pragma once
#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "utils.h"
using namespace std;

#define EDNS_PORT 5053


// Establish connection between the client and the EDNS0 server
inline void connect_to_server(const int& socket_fd, const char* server_ip, const int server_port)
{
    sockaddr_in dns_server;
    dns_server.sin_family = AF_INET;
    dns_server.sin_port = htons(server_port);
    dns_server.sin_addr.s_addr = inet_addr(server_ip);
    if (connect(socket_fd, (sockaddr*)&dns_server, sizeof(dns_server)) < 0)
        throw logic_error("Connecting to the server failed");
    cout << "Conneted to the server with IP: " << server_ip << " PORT: " << server_port << "\n";
}


// Function that makes a DNS request through the main socket for the most convenient edge-server
in_addr dns_get_request(int socket_fd, const string& resource, const string& public_ip);

// Asking for the resource to the edge-server using its socket
void send_get_resource(int edge_server_sd, const string& request);

// Wait for the resource-response from the edge-server
string wait_for_response(int edge_server_sd);

#endif