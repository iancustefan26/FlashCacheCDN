#include "dns_server_protocol.h"

// Here I will add functions for handling data transmission (config files etc.)
// between the main server and the DNS server

void send_json_info(int socket_sd)
{
    const int fd = open(JSON_FILE, O_WRONLY);
    if (fd == -1)
    {
        cerr << "Error opening JSON file for writing\n";
        return;
    }
    if (flock(fd,  LOCK_SH| LOCK_NB) == -1)
    {
        cerr << "Error locking JSON file\n";
        close(fd);
        return;
    }
    ifstream file(JSON_FILE);
    if (!file.is_open()) {
        cerr << "Error opening file!\n";
        flock(fd, LOCK_UN);
        close(fd);
        return ;
    }

    ostringstream json_content;
    json_content << file.rdbuf();
    file.close();
    size_t packet_size = json_content.str().size() + sizeof(size_t);
    char* packet = new char[packet_size];
    memcpy(packet, &packet_size, sizeof(size_t));
    memcpy(packet + sizeof(size_t), json_content.str().c_str(), packet_size - sizeof(size_t));
    cout << json_content.str().c_str() << endl;
    const size_t bytes_sent = send(socket_sd, packet, packet_size, 0);
    if (bytes_sent == -1)
        cerr << "Sending response to main server failed";
    if (bytes_sent == 0)
        cerr << "Connection with the main server was closed gracefully";
    delete[] packet;

    flock(fd, LOCK_UN);
    close(fd);
}