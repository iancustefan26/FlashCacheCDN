#include "main_server_protocol.h"

#include <iostream>
#include <unistd.h>
#include <utils.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

void treat_main_server(int main_server_sd, string edge_server_private_ip)
{
    // Communication with the main server
    // Retrieving information about the updates, TTL for the stored cache resources
    // And also requesting a resource in case of a cache MISS, and giving info about his daily request incomes
    // so the server will make a logic and send a config file with more accurate variables like TTL of the resources
    // and the number of the threads in the pool of each edge-server in order to lower the cost for the architecture
    // but still bring the same efficiency
    string machine_name = get_machine_name();
    vector<string> resources;
    resources.reserve(2048);
    while (true)
    {
        // Provizor
        sleep(10);
        cout << "PID: " << getpid() << " communicating with the main server\n";
        float cpu_load = get_cpu_load();
        cout << "CPU Load: " << cpu_load << "%" << endl;
        pair<float, float> memory_load = get_memory_usage();
        cout << "Memory Used: " << memory_load.first << " MB" << endl;
        cout << "Memory Available: " << memory_load.second << " MB" << endl;
        get_available_resources(resources);
      json server_details = {
        {"name", machine_name},
        {"load", {
              {"cpu_load", cpu_load},
              {"memory_load", {
                  {"used", memory_load.first},
                  {"available", memory_load.second}
              }}
        }},
        {"resources", resources}
      };
      json system_stats = {
        {edge_server_private_ip, server_details}
      };
      string json_output = system_stats.dump(4);
      cout << "Serialized JSON info about the machine:" << "\n";
      cout << json_output << "\n";
    }
}
