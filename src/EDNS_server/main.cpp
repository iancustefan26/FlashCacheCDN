#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netdb.h>
#include "requests.h"

#define PORT 5053
using namespace std;


int main() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    throw runtime_error("socket() failed");
  }
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);
  // Setting REUSEADDR option to avoid bind() errors when restarting the server in a short period of time (the used port is still cached)
  if (bind(socket_fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    throw runtime_error("bind() failed");
  constexpr int optval = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
    throw runtime_error("setsockopt() failed");
  cout << "EDNS0 server listening on IP: " << inet_ntoa(server.sin_addr) << " Port:" << PORT << "\n";
  if (listen(socket_fd, 5) == -1)
    throw runtime_error("listen() failed");
  while (true)
  {
    try
    {
      respond_to_client(socket_fd);
    }
    catch (exception& e)
    {
      std::cerr << "Serving client failed: " << e.what() << "\n";
    }
  }
  return 0;
}