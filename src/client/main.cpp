
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include "usable.h"
#include "concurrency.h"

using namespace std;


int main(int argc, char* argv[]) {
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " <IP Address of DNS server>" << endl;
    return EXIT_SUCCESS;
  }
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  connect_to_edns0_server(socket_fd, argv[1]); // argv[1] = EDNS0's IP address
  int request_number = 1;
  const string public_ip = get_public_ipv4();
  string resource;
  cout << "Hello, Client! Welcome to my CDN!\n";
  cout << "Your public IPv4 Address: " << public_ip << "\n\n";
  while (true)
  {
    cout << request_number << ".Request a resource or q to quit: ";
    cin >> resource;
    if (resource == "q")
    {
      close(socket_fd); // Closing the main socket (connection with the EDNS0 server)
      cout << "Quit.\n";
      return EXIT_SUCCESS;
    }
    const in_addr edge_server_ip = dns_get_request(socket_fd, resource, public_ip);
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
  }
  while (true)
  {
    // Handling concurrent I/O operations
    // The main process is handling INPUT operations and the child communicates with the EDNS0 server and
    // establishes a connection with the received edge server trough TCP
    pid_t child_pid = fork();
    switch (child_pid)
    {
    case -1:
      throw runtime_error("fork failed");
    case 0:
      {
        // Child process - STDOUT (Response from the DNS server)
        // Sending requested resource to the EDNS0 server trough socket and waiting for a convenient CDN IP address
        try
        {
          child_work_send_edns0_request(resource, argv[1], request_number, public_ip);
        }
        catch (exception& e)
        {
          std::cerr << "Child process: " << e.what() << "\n";
          return EXIT_FAILURE;
        }
      }
    default:
      {
        // Parent process - STDIN ( Input from user, requests)
        int exit_code;
        wait(&exit_code);
        if (exit_code != EXIT_SUCCESS) {
          cerr << "Parent process: Connection failed! Exit Code: " << exit_code << "\n";
          return EXIT_FAILURE;
        }
        try
        {
          parent_work(request_number, resource);
        }
        catch (exception& e)
        {
          cerr << e.what() << "\n";
          return EXIT_FAILURE;
        }
        break;
      }
    }
  }
}