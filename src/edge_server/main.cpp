#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include "client_protocol.h"
#include "main_server_protocol.h"
using namespace std;

// TODO: Replace with real IP
#define MAIN_SERVER_IP "127.0.0.1"
#define MAIN_SERVER_PORT 3333
#define PORT 2222
#define EDGE_SERVER_IP "127.0.0.1"

int main() {
  // Creating a child process that will communicate with the main server about the resources, cache, TTL, updates etc.
  pid_t main_server_child = fork();
  switch (main_server_child)
  {
    case -1:
      throw runtime_error("fork() main_server_child failed");
    case 0:
      {
        // The actual child that will communicate with the main server
        int main_server_sd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in main_server;
        main_server.sin_family = AF_INET;
        main_server.sin_addr.s_addr = inet_addr(MAIN_SERVER_IP);
        main_server.sin_port = htons(MAIN_SERVER_PORT);
        if (connect(main_server_sd, (struct sockaddr *)&main_server, sizeof(struct sockaddr_in)) == -1)
          throw runtime_error("main_server_connect failed");
        cout << "Connected to the main server on PORT: " << MAIN_SERVER_PORT << "\n";
        treat_main_server(main_server_sd);
      }
    default:
      {
        // Communicate with the clients and treat them in a concurrent manner
        // Setting up the server
        sockaddr_in edge_server;
        edge_server.sin_family = AF_INET;
        edge_server.sin_addr.s_addr = inet_addr(EDGE_SERVER_IP); // TODO: replace with edge-server's public IPv4
        edge_server.sin_port = htons(PORT);
        const int socket_sd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_sd == -1)
          throw runtime_error("socket() failed");
        if (bind(socket_sd, (struct sockaddr *)&edge_server, sizeof(struct sockaddr_in)) == -1)
          throw runtime_error("bind() failed");
        if (listen(socket_sd, 5) == -1)
          throw runtime_error("listen() failed");
        treat_clients(socket_sd);
      }
  }
  return 0;
}