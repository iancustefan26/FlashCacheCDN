#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <curl/curl.h>
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


int main() {
  string resource_name;
  const string public_ip = get_public_ipv4();
  int request_number = 0;
  cout << "Hello, Client! Welcome to my CDN!\n";
  cout << "Your public IPv4 Address: " << public_ip << "\n\n";
  cout << "Request a resource or q to quit: ";
  cin >> resource_name;
  request_number++;
  if (resource_name == "q")
  {
    cout << "Quit.\n";
    return EXIT_SUCCESS;
  }
  while (true)
  {
    pid_t child_pid = fork();
    switch (child_pid)
    {
    case -1:
      throw runtime_error("fork failed");
    case 0:
      // Child process - STDOUT (Response from the DNS server)
        sleep(2);
        cout << "\nResponse from DNS server for request #" << request_number << ": " << child_pid << "\n";
      return EXIT_SUCCESS;
    default:
      // Parent process - STDIN ( Input from user, requests)
      cout << "Request a resource or q to quit: ";
      cin >> resource_name;
      request_number++;
      if (resource_name == "q")
      {
        cout << "Quit.\n";
        return EXIT_SUCCESS;
      }
    }
  }
}