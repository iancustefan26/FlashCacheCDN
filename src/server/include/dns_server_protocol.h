
#ifndef DNS_SERVER_PROTOCOL_H
#define DNS_SERVER_PROTOCOL_H
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"

using namespace std;

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
        // Provizor function
        sleep(10);
        cout << "Giving info about edge-servers to EDNS0 server...\n";
    }
}

#endif
