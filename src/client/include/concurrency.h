#pragma once
#ifndef CONCURRENCY_H
#define CONCURRENCY_H
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "usable.h"
using namespace std;

#define EDNS_PORT 5053

// TODO: Provizor, the work with I/O but it will be probably removed or modified
void parent_work(int &request_number, string& resource);

// TODO: Provizor, the work with the DNS server but it is already replaced within the main logic and
// TODO: will be probably removed
void child_work_send_edns0_request(const string& resource, const char* dns_server_ip, const int request_number, const string& public_ip);


// Establish connection between the client and the EDNS0 server
inline void connect_to_edns0_server(const int& socket_fd, const char* dns_server_ip)
{
    sockaddr_in dns_server;
    dns_server.sin_family = AF_INET;
    dns_server.sin_port = htons(EDNS_PORT);
    dns_server.sin_addr.s_addr = inet_addr(dns_server_ip);
    if (connect(socket_fd, (sockaddr*)&dns_server, sizeof(dns_server)) < 0)
        throw logic_error("connect to EDNS0 failed");
}

// Function that makes a DNS request through the main socket for the most convenient edge-server
in_addr dns_get_request(int socket_fd, const string& resource, const string& public_ip);

#endif