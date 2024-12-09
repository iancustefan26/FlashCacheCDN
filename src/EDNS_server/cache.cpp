#include "cache.h"

#include <random>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>

float CPU::get_average_load() const
{
    return this->last_1_minute + this->last_5_minutes + this->last_15_minutes / 3.0f * 100.0f;
}

float Memory::get_average_load() const
{
    return this->used_in_MB * 100.0f / this->size_in_MB;
}

float Disk::get_average_load() const
{
    return this->used_in_MB * 100.0f / this->size_in_MB;
}

Edge_server::Edge_server(const Load& load, const in_addr_t ip_address, const string& name, const unordered_set<string>& resources)
    : load(load), ip_address(ip_address), name(name), resources(resources) {}

string Edge_server::get_name() const
{
    return this->name;
}

in_addr_t Edge_server::get_ip() const
{
    return ip_address;
}


float Edge_server::get_average_load() const
{
    // The CPU load is the most important, then the RAM and then the Disk
    return 0.55f * load.cpu_load.get_average_load()
    + 0.35f * load.memory_load.get_average_load()
    + 0.1f * load.disk_load.get_average_load()
    / 3.0f;
}

float Edge_server::get_geo_distance_estimation_index(in_addr_t client_ip)
{
    // Estimating the geographical distance between the client and the edge-server, resulting
    // in an index reported to 100%
    // TODO: implementing the actual logic

    mt19937 generator(std::random_device{}());
    uniform_real_distribution<float> distribution(0.0f, 1.0f);

    const float random_float = distribution(generator);
    return random_float;
}


float Edge_server::compute_decide_grade(in_addr_t client_ip, const char* resource)
{
    // A cache MISS but still the most convenient one
    if (resources.contains(resource) == false)
        return get_geo_distance_estimation_index(client_ip) * 0.67f + get_average_load() * 0.33f + 0.0f / 3.0f;

    return get_geo_distance_estimation_index(client_ip) * 0.67f + get_average_load() * 0.34f + 1.0f / 3.0f;
}

Cache::Cache(const char* main_server_ip)
{
    this -> main_server_ip = main_server_ip;
    number_of_edge_servers = 5; // TODO: receive the actual number of edge servers from the main server through a socket
    edge_servers.reserve(number_of_edge_servers);
    // Implement the deserialization of the JSON file and intialize the members
    this->initialize();
}

void Cache::initialize()
{
    // There I make the JSON received from the main server full of info about the edge server
    // and I will deserialize it into C++ objects and add them to the vector
    edge_servers.push_back
    (
        Edge_server({
            {0.15, 0.37, 0.40},
            {1024, 240},
            {5096, 1055}
        },
        inet_addr("10.176.86.169"),
        "edge-server-1",
        {"./test", "./test_5_seconds"}
        )
    );
}

void Cache::update_mapping()
{
 // TODO: Communicates with the main server and receives the newest updates through the JSON file
    // TODO: and when he receives, update the stored data with the latest JSON
    int main_server_sd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in main_server;
    main_server.sin_family = AF_INET;
    main_server.sin_addr.s_addr = inet_addr(main_server_ip.c_str());
    main_server.sin_port = htons(MAIN_SERVER_PORT);
    if (connect(main_server_sd, (sockaddr *)&main_server, sizeof(sockaddr_in)) == -1)
        throw runtime_error("main_server_connect failed");
    cout << "Connected to the main server on PORT: " << MAIN_SERVER_PORT << "\n";
    for (int i = 0; i < 100; i++)
    {
        sleep(5);
        cout << "Updating cache info from JSON file...\n";
    }
}

void* Cache::update_mapping_entry_point(void* actual_object)
{
    Cache* actual_cache = static_cast<Cache*>(actual_object);
    actual_cache->update_mapping();

    return nullptr;
}

in_addr_t Cache::decide(const char* resource, in_addr_t client_ip)
{
    // Based on client's IP address, resource requested and the load of the machines, make a decision
    // The most important thing is first that the machine must have the resource cached
    // Second, the edge-server should be near the client
    // Third, the load should not be so high
    // So, we will need to calculate an average based on (distance, load)
    // Taking into acount that the distance is more important than the actual load
    // We will compute the average like this : average = (distance * 0,67 + load * 0,33 ) / 2;

    // IP - decide_index
    Edge_server most_convenient_edge = edge_servers[0];
    float most_convenient_index = edge_servers[0].compute_decide_grade(client_ip, resource);
    for (auto& edge: edge_servers)
    {
        float decide_index = edge.compute_decide_grade(client_ip, resource);
        if (decide_index > most_convenient_index)
            most_convenient_edge = edge, most_convenient_index = decide_index;
    }
    cout << "Returned to EDNS0 server: IP: " << inet_ntoa({most_convenient_edge.get_ip()})
         << " with decide index: " << most_convenient_index << "\n";

    return most_convenient_edge.get_ip();
}












