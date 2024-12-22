#include "cache.h"


float Memory::get_average_load() const
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
    return 0.65f * load.cpu_load
    + 0.35f * load.memory_load.get_average_load()
    ;
}

float Edge_server::get_geo_distance_estimation_index(const in_addr_t client_ip)
{
    // Estimating the geographical distance between the client and the edge-server, resulting
    // in an index reported to 1.0f
    // Split IPs into their four bytes
    uint8_t server_bytes[4];
    uint8_t client_bytes[4];
    in_addr_t server_ip = get_ip();

    for (int i = 0; i < 3; ++i) {
        server_bytes[i] = (server_ip >> (24 - i * 8)) & 0xFF; // Extracting the first 3 bytes from the IP
        client_bytes[i] = (client_ip >> (24 - i * 8)) & 0xFF; // 192       168      1      101
    }

    float index = 0.0f;

    // Calculate contribution for each byte
    // (most LAN's have the network mask 255.255.255.0 so the last byte doesn't play an important role
    // to the geolocation)
    for (int i = 0; i < 3; ++i)
    {
        if (server_bytes[i] == client_bytes[i])
        {
            index += 0.33f; // Perfect match of the byte
        }
        else
        {
            float contribution = 0.33f - (abs(1.0f * static_cast<int>(server_bytes[i]) - 1.0f * static_cast<int>(client_bytes[i])) / 255.0f * 0.33f);
            index += contribution;
        }
    }
    return index;
}


float Edge_server::compute_decide_grade(in_addr_t client_ip, const char* resource)
{
    // A cache MISS
    // Returns 0 so this edge server will not be picked
    if (resources.contains(resource) == false)
        return 0.0f;

    return get_geo_distance_estimation_index(client_ip) * 0.67f + get_average_load() * 0.33f + 1.0f;
}

Cache::Cache(const char* main_server_ip)
{
    this -> main_server_ip = main_server_ip;
    number_of_edge_servers = 10;  // Replace with the maximum number of edge-servers that can be available
    edge_servers.reserve(number_of_edge_servers);

    // Implement the deserialization of the JSON file and initialize the members
    this->initialize();
}

void Cache::initialize()
{
    // There I parse the JSON received from the main server full of info about the edge servers
    // and I will deserialize it into C++ objects and add them to the vector
    // Example:
    // Read the JSON file
    ifstream json_file(JSON_FILE);
    if (!json_file.is_open())
    {
        cerr << "Could not open JSON file: " << JSON_FILE << "\n";
        return;
    }

    // Parse the JSON file
    json parsed_json;
    try
    {
        json_file >> parsed_json;
    }
    catch (const json::exception &e)
    {
        cerr << "Error parsing JSON: " << e.what() << "\n";
        return;
    }

    edge_servers.clear();
    for (const auto &server_entry : parsed_json)
    {
        for (const auto &server_ip : server_entry.items())
        {
            try
            {
                const auto &server_info = server_ip.value();

                // Extract server data
                const string& ip = server_ip.key();
                string name = server_info.at("name").get<string>();
                float cpu_load = server_info.at("load").at("cpu_load").get<float>();
                float memory_available = server_info.at("load").at("memory_load").at("available").get<float>();
                float memory_used = server_info.at("load").at("memory_load").at("used").get<float>();
                unordered_set<string> resources(
                    server_info.at("resources").begin(),
                    server_info.at("resources").end());

                // Create Edge_server object and add it to edge_servers
                edge_servers.emplace_back(
                    Edge_server({cpu_load, {memory_available, memory_used}}, inet_addr(ip.c_str()), name, resources));
            }
            catch (const json::exception &e)
            {
                cerr << "Error processing server entry: " << e.what() << "\n";
            }
        }
    }

    cout << "Cache initialized with " << edge_servers.size() << " edge servers.\n";

}

void Cache::update_mapping()
{
    // TODO: and when he receives, update the stored data with the latest JSON
    int main_server_sd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in main_server;
    main_server.sin_family = AF_INET;
    main_server.sin_addr.s_addr = inet_addr(main_server_ip.c_str());
    main_server.sin_port = htons(MAIN_SERVER_PORT);
    if (connect(main_server_sd, (sockaddr *)&main_server, sizeof(sockaddr_in)) == -1)
        throw runtime_error("main_server_connect failed");
    cout << "Connected to the main server on PORT: " << MAIN_SERVER_PORT << "\n";
    while (true)
    {
        sleep(5);
        cout << "Updating cache info from JSON file...\n";
        size_t packet_size;
        size_t bytes_received = recv(main_server_sd, &packet_size, sizeof(size_t), 0);
        if (bytes_received == 0 || bytes_received == -1)
        {
            cerr << "Invalid JSON received\n";
            continue;
        }
        char* json_buffer = new char[packet_size];
        bytes_received = recv(main_server_sd, json_buffer, packet_size - sizeof(size_t), 0);
        if (bytes_received == 0 || bytes_received == -1)
        {
            cerr << "Invalid JSON received\n";
            continue;
        }
        json_buffer[bytes_received] = '\0';
        cout << json_buffer << "\n";
        ofstream output_file(JSON_FILE);
        if (!output_file.is_open()) {
            cerr << "Could not open JSON file for writing: " << JSON_FILE << "\n";
            return;
        }
        output_file << json_buffer;
        output_file.close();

        // Update the Cache data structure with the new received info
        this->initialize();
        delete[] json_buffer;
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
    // Second, the edge-server should be near the client (geolocation position)
    // Third, the load should not be that high
    // So, we will need to calculate an index based on (distance, load)
    // Taking into account that the distance is more important than the actual load
    // in order for latency to be the lowest possible
    // We will compute the index like this : index = distance * 0,67 + load * 0,33
    // Also, I am using floats even if I know that computations on floats are costly, but at a high scale
    // a 0,1 plays a vital role
    // And I think a double precision data structure will not be worth

    Edge_server most_convenient_edge = edge_servers[0];
    float most_convenient_index = edge_servers[0].compute_decide_grade(client_ip, resource);
    for (auto& edge: edge_servers)
    {
        float decide_index = edge.compute_decide_grade(client_ip, resource);
        cout << "Edge: " << inet_ntoa({edge.get_ip()}) << " -- decide index: " << decide_index;
        if (decide_index == 0.0f)
        {
            cout << " (Resource not cached)\n";
            continue;
        }
        cout << "\n";

        if (decide_index < most_convenient_index )
            most_convenient_edge = edge, most_convenient_index = decide_index;
    }
    cout << "Returned to the client egde-server: IP: " << inet_ntoa({most_convenient_edge.get_ip()})
         << "\nwith decide index: " << most_convenient_index << "\n";

    return most_convenient_edge.get_ip();
}












