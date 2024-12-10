#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include "client_protocol.h"
#include "concurrency.h"
#include "main_server_protocol.h"
#include "utils.h"
#include <fstream>
#include "configparser.hpp"
#include "concurrency.h"
using namespace std;

// TODO: Replace with real IP
#define MAIN_SERVER_PORT 3333
#define PORT 2222

const string config_path = "../src/edge_server/config/config.ini";

int main(int argc, char *argv[]) {
  if (argc != 2)
  {
    cout << "Bad usage: " << argv[0] << " <main_server_private_ip>";
    return EXIT_SUCCESS;
  }
  // Initializing the settings from the config file
  ConfigParser parser = ConfigParser(config_path);
  // TODO: for now this will be a constant number but it will more accurate in the future based on the number of clients
  int thread_count = parser.aConfig<int>("pool", "poolsize");

  string edge_server_private_ip = get_private_ipv4();
  string main_server_ip = argv[1];
  // Creating a child process that will communicate with the main server about the resources, cache, TTL, updates etc.
  pid_t main_server_child = fork();
  switch (main_server_child)
  {
    case -1:
      throw runtime_error("fork() main_server_child failed");
    case 0:
      {
        // The actual child process that will communicate with the main server
        // TODO: REUSEADDR
        int main_server_sd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in main_server;
        main_server.sin_family = AF_INET;
        main_server.sin_addr.s_addr = inet_addr(main_server_ip.c_str());
        main_server.sin_port = htons(MAIN_SERVER_PORT);
        if (connect(main_server_sd, (struct sockaddr *)&main_server, sizeof(struct sockaddr_in)) == -1)
          throw runtime_error("main_server_connect failed");
        cout << "Connected to the main server on PORT: " << MAIN_SERVER_PORT << "\n";
        treat_main_server(main_server_sd);
        break;
      }
    default:
      {
        // Communicate with the clients and treat them in a concurrent manner
        // Setting up the server
        // TODO: REUSEADDR
        sockaddr_in edge_server;
        edge_server.sin_family = AF_INET;
        edge_server.sin_addr.s_addr = inet_addr(edge_server_private_ip.c_str()); // For real deployment: replace with edge-server's public IPv4
        edge_server.sin_port = htons(PORT);
        const int socket_sd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_sd == -1)
          throw runtime_error("socket() failed");
        if (bind(socket_sd, (struct sockaddr *)&edge_server, sizeof(struct sockaddr_in)) == -1)
          throw runtime_error("bind() failed");
        if (listen(socket_sd, 5) == -1)
          throw runtime_error("listen() failed");
        cout << "Edge server listening on IP: " << edge_server_private_ip << " PORT: " << PORT << "\n";
        constexpr int optval = 1;
        if (setsockopt(socket_sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
          throw runtime_error("setsockopt() failed");
        ThreadPool thread_pool(thread_count, treat_clients, socket_sd);
        thread_pool.join_all();
        cout << "Server running on: " << thread_count << " threads\n";
      }
  }
  return 0;
}