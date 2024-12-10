#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.h"
#include "dns_server_protocol.h"
#include "edge_protocol.h"
using namespace std;


// TODO: Replace this with the actual IPv4 addresses
#define PORT_FOR_EDGE 3333
#define PORT_FOR_DNS 6666
#define N_OF_EDGES 5


int main() {
  // Setting up the server
  string main_server_private_ip = get_private_ipv4();

  // Creating the socket that will listen for edge-servers
  sockaddr_in main_server_for_edge;
  main_server_for_edge.sin_family = AF_INET;
  main_server_for_edge.sin_addr.s_addr = inet_addr(main_server_private_ip.c_str());
  main_server_for_edge.sin_port = htons(PORT_FOR_EDGE);
  const int socket_edge_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_edge_sd == -1)
    throw runtime_error("socket() failed");
  if (bind(socket_edge_sd, (struct sockaddr *)&main_server_for_edge, sizeof(struct sockaddr_in)) == -1)
    throw runtime_error("bind() failed");
  if (listen(socket_edge_sd, 5) == -1)
    throw runtime_error("listen() failed");
  cout << "Server listening for edges on IP:" << inet_ntoa(main_server_for_edge.sin_addr) << " PORT: " << PORT_FOR_EDGE << "\n";
  int optval = 1;
  if (setsockopt(socket_edge_sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    throw runtime_error("setsockopt() failed");

  // Creating the socket that will listen for the EDNS0 server

  sockaddr_in main_server_for_dns;
  main_server_for_dns.sin_family = AF_INET;
  main_server_for_dns.sin_addr.s_addr = inet_addr(main_server_private_ip.c_str());
  main_server_for_dns.sin_port = htons(PORT_FOR_DNS);
  const int socket_dns_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_dns_sd == -1)
    throw runtime_error("socket() failed");
  if (bind(socket_dns_sd, (struct sockaddr *)&main_server_for_dns, sizeof(struct sockaddr_in)) == -1)
    throw runtime_error("bind() failed");
  if (listen(socket_dns_sd, 1) == -1)
    throw runtime_error("listen() failed");
  cout << "Server listening for the EDNS0 server on IP:" << inet_ntoa(main_server_for_dns.sin_addr) << " PORT: " << PORT_FOR_DNS << "\n";
  optval = 1;
  if (setsockopt(socket_dns_sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    throw runtime_error("setsockopt() failed");

  // Preforking for serving edge-servers
  for (int i = 1; i <= N_OF_EDGES; i++)
  {
    pid_t edge_server_pid = fork();
    switch (edge_server_pid)
    {
      case -1:
        throw runtime_error("fork() failed");
      case 0:
        // The process that will communicate with edge server #i to retrieve information about the load
        // the cached info and the CACHE MISSES that occur
          close(socket_dns_sd);
          handle_edge_server(i, socket_edge_sd); // infinite loop
        // TODO: If an edge-server crashes (function returns) we would have to handle this exception and
        // TODO: quickly remove the edge-server from the cached info until it reconnects
          close(socket_edge_sd);
          return 0;
    default:
        // (THIS PROCESS WILL "TAKE PLACE" OUTSIDE THE FOR LOOP (AFTER THE PROCESS POOL CREATION))
        // The parent process that will communicate with the EDNS0 server and will
        // send the info about all the edge-servers
          break;
    }
  }
  // After creating the process pool, it's time to handle the connection with the DNS server
  close(socket_edge_sd); // Closing the socket that will be used to transfer data to edge-servers
  handle_dns_info_transmission(socket_dns_sd); // infinite loop

  close(socket_dns_sd);

  return 0;
}