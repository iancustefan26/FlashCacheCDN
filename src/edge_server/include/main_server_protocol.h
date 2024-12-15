#pragma once
#ifndef MAIN_SERVER_H
#define MAIN_SERVER_H
#include <string>

using namespace std;

// Will communicate with the main server : receiving updates, manage caching, loading resources etc.
void treat_main_server(int main_server_sd, string edge_server_private_ip);


#endif //MAIN_SERVER_H
