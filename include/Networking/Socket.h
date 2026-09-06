#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <string>
using namespace std;

class Socket {
    public:
        Socket();
        ~Socket();
        void bind(int port);
        void listen(int backlog);
        Socket accept();
        string receive();
        void send(const string& data);

    private:
        int fileDescriptor;
};

#endif