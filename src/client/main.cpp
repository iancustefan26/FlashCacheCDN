
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netdb.h>
#include "usable.h"
#include "concurrency.h"

using namespace std;


int main(int argc, char* argv[]) {
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " <IP Address of DNS server>" << endl;
    return EXIT_SUCCESS;
  }
  string resource;
  const string public_ip = get_public_ipv4();
  int request_number = 1;
  cout << "Hello, Client! Welcome to my CDN!\n";
  cout << "Your public IPv4 Address: " << public_ip << "\n\n";
  cout << "1.Request a resource or q to quit: ";
  cin >> resource;
  if (resource == "q")
  {
    cout << "Quit.\n";
    return EXIT_SUCCESS;
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
          std::cerr << e.what() << "\n";
          return EXIT_FAILURE;
        }
      }
    default:
      {
        // Parent process - STDIN ( Input from user, requests)
        try
        {
          parent_work(request_number, resource);
        }
        catch (exception& e)
        {
          std::cerr << e.what() << "\n";
          return EXIT_FAILURE;
        }
        break;
      }
    }
  }
}