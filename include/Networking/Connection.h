#ifndef CONNECTION_H
#define CONNECTION_H

#include "Socket.h"

// A thin, request-scoped handle around one accepted Socket.
class Connection : public Socket {
    public:
        /*
            Takes ownership of an already-accepted Socket (composition --- the Connection owns this Socket for its lifetime)
        */
        Connection(Socket socket);

        /*
            Delegates the underlying Socket's receive() to pull the raw request bytes off the wire.
        */
        string read();

        /*
            Delegates to the underlying Socket's send() to write the raw response byte sback to the client.
        */
        void write(const string& data);

        /*
            Explocitly closes the connection ahead of destruction, e.g. after a response had been fully sent and keep-alive is not in use.
        */
        void close();

    private:
        Socket socket;
};

#endif