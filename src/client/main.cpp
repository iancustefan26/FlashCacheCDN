
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include "usable.h"
#include "protocol.h"

#define EDGE_SERVER_PORT 2222
#define EDNS_PORT 5053

using namespace std;

// TODO: rename the descriptors from _fd to _sd (socket_descriptor) for all the code

int main(int argc, char* argv[]) {
  if (argc != 3)
  {
    cerr << "Usage: " << argv[0] << " <IP Address of DNS server> <Edge server IP>" << endl; // TODO: edge server ip prozivor arg
    return EXIT_SUCCESS;
  }
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  connect_to_server(socket_fd, argv[1], EDNS_PORT); // argv[1] = EDNS0's IP address
  int request_number = 1;
  const string public_ip = get_public_ipv4();
  string resource;
  cout << "Hello, Client! Welcome to my CDN!\n";
  cout << "Your public IPv4 Address: " << public_ip << "\n\n";
  cout << request_number << ".Request a resource or q to quit: ";
  while (true)
  {
    cin >> resource;
    if (resource == "q")
    {
      close(socket_fd); // Closing the main socket (connection with the EDNS0 server)
      cout << "Quit.\n";
      return EXIT_SUCCESS;
    }
    if (resource == "clear")
    {
      clear_screen();
      continue;
    }

    // TODO: when i resolve the mapping edge-servers for the DNS it should retrieve a valid
    // TODO: edge-server IP, for now i get it using argv[2]
    // THE ORIGINAL ONE: const in_addr edge_server_ip = dns_get_request(socket_fd, resource, public_ip);
    const in_addr edge_server_ip = dns_get_request(socket_fd, resource, public_ip);
    // PROVIZOR:
    //const in_addr edge_server_ip = {inet_addr(argv[2])}; // argv[2] = edge_server_ip
    if (edge_server_ip.s_addr == 0)
    {
      // The EDNS0 Server's socket was closed gracefully
      cout << "Request failed: DNS servet got shut down, try again later";
      close(socket_fd);

      return EXIT_SUCCESS;
    }
    cout << "\nResponse from DNS server for request #" << request_number << " (" << resource << "): " << inet_ntoa(edge_server_ip)
        << "\n";
    // TODO: If a valid IP for an edge-server response is coming from the DNS server we will need to create a child process
    // TODO: that will interact with the edge-server
    pid_t child_pid = fork();
    switch (child_pid)
    {
    case -1:
      {
        throw runtime_error("fork failed");
      }
    case 0:
      {
        // The process that will interact with the edge server that dies when the requested resource is received
        close(socket_fd);
        int edge_server_sd = socket(AF_INET, SOCK_STREAM, 0);
        if (edge_server_sd == -1)
        {
          cerr << "Error opening edge server socket";
          return EXIT_FAILURE;
        }
        try
        {
          connect_to_server(edge_server_sd, inet_ntoa(edge_server_ip), EDGE_SERVER_PORT);
        }
        catch (const logic_error& e)
        {
          cerr << "On the way to edge server: " << e.what() << "\n";
        }
        // TODO: implement the communication protocol between CLIENT-EDGE_SERVER
        try
        {
          send_get_resource(edge_server_sd, resource);
          const string response = wait_for_response(edge_server_sd);
          cout << "Response from the edge-server with IP " << inet_ntoa(edge_server_ip) << ":\n" << response << "\n";
        }
        catch (const exception& e)
        {
          cerr << "Error when communicating with the edge-server: " << e.what() << "\n";
        }
        close(edge_server_sd);
        return EXIT_SUCCESS;
      }
    default:
      {
        // The parent that will continue to send the dns requests so the client can request more meanwhile
        // the edge-server is computing client's request
      }
    }
  }

}