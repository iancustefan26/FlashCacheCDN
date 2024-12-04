#pragma once
#ifndef REQUESTS_H
#define REQUESTS_H
#include <iostream>
#include <netinet/in.h>

#include "cache.h"

using namespace std;

in_addr_t handle_request(const char* request, size_t length, Cache* mapped_cached_content);

void respond_to_client(fd_set* active_fds, int client_fds, Cache* mapped_cached_content);



#endif