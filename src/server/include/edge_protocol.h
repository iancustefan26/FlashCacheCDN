#pragma once
#ifndef EDGE_PROTOCOL_H
#define EDGE_PROTOCOL_H
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <nlohmann/json_fwd.hpp>

using namespace std;

void append_to_info_file(const string& json_buffer);

inline void  handle_edge_server(int index, int socket_sd)
{
    sockaddr_in edge;
    socklen_t len_edge = sizeof(edge);
    int egde_sd = accept(socket_sd, (sockaddr*)&edge, &len_edge);
    if (egde_sd == -1)
        cout << "Process with PID: " << getpid() << " handling edge-server #"
         << index << " with IP: " << inet_ntoa(edge.sin_addr) << "\n";
    while (true)
    {
        // Provizor function
        //sleep(10);
        size_t packet_size;
        char* json_buffer = new char[2048]; // Couldn't be more than that
        size_t bytes_received = recv(egde_sd, &packet_size, sizeof(size_t), 0);
        cout << "PID: " << getpid() << " handling edge-server #" << index << "\n";
        if (bytes_received == -1)
            throw logic_error("recv() failed");
        if (bytes_received == 0)
            throw logic_error("Connection with one of edge-servers was close gracefully");
        bytes_received = recv(egde_sd, json_buffer, packet_size - sizeof(size_t), 0);
        json_buffer[bytes_received] = '\0';
        cout << json_buffer << "\n";
        append_to_info_file(json_buffer);
        delete[] json_buffer;
    }
}

#endif
