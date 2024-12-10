#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <unordered_set>
#include <netinet/in.h>
#include <nlohmann/json.hpp>

#define MAIN_SERVER_PORT 6666

using namespace std;
using namespace nlohmann;

struct CPU
{
    float last_1_minute;
    float last_5_minutes;
    float last_15_minutes;

    float get_average_load() const;
};

struct Memory
{
    float size_in_MB;
    float used_in_MB;

    float get_average_load() const;
};

struct Disk
{
    float size_in_MB;
    float used_in_MB;

    float get_average_load() const;
};

struct Load
{
    CPU cpu_load;
    Memory memory_load;
    Disk disk_load;
};

class Edge_server
{
    Load load;
    in_addr_t ip_address;
    string name;
    unordered_set<string> resources; // O(1) lookups by using unordered_set
    // Compute the average load of the machine
    float get_average_load() const;

    // Estimating the geographical distance between the client and the edge-server
    float get_geo_distance_estimation_index(in_addr_t client_ip);

public:
    Edge_server(const Load& load, in_addr_t ip_address, const string& name, const unordered_set<string>& resources); // Constructor

    // Computing the final grade of the edge-server based on cached content, load and geolocation
    float compute_decide_grade(in_addr_t client_ip, const char* resource);
    string get_name() const;
    in_addr_t get_ip() const;
};

class Cache
{
    string main_server_ip;
    int number_of_edge_servers;
    vector<Edge_server> edge_servers;
    void initialize(); // Initializing the cached data from the JSON config file
public:
    Cache(const char* main_server_ip); // Will call initialize after allocating memory
    void update_mapping();

    // It is needed in order for the thread to access the actual object's instance function update_mapping()
    static void* update_mapping_entry_point(void* actual_object);

    // Decide which edge-server will pick based on the implemented algorithms
    in_addr_t decide(const char* resource, in_addr_t client_ip);
};

#endif
