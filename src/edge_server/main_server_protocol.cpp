#include "main_server_protocol.h"
#include <iostream>
#include <unistd.h>

using namespace std;

void treat_main_server(int main_server_sd)
{
    // TODO: the actual communication
    while (true)
    {
        cout << "Child PID: " << getpid() << " (PPID=" << getppid() << ") Communicating with the main server...\n";
        sleep(5);
    }
}
