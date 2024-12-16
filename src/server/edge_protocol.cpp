#include "edge_protocol.h"
#include <fstream>

#define JSON_FILE "../src/server/assets/edge_servers_control.json"
// Here I will add functions for handling data transmission (config files, updates, cache TTL etc.) from main to edge,
// requests for a resource from an edge server in case of a cache MISS
// "These functions will be added to the Provizor infinite loop in the header"

using namespace std;

void append_to_info_file(const string& json_buffer)
{
    if (json_buffer.empty())
    {
        cerr << "Invalid JSON buffer\n";
        return;
    }

    ofstream file(JSON_FILE, ios::app);
    if (!file)
    {
        cerr << "Could not open file: " << JSON_FILE << "\n";
        return;
    }
    file << json_buffer << "\n";

    if (!file)
    {
        cerr << "Could not write to file: " << JSON_FILE << "\n";
    }
}