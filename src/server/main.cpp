#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <usable.h>
using namespace std;


// TODO: Replace this with the actual IPv4 addresses
#define PORT 3333
#define EDGE_SERVER_PORT 2222
#define N_OF_EDGES 5


int main() {
  // Setting up the server
  string main_server_private_ip = get_private_ipv4();
  sockaddr_in main_server;
  main_server.sin_family = AF_INET;
  main_server.sin_addr.s_addr = inet_addr(main_server_private_ip.c_str());
  main_server.sin_port = htons(PORT);
  const int socket_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_sd == -1)
    throw runtime_error("socket() failed");
  if (bind(socket_sd, (struct sockaddr *)&main_server, sizeof(struct sockaddr_in)) == -1)
    throw runtime_error("bind() failed");
  if (listen(socket_sd, 5) == -1)
    throw runtime_error("listen() failed");
  cout << "Server listening on IP:" << main_server_private_ip << " PORT: " << PORT << "\n";
  // TODO: Preforking for serving edge-servers

  for (int i = 1; i <= N_OF_EDGES; i++)
  { // TODO: Provizor
    sockaddr_in edge;
    int egde_sd = accept(socket_sd, (sockaddr*)&edge, (socklen_t*)&edge);
    if (egde_sd == -1)
      throw runtime_error("accept() failed");
    cout << "Edge-server #" << i << " ---connected with IP: " << inet_ntoa(edge.sin_addr) << "\n";
  }
  return 0;
}