#ifndef CONNECTION_H
#define CONNECTION_H

#include "Socket.h"

class Connection : public Socket {
    public:
        Connection( Socket socket);
        string read();
        void write(const string& data);
        void close();

    private:
        Socket socket;
};

#endif