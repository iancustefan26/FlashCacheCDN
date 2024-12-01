#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <curl/curl.h>
#include <netdb.h>
#include <cstring>

#define EDNS_PORT 5053

using namespace std;


// Usable
size_t call_back(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

void clear_screen()
{
  system("clear");
}

string get_public_ipv4()
{
  CURL* curl = curl_easy_init();
  std::string public_ip;
  curl_global_init(CURL_GLOBAL_DEFAULT);

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://ifconfig.me");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_back);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &public_ip);

    const CURLcode response = curl_easy_perform(curl); // Curl request

    if (response != CURLE_OK)
      throw runtime_error("curl_easy_perform() failed");
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return public_ip;
}

void parent_work(int &request_number, string& resource)
{
  request_number++;
  cout << request_number << ".Request a resource or q to quit: ";
  cin >> resource;
  if (resource == "q")
  {
    cout << "Quit.\n";
    exit(EXIT_SUCCESS);
  }
  if (resource == "clear")
    clear_screen();
}

void child_work_send_edns0_request(const string& resource, const char* dns_server_ip, const int request_number, const string& public_ip)
{
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
    throw runtime_error("socket for EDNS0 failed");
  sockaddr_in dns_server;
  dns_server.sin_family = AF_INET;
  dns_server.sin_port = htons(EDNS_PORT);
  dns_server.sin_addr.s_addr = inet_addr(dns_server_ip); // argv[1] = EDNS0's IP address
  if (connect(socket_fd, (sockaddr*)&dns_server, sizeof(dns_server)) < 0)
    throw runtime_error("connect to EDNS0 failed");

  // Creating the packet
  const size_t dns_packet_length = resource.length() + 1 + sizeof(in_addr_t); // length header + sender's public IPv4 (sizeof(uint32_t)) + request + NULL
  const auto dns_packet = new unsigned char[dns_packet_length + sizeof(size_t)];
  const auto pub_ipv4n_addr = inet_addr(public_ip.c_str());
  memcpy(dns_packet, &dns_packet_length, sizeof(size_t));
  memcpy(dns_packet + sizeof(size_t), resource.c_str() , resource.length() + 1);
  memcpy(dns_packet + sizeof(size_t) + resource.length() + 1, &pub_ipv4n_addr, sizeof(in_addr_t));

  // Sending the packet to the EDNS0 server
  if (send(socket_fd, dns_packet, dns_packet_length + sizeof(size_t), 0) == -1)
  {
    close(socket_fd);
    delete[] dns_packet; // Freeing the allocated memory
    throw runtime_error("Packet sending to EDNS0 server failed");
  }
  delete[] dns_packet; // Freeing the allocated memory
  in_addr dns_response = {0};
  // Receiving the response from the EDNS0 server
  if (recv(socket_fd, &dns_response, sizeof(in_addr_t), 0) == -1)
    throw runtime_error("Packet receiving from EDNS0 server failed");
  close(socket_fd);
  const string edge_server_ip = inet_ntoa(dns_response);
  cout << "\nResponse from DNS server for request #" << request_number << " (" << resource << "): " << edge_server_ip << "\n";
  exit(EXIT_SUCCESS);
}

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