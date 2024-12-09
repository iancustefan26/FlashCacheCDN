
#ifndef EDGE_PROTOCOL_H
#define EDGE_PROTOCOL_H
#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <usable.h>

inline void  handle_edge_server(int index, int socket_sd)
{
    sockaddr_in edge;
    socklen_t len_edge = sizeof(edge);
    int egde_sd = accept(socket_sd, (sockaddr*)&edge, &len_edge);
    if (egde_sd == -1)
        throw runtime_error("accept() failed");
    cout << "Process with PID: " << getpid() << " handling edge-server #"
         << index << " with IP: " << inet_ntoa(edge.sin_addr) << "\n";
    while (true)
    {
        sleep(10);
        cout << "PID: " << getpid() << " handling edge-server #" << index << "\n";
    }
}

#endif
