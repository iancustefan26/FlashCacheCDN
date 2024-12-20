#include "edge_protocol.h"
#include <fstream>
#include <nlohmann/json.hpp>



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

    int fd = open(JSON_FILE, O_WRONLY);
    if (fd == -1)
    {
        cerr << "Error opening JSON file for writing\n";
        return;
    }
    if (flock(fd, LOCK_SH | LOCK_NB) == -1)
    {
        cerr << "Error locking JSON file\n";
        close(fd);
        return;
    }

    json new_entry = json::parse(json_buffer);
    string new_ip = new_entry.begin().key();

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


    if (!existing_json.is_array()) {
        cerr << "Error: JSON file is not an array. Resetting to an empty array.\n";
        existing_json = json::array();
    }

    bool found = false;
    for (auto& entry : existing_json) {
        if (entry.contains(new_ip)) {
            entry = new_entry;
            found = true;
            break;
        }
    }

    if (!found) {
        existing_json.push_back(new_entry);
    }
    ofstream output_file(JSON_FILE);
    if (!output_file.is_open()) {
        cerr << "Could not open JSON file for writing: " << JSON_FILE << "\n";
        return;
    }
    output_file << existing_json.dump(4);
    output_file.close();

    flock(fd, LOCK_UN);
    close(fd);
}
