#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <netdb.h>
#include <strings.h>
#include <cstring>
#include "cache.h"
#include "usable.h"

#include "requests.h"

#define PORT 5053
using namespace std;

// TODO: renaming the variables
// TODO: think about he improvements that can be done against DDoS attacks

int main() {
  // Creating the object that initializes the cached info about edge-servers (load, content, geolocation)
  Cache* mapped_cached_content = new Cache();
  // Creating a thread that will handle the cached content on edge-servers and will provide
  // info about their available info and load, it is accessible from other thread because
  // they share the same HEAP
  thread cache_thread(&Cache::update_mapping_entry_point, &mapped_cached_content);
  cache_thread.detach();
  // Creating the data structures that will handle multiplexing
  fd_set read_fds;
  fd_set active_fds;
  timeval tv;
  int number_of_fds = 2;
  // Creating the actual main socket
  string dns_ip = get_private_ipv4();
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    throw runtime_error("socket() failed");
  }
  sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(dns_ip.c_str()); // TODO: real IP
  server.sin_port = htons(PORT);
  // Setting REUSEADDR option to avoid bind() errors when restarting the server in a short period of time (the used port is still cached)
  if (bind(socket_fd, (sockaddr *)&server, sizeof(server)) == -1)
    throw runtime_error("bind() failed");
  constexpr int optval = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    throw runtime_error("setsockopt() failed");
  cout << "EDNS0 server listening on IP: " << inet_ntoa(server.sin_addr) << " Port:" << PORT << "\n";
  if (listen(socket_fd, 5) == -1)
    throw runtime_error("listen() failed");
  FD_ZERO (&active_fds);		// Initializing the set of descriptors
  FD_SET (socket_fd, &active_fds);
  number_of_fds = socket_fd;
  while (true)
  {
    // I have done a little bit of research with this timeval using a syscall trace
    // and for whatever reason if I initialize it outside the loop
    // it sets to {0, 0} after the first loop and it overloads the CPU by doing
    tv = {1, 0};
    memcpy(&read_fds, &active_fds, sizeof(read_fds)); // Updating the set of descriptors
    if (select (number_of_fds+1, &read_fds, nullptr, nullptr, &tv) < 0)
    {
      cerr << "select failed" << "\n";
      return EXIT_FAILURE;
    }
    if (FD_ISSET (socket_fd, &read_fds))
    {
      // Creating client socket
      sockaddr_in client;
      socklen_t len = sizeof(client);
      const int client_fd = accept(socket_fd, (sockaddr *)&client, &len);
      if (client_fd == -1)
        throw runtime_error("accept() failed");
      FD_SET(client_fd, &active_fds); // Adding the new client to the set
      number_of_fds = client_fd; // Updating the size of the set
    }
    try
    {
      for (int client_fds = 4; client_fds <= number_of_fds; client_fds++) // 0,1,2 - I/O, 3 - main socket, 4.. - clients
        if (FD_ISSET (client_fds, &read_fds)) // If there is actually any descriptor waiting for action (from the set of "active" descriptors)
          respond_to_client(&active_fds, client_fds, mapped_cached_content);
    }
    catch (exception& e)
    {
      cerr << "Serving client failed: " << e.what() << "\n";
    }
  }
}