#pragma once
#ifndef DNS_SERVER_PROTOCOL_H
#define DNS_SERVER_PROTOCOL_H
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>

#define JSON_FILE "../src/server/assets/edge_servers_control.json"

using namespace std;

void send_json_info(int socket_sd);

inline void handle_dns_info_transmission(int socket_sd)
{
    sockaddr_in dns;
    socklen_t len_dns = sizeof(dns);
    int dns_sd = accept(socket_sd, (sockaddr*)&dns, &len_dns);
    if (dns_sd == -1)
        throw runtime_error("accept() failed");
    cout << "Parent process with PID: " << getpid() << " handling info transmissions to EDNS0 server"
         << " with IP: " << inet_ntoa(dns.sin_addr) << "\n";
    while (true)
    {
        sleep(10);
        cout << "Giving info about edge-servers to EDNS0 server...\n";
        send_json_info(dns_sd);
    }
}

#endif
