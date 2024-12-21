#pragma once
#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H
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

// Future features
enum RequestType
{
    EXEC,
    SEND
};

void treat_clients(int socket_fd); // Will develop a concurrent manner of serving clients

#endif
