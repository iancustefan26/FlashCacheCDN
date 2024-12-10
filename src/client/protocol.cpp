#include "protocol.h"

in_addr dns_get_request(int socket_fd, const string& resource, const string& public_ip)
{
    // Creating the packet
    const size_t dns_packet_length = sizeof(size_t) + resource.length() + 1 + sizeof(in_addr_t);
    // length header (4 bytes) + sender's public IPv4 (4 bytes) + request + nullptr
    char* dns_packet = new char[dns_packet_length];
    const in_addr_t pub_ipv4n_addr = inet_addr(public_ip.c_str());
    memcpy(dns_packet, &dns_packet_length, sizeof(size_t));
    memcpy(dns_packet + sizeof(size_t), resource.c_str(), resource.length() + 1);
    memcpy(dns_packet + sizeof(size_t) + resource.length() + 1, &pub_ipv4n_addr, sizeof(in_addr_t));

    // Sending the packet to the EDNS0 server
    size_t bytes_sent = send(socket_fd, dns_packet, dns_packet_length, 0);
    if (bytes_sent == -1)
    {
        close(socket_fd);
        delete[] dns_packet; // Freeing the allocated memory
        throw runtime_error("Packet sending to EDNS0 server failed");
    }
    if (bytes_sent == 0)
    {
        close(socket_fd);
        delete[] dns_packet; // Freeing the allocated memory
        throw runtime_error("The connection was close gracefully");
    }
    delete[] dns_packet; // Freeing the allocated memory
    in_addr dns_response = {0};
    // Receiving the response from the EDNS0 server
    size_t bytes_received = recv(socket_fd, &dns_response, sizeof(in_addr_t), 0);
    if (bytes_received == -1)
        throw runtime_error("Packet receiving from EDNS0 server failed");
    if (bytes_received == 0)
        throw runtime_error("The connection was close gracefully");

    return dns_response;
}

void send_get_resource(int edge_server_sd, const string& resource)
{
    // Creating the packet
    const size_t packet_size = resource.length() + sizeof(size_t) + 1;
    char* packet = new char[packet_size];
    memcpy(packet, &packet_size, sizeof(size_t)); // Lenght header of the packet
    memcpy(packet + sizeof(size_t), resource.c_str(),  packet_size - sizeof(size_t)); // Adding the resource to the packet
    printf("Packet sent: %s\n", packet);
    size_t sent_bytes = send(edge_server_sd, packet, packet_size, 0);
    delete[] packet; // Freeing the memory
    if (sent_bytes == -1)
        throw logic_error("Packet sending to edge-server failed");
    if (sent_bytes == 0)
        throw logic_error("Packet sending to edge-server failed: Connection was closed gracefully");
}

string wait_for_response(int edge_server_sd)
{
    size_t response_size;
    size_t received_bytes = recv(edge_server_sd, &response_size, sizeof(size_t), 0);
    if (received_bytes == -1)
        throw logic_error("Packet receiving from edge-server failed");
    if (received_bytes == 0)
        throw logic_error("Packet receiving from edge-server failed: Connection was closed gracefully");
    char* response = new char[response_size + 1];
    received_bytes = recv(edge_server_sd, response, response_size - sizeof(size_t), 0);
    string return_response = response;
    delete[] response; // Freeing the memory
    if (received_bytes == -1)
        throw logic_error("Packet receiving from edge-server failed");
    if (received_bytes == 0)
        throw logic_error("Packet receiving from edge-server failed: Connection was closed gracefully");

    return return_response;
}