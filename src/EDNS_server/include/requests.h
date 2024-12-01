#pragma once
#ifndef REQUESTS_H
#define REQUESTS_H
#include <iostream>
#include <netdb.h>

using namespace std;

in_addr_t handle_request(const char* request, const size_t length);

void respond_to_client(const int socket_fd);



#endif