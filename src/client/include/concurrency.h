#pragma once
#ifndef CONCURRENCY_H
#define CONCURRENCY_H
#include <iostream>
#include "usable.h"
using namespace std;

#define EDNS_PORT 5053

void parent_work(int &request_number, string& resource);

void child_work_send_edns0_request(const string& resource, const char* dns_server_ip, const int request_number, const string& public_ip);



#endif