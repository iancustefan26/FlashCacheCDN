#pragma once
#ifndef EDGE_PROTOCOL_H
#define EDGE_PROTOCOL_H
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json_fwd.hpp>
#include <unistd.h>
#include <sys/file.h>

using namespace std;

#define JSON_FILE "../src/server/assets/edge_servers_control.json"


void append_to_info_file(const string& json_buffer);

inline void  handle_edge_server(int index, int socket_sd)
{
    sockaddr_in edge;
    socklen_t len_edge = sizeof(edge);
    int edge_sd = accept(socket_sd, (sockaddr*)&edge, &len_edge);
    if (edge_sd == -1)
        cout << "Process with PID: " << getpid() << " handling edge-server #"
         << index << " with IP: " << inet_ntoa(edge.sin_addr) << "\n";
    while (true)
    {
        size_t packet_size;
        size_t bytes_received = recv(edge_sd, &packet_size, sizeof(size_t), 0);
        if (bytes_received == -1)
            throw logic_error("recv() failed");
        if (bytes_received == 0)
            throw logic_error("Connection with one of edge-servers was close gracefully");
        cout << "PID: " << getpid() << " handling edge-server #" << index << "\n";
        char* json_buffer = new char[packet_size];
        bytes_received = recv(edge_sd, json_buffer, packet_size - sizeof(size_t), 0);
        if (bytes_received == -1)
            throw logic_error("recv() failed");
        if (bytes_received == 0)
            throw logic_error("Connection with one of edge-servers was close gracefully");
        json_buffer[bytes_received] = '\0';
        cout << json_buffer << "\n";
        append_to_info_file(json_buffer);
        delete[] json_buffer;
    }
}

#endif
