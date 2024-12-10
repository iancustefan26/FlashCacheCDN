#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

// Future features
enum RequestType
{
    EXEC, // For now only this one is available
    SEND
};

void treat_clients(int socket_fd); // Will develop a concurrent manner of serving clients

#endif
