#include "edge_protocol.h"
#include <fstream>
#include <nlohmann/json.hpp>

#define JSON_FILE "../src/server/assets/edge_servers_control.json"

using json = nlohmann::json;
// Here I will add functions for handling data transmission (config files, updates, cache TTL etc.) from main to edge,
// requests for a resource from an edge server in case of a cache MISS
// "These functions will be added to the Provizor infinite loop in the header"

using namespace std;

void append_to_info_file(const std::string& json_buffer)
{
    if (json_buffer.empty()) {
        cerr << "Invalid JSON buffer\n";
        return;
    }
    json entry = json::parse(json_buffer);

    ifstream input_file(JSON_FILE);
    json existing_json;

    if (input_file.is_open()) {
        try {
            input_file >> existing_json;
        } catch (const nlohmann::json::parse_error& e) {
            cerr << "Error parsing JSON: " << e.what() << "\n";
            existing_json = json::array();
        }
        input_file.close();
    } else {
        existing_json = nlohmann::json::array();
    }

    if (!existing_json.is_array()) {
        cerr << "Error: JSON file is not an array. Resetting to an empty array.\n";
        existing_json = json::array();
    }

    existing_json.push_back(entry);

    ofstream output_file(JSON_FILE);
    if (!output_file.is_open()) {
        cerr << "Could not open JSON file for writing: " << JSON_FILE << "\n";
        return;
    }
    output_file << existing_json.dump(4);
    output_file.close();
}
