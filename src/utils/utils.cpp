#include "utils.h"

#include <cstring>
#include <netdb.h>
#include <curl/curl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

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

float get_cpu_load()
{
    ifstream load_file("/proc/stat");
    string line;
    getline(load_file, line);
    load_file.close();

    istringstream parser(line);
    string cpu;
    size_t user, nice, system, idle, iowait, interrupts, soft_interrupts, steal;

    parser >> cpu >> user >> nice >> system >> idle >> iowait >> interrupts >> soft_interrupts >> steal;

    static size_t previous_total = 0, previous_idle = 0; // The load average is based on the previous load average
    size_t total = user + nice + system + idle + iowait + interrupts + soft_interrupts + steal;
    size_t idle_time = idle + iowait;

    // Calculate delta
    size_t total_diff = total - previous_total;
    size_t idle_diff = idle_time- previous_idle;

    previous_total = total;
    previous_idle = idle_time;

    return (1.0f - static_cast<float>(idle_diff) / total_diff) * 100.0f;
}

pair<float, float> get_memory_usage()
{
    ifstream memory_file("/proc/meminfo");
    string line;
    long total = 0, available = 0;

    while (getline(memory_file, line)) {
        istringstream iss(line);
        string key;
        long value;
        string unit;

        iss >> key >> value >> unit;
        if (key == "MemTotal:") {
            total = value;
        } else if (key == "MemAvailable:") {
            available = value;
        }
    }

    memory_file.close();
    return pair<float, float>((total - available) / 1024.0f, available / 1024.0f);
}

void get_available_resources(vector<string> &resources)
{
    // TODO: here i have to retrieve the info based on the stored folders
    resources.clear();
    char path[256]; // this will be the build directory
    getcwd(path, sizeof(path));
    strcat(path, "/../src/edge_server/available_resources");
    cout << "Available resources path: " << path << "\n";
    for (const auto& entry : filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) { // Check if it's a file
            string filename = entry.path().filename().string(); // Get the filename with extension
            size_t dotPos = filename.find_last_of('.'); // Find the last dot position
            if (dotPos != string::npos) {
                filename = filename.substr(0, dotPos); // Remove the extension
            }
            resources.push_back("./" + filename);
        }
    }
}


string get_machine_name()
{
    const char* username = getlogin();
    if (username) {
        return username;
    }
    return "unknown";
}

