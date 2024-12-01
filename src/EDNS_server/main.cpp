#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <curl/curl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 5053
using namespace std;

in_addr_t handle_request(const char* request, const size_t length)
{
  // Make a decision based on the resource requested ( first n - 4 bytes ), the geographical location of the client's IP
  // address (last 4 bytes), the load of the edge-servers, and the cached content of them
  cout << "Received packet size: " << length << "\n";
  char resource[length - 4];
  in_addr client_address;
  memcpy(resource, request, length - 4);
  cout << "Resource requested: " << request << "\n";
  memcpy(&client_address, request + length - 4, 4);
  cout << "Client's public IPv4 address: " << inet_ntoa(client_address) << "\n";
  return client_address.s_addr; // Provizor
  return 0; // Provizor
}

void respond_to_client(const int socket_fd)
{
  sockaddr_in client;
  socklen_t len = sizeof(client);
  const int client_fd = accept(socket_fd, (struct sockaddr *)&client, &len);
  if (client_fd == -1)
    throw runtime_error("accept() failed");
  size_t packet_size;
  if (recv(client_fd, &packet_size, sizeof(size_t), 0) == -1)
    throw runtime_error("recv() failed");
  char request[packet_size];
  if (recv(client_fd, request, packet_size, 0) == -1)
    throw runtime_error("recv() failed");
  cout << "--------------------------\n";
  const in_addr_t response = handle_request(request, packet_size);
  cout << "--------------------------\n";
  if (send(client_fd, &response, sizeof(in_addr_t), 0) == -1)
    throw runtime_error("Sending response to client failed");
  close(client_fd);
}

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