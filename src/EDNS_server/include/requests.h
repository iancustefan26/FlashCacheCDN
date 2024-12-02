#pragma once
#ifndef REQUESTS_H
#define REQUESTS_H
#include <iostream>
#include <netinet/in.h>

using namespace std;

in_addr_t handle_request(const char* request, const size_t length);

void respond_to_client(fd_set* actfds, int client_fd);



#endif