#pragma once
#ifndef REQUESTS_H
#define REQUESTS_H

#include "cache.h"

using namespace std;

// For incoming requests from clients
in_addr_t handle_request(const char* request, size_t length, Cache* mapped_cached_content);

// Receiving the request and giving the response back to the client with chosen edge-server's IP
void respond_to_client(fd_set* active_fds, int client_fds, Cache* mapped_cached_content);



#endif