#include "main_server_protocol.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void treat_main_server(int main_server_sd)
{
    // Communication with the main server
    // Retrieving information about the updates, TTL for the stored cache resources
    // And also requesting a resource in case of a cache MISS, and giving info about his daily request incomes
    // so the server will make a logic and send a config file with more accurate variables like TTL of the resources
    // and the number of the threads in the pool of each edge-server in order to lower the cost for the architecture
    // but still bring the same efficiency
    while (true)
    {
        // Provizor
        sleep(10);
        cout << "PID: " << getpid() << " communicating with the main server\n";
    }
}
