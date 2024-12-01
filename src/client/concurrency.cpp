#include "concurrency.h"

#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

void parent_work(int &request_number, string& resource)
{
    request_number++;
    cout << request_number << ".Request a resource or q to quit: ";
    cin >> resource;
    if (resource == "q")
    {
        cout << "Quit.\n";
        exit(EXIT_SUCCESS);
    }
    if (resource == "clear")
        clear_screen();
}

void child_work_send_edns0_request(const string& resource, const char* dns_server_ip, const int request_number, const string& public_ip)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        throw runtime_error("socket for EDNS0 failed");
    sockaddr_in dns_server;
    dns_server.sin_family = AF_INET;
    dns_server.sin_port = htons(EDNS_PORT);
    dns_server.sin_addr.s_addr = inet_addr(dns_server_ip); // argv[1] = EDNS0's IP address
    if (connect(socket_fd, (sockaddr*)&dns_server, sizeof(dns_server)) < 0)
        throw runtime_error("connect to EDNS0 failed");

    // Creating the packet
    const size_t dns_packet_length = resource.length() + 1 + sizeof(in_addr_t);
    // length header + sender's public IPv4 (sizeof(uint32_t)) + request + NULL
    const auto dns_packet = new unsigned char[dns_packet_length + sizeof(size_t)];
    const auto pub_ipv4n_addr = inet_addr(public_ip.c_str());
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
    close(socket_fd);
    const string edge_server_ip = inet_ntoa(dns_response);
    cout << "\nResponse from DNS server for request #" << request_number << " (" << resource << "): " << edge_server_ip
        << "\n";
    exit(EXIT_SUCCESS);
}
