#include "usable.h"

#include <netdb.h>
#include <curl/curl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

size_t call_back(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
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
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
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

string get_private_ipv4() {
    ifaddrs* ifaddr;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        cerr << "getifaddrs() failed - returned loopback\n";
        return "0.0.0.0";
    }
    string private_ip;
    for (ifaddrs* it = ifaddr; it != nullptr; it = it->ifa_next) {
        if (it->ifa_addr == nullptr)
            continue;

        if (it->ifa_addr->sa_family == AF_INET) {
            const void* address = &((sockaddr_in*)it->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, address, host, NI_MAXHOST);
            string ip = host;
            if (ip.substr(0, 3) == "10." ||
                (ip.substr(0, 7) == "172.16" && ip[7] == '.') ||
                (ip.substr(0, 8) == "192.168" && ip[8] == '.'))
                {
                freeifaddrs(ifaddr);
                return ip;
                }
        }
    }
    cout << "Getting private IPv4 address failed - returned loopback\n";
    return "0.0.0.0";
}

