#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netdb.h>
#include <strings.h>

#include "requests.h"

#define PORT 5053
using namespace std;


int main() {
  fd_set readfds;
  fd_set actfds;
  timeval tv = {0, 2};
  int nfds = 2;
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    throw runtime_error("socket() failed");
  }
  sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
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
  FD_ZERO (&actfds);		// Initializing the set of descriptors
  FD_SET (socket_fd, &actfds);
  nfds = socket_fd;
  while (true)
  {
    bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds)); // TODO: Not C like updating the set of descriptors
    if (select (nfds+1, &readfds, nullptr, nullptr, &tv) < 0)
    {
      cerr << "select failed" << "\n";
      return EXIT_FAILURE;
    }
    if (FD_ISSET (socket_fd, &readfds))
    {
      // Creating client socket
      sockaddr_in client;
      socklen_t len = sizeof(client);
      const int client_fd = accept(socket_fd, (sockaddr *)&client, &len);
      if (client_fd == -1)
        throw runtime_error("accept() failed");
      FD_SET(client_fd, &actfds); // Adding the new client to the set
      nfds = client_fd; // Updating the size of the set
    }
    try
    {
      for (int c = 4; c <= nfds; c++) // 0,1,2 - I/O, 3 - main socket, 4.. - clients
        if (FD_ISSET (c, &readfds)) // If there is actually any descriptor waiting for action (from the set of "active" descriptors)
          respond_to_client(&actfds, c);
    }
    catch (exception& e)
    {
      cerr << "Serving client failed: " << e.what() << "\n";
    }
  }
  return 0;
}