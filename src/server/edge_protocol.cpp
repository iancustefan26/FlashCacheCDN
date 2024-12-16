#include "edge_protocol.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define JSON_FILE "../src/server/assets/edge_servers_control.json"

using json = nlohmann::json;
// Here I will add functions for handling data transmission (config files, updates, cache TTL etc.) from main to edge,
// requests for a resource from an edge server in case of a cache MISS
// "These functions will be added to the Provizor infinite loop in the header"

using namespace std;

void append_to_info_file(const string& json_buffer)
{
    if (json_buffer.empty()) {
        cerr << "Invalid JSON buffer\n";
        return;
    }

    // Parse the incoming JSON buffer
    json new_entry = json::parse(json_buffer);
    string new_ip = new_entry.begin().key(); // Extract the IP address (key)

    // Read the existing JSON file
    ifstream input_file(JSON_FILE);
    json existing_json;

    if (input_file.is_open()) {
        try {
            input_file >> existing_json;
        } catch (const json::parse_error& e) {
            cerr << "Error parsing JSON: " << e.what() << "\n";
            existing_json = json::array();
        }
        input_file.close();
    } else {
        existing_json = json::array();
    }

    // Ensure the top-level JSON is an array
    if (!existing_json.is_array()) {
        cerr << "Error: JSON file is not an array. Resetting to an empty array.\n";
        existing_json = json::array();
    }

    // Check if the IP already exists and modify it if found
    bool found = false;
    for (auto& entry : existing_json) {
        if (entry.contains(new_ip)) { // Check if this entry matches the new IP
            entry = new_entry; // Update the entry
            found = true;
            break;
        }
    }

    // If the IP does not exist, append the new entry
    if (!found) {
        existing_json.push_back(new_entry);
    }

    // Write the updated JSON back to the file
    ofstream output_file(JSON_FILE);
    if (!output_file.is_open()) {
        cerr << "Could not open JSON file for writing: " << JSON_FILE << "\n";
        return;
    }
    output_file << existing_json.dump(4); // Pretty-print with 4 spaces
    output_file.close();
}
