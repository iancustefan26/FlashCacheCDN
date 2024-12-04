#include "client_protocol.h"
#include <iostream>
#include <unistd.h>
using namespace std;
void treat_clients(int socket_fd)
{
    while (true)
    {
        cout << "PID: " << getpid() << " Treating clients..." << "\n";
        sleep(6);
    }
}
