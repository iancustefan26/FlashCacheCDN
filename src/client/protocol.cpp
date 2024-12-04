#include "protocol.h"
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>


in_addr dns_get_request(int socket_fd, const string& resource, const string& public_ip)
{
    // Creating the packet
    const size_t dns_packet_length = resource.length() + 1 + sizeof(in_addr_t);
    // length header + sender's public IPv4 (sizeof(uint32_t)) + request + nullptr
    unsigned char* dns_packet = new unsigned char[dns_packet_length + sizeof(size_t)];
    const in_addr_t pub_ipv4n_addr = inet_addr(public_ip.c_str());
    memcpy(dns_packet, &dns_packet_length, sizeof(size_t));
    memcpy(dns_packet + sizeof(size_t), resource.c_str(), resource.length() + 1);
    memcpy(dns_packet + sizeof(size_t) + resource.length() + 1, &pub_ipv4n_addr, sizeof(in_addr_t));

    // Sending the packet to the EDNS0 server
    if (send(socket_fd, dns_packet, dns_packet_length + sizeof(size_t), 0) == -1)
    {
        close(socket_fd);
        delete[] dns_packet; // Freeing the allocated memory
        throw runtime_error("Packet sending to EDNS0 server failed");
    }
    delete[] dns_packet; // Freeing the allocated memory
    in_addr dns_response = {0};
    // Receiving the response from the EDNS0 server
    if (recv(socket_fd, &dns_response, sizeof(in_addr_t), 0) == -1)
        throw runtime_error("Packet receiving from EDNS0 server failed");

    return dns_response;
}
