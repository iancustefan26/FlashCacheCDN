#include "client_protocol.h"
#include "concurrency.h"

using namespace std;

char* receive_packet(int socket_fd)
{
    size_t packet_size;
    size_t bytes_received = recv(socket_fd, &packet_size, sizeof(size_t),  0);
    if (bytes_received == -1)
        throw logic_error("Receiving packet from client failed");
    if (bytes_received == 0)
        throw logic_error("Connection with the client was closed gracefully");
    char *packet_buffer = new char[packet_size];
    bytes_received = recv(socket_fd, packet_buffer, packet_size - sizeof(size_t), 0);
    if (bytes_received == -1)
    {
        delete[] packet_buffer;
        throw logic_error("Receiving packet from client failed");
    }
    if (bytes_received == 0)
    {
        delete[] packet_buffer;
        throw logic_error("Connection with the client was closed gracefully");
    }

    return packet_buffer;
}

RequestType parse_request(char *packet)
{
    // TODO: Recognize different types of request
    // Future features
    return RequestType::EXEC;
}

void exec_script(const char* packet, string* response)
{
    // A more C++ like style to make an auto-destructible pointer with popen function
    // using unique_ptr and lambda functions
    const unique_ptr<FILE, void(*)(FILE*)> output(popen(packet, "r"), [](FILE* file) { pclose(file); });

    fstream output_file;
    if (!output)
        throw logic_error("Failed to run script");
    char buffer[128];
    buffer[0] = '\0';
    while (fgets(buffer, 128, output.get()) != nullptr)
        &response -> append(buffer);
}

string compute_request(char *packet, RequestType request_type)
{
    // (the edge-server will execute a script and send back the response to the client)
    string response;
    exec_script(packet, &response);
    return response;
}

void send_response(int socket_fd, const string& response)
{
    size_t packet_size = response.size() + sizeof(size_t) + 1;
    char* packet = new char[packet_size];
    memcpy(packet, &packet_size, sizeof(size_t));
    memcpy(packet + sizeof(size_t), response.c_str(), packet_size - sizeof(size_t));
    const size_t bytes_sent = send(socket_fd, packet, packet_size, 0);
    if (bytes_sent == -1)
        throw logic_error("Sending response to client failed");
    if (bytes_sent == 0)
        throw logic_error("Connection with the client was closed gracefully");
    delete[] packet;
}

void treat_clients(int socket_sd)
{
    while (true)
    {
        sockaddr_in client;
        socklen_t addr_len = sizeof(client);
        ThreadPool::accept_mtx.lock();
        int client_sd = accept(socket_sd, ( sockaddr *)&client, &addr_len);
        ThreadPool::accept_mtx.unlock();
        cout << "Received connection from IP: " << inet_ntoa(client.sin_addr) << "\n";
        char* received_packet = receive_packet(client_sd);
        cout << "Received: " << received_packet << " - ";
        const RequestType type_of_request = parse_request(received_packet);
        string response = compute_request(received_packet, type_of_request);
        delete[] received_packet; // Freeing the memory
        if (response[0] == '\0')
            // Cache MISS
            response = "Resource not found.";
        send_response(client_sd, response);
        close(client_sd);
        cout << "Sent back: " << response.c_str() << "\n";
    }
}
