#include "requests.h"
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

in_addr_t handle_request(const char* request, const size_t length)
{
    // Make a decision based on the resource requested ( first n - 4 bytes ), the geographical location of the client's IP
    // add
    // ress (last 4 bytes), the load of the edge-servers, and the cached content of them
    cout << "Received payload size: " << length << "\n";
    char resource[length - 4];
    in_addr client_address;
    memcpy(resource, request, length - 4);
    cout << "Resource requested: " << request << "\n";
    memcpy(&client_address, request + length - 4, 4);
    cout << "Client's public IPv4 address: " << inet_ntoa(client_address) << "\n";
    return client_address.s_addr; // Provizor
    return 0; // Provizor
}

void respond_to_client(const int socket_fd)
{
    // Creating client socket
    sockaddr_in client;
    socklen_t len = sizeof(client);
    const int client_fd = accept(socket_fd, (struct sockaddr *)&client, &len);
    if (client_fd == -1)
        throw runtime_error("accept() failed");

    // Receiving the packet and parsing it
    size_t packet_size;
    if (recv(client_fd, &packet_size, sizeof(size_t), 0) == -1)
        throw runtime_error("recv() failed");
    char request[packet_size];
    if (recv(client_fd, request, packet_size, 0) == -1)
        throw runtime_error("recv() failed");
    cout << "--------------------------\n";

    // Make a decision based on available edge-server's caching
    const in_addr_t response = handle_request(request, packet_size);
    cout << "--------------------------\n";
    if (send(client_fd, &response, sizeof(in_addr_t), 0) == -1)
        throw runtime_error("Sending response to client failed");
    close(client_fd);
}