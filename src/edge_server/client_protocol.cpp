#include "client_protocol.h"

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <fstream>
#include <sys/stat.h>

#include "../client/include/protocol.h"
using namespace std;


// TODO: organize the code in the header
enum RequestType
{
    EXEC,
    SEND
};

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
    // TODO: parse different types of request
    return RequestType::EXEC;
}

void exec_script(char* packet, string* response)
{
    char work_dir[128];
    getcwd(work_dir, 128);
    const unique_ptr<FILE, decltype(&pclose)> output(popen(packet, "r"), pclose);
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
    // TODO: make different types of behaviour based on the request type
    // TODO: but for now let's assume that the default is EXEC
    // TODO: (the server will execute a script and send back the response)

    // TODO: the server will receive the name of the script and will execute it
    // TODO: and send the response back to the client
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

void treat_clients(int socket_fd)
{

    char* received_packet = receive_packet(socket_fd);
    cout << "Received: " << received_packet << " - ";
    const RequestType type_of_request = parse_request(received_packet);
    string response = compute_request(received_packet, type_of_request);
    delete[] received_packet; // Freeing the memory
    if (response[0] == '\0')
        response = "Resource not found.";
    send_response(socket_fd, response);
    cout << "Sent back: " << response.c_str() << "\n";
}
