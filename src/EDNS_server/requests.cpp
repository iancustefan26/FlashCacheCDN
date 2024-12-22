#include "requests.h"
#include "cache.h"

in_addr_t handle_request(const char* request, const size_t length, Cache* mapped_cached_content)
{
    // Make a decision based on the resource requested ( first n - 4 bytes ), the geographical location of the client's IP
    // address (last 4 bytes), the load of the edge-servers, and the cached content of them
    cout << "Received payload size: " << length << "\n";
    char resource[length - sizeof(in_addr_t)];
    in_addr client_address;
    memcpy(resource, request, length - sizeof(in_addr));
    cout << "Resource requested: " << request << "\n";
    memcpy(&client_address, request + length - sizeof(in_addr), sizeof(in_addr));
    cout << "Client's public IPv4 address: " << inet_ntoa(client_address) << "\n";
    const in_addr_t chosen_edge_server = mapped_cached_content->decide(resource, client_address.s_addr);
    cout << "Chosen edge server's IP: " << inet_ntoa({chosen_edge_server}) << "\n";
    return chosen_edge_server;
}

void respond_to_client(fd_set* active_fds, int client_fds, Cache* mapped_cached_content)
{
    // Receiving the packet and parsing it
    size_t packet_size;
    size_t bytes_received = recv(client_fds, &packet_size, sizeof(size_t), 0);
    if (bytes_received == -1)
        throw runtime_error("recv() failed");
    if (bytes_received == 0)
    {
        // connection was gracefully closed by the user
        close(client_fds);
        FD_CLR(client_fds, active_fds);
        return;
    }
    char request[packet_size];
    bytes_received = recv(client_fds, request, packet_size - sizeof(size_t), 0);
    if (bytes_received == -1)
        throw runtime_error("recv() failed");
    if (bytes_received == 0)
    {
        // connection was gracefully closed by the user
        close(client_fds);
        FD_CLR(client_fds, active_fds);
        return;
    }
    cout << "--------------------------\n";
    // Make a decision based on available edge-server's caching, load and geolocation
    const in_addr_t response = handle_request(request, packet_size - sizeof(size_t), mapped_cached_content);
    cout << "--------------------------\n";
    // And send the edge-server's IP address back to the client
    size_t bytes_sent = send(client_fds, &response, sizeof(in_addr_t), 0);
    if (bytes_sent == -1)
        throw runtime_error("Sending response to client failed");
    if (bytes_sent == 0)
        throw logic_error("The connection was closed gracefully");

}