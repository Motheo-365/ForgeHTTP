#ifndef CONNECTION_H
#define CONNECTION_H

#include "Socket.h"

// A thin, request-scoped handle around one accepted Socket.
class Connection {
    public:
        /*
            Takes ownership of an already-accepted Socket
        */
        explicit Connection(Socket&& socket) noexcept;

        // Disable copying (since Socket cannot be copied)
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        // Enable moving
        Connection(Connection&&) noexcept = default;
        Connection& operator=(Connection&&) noexcept = default;

        /*
            Delegates the underlying Socket's receive()
        */
        std::string read();

        /*
            Delegates to the underlying Socket's send()
        */
        void write(const std::string& data);

        /*
            Closes connection by resetting the underlying socket
        */
        void close();

    private:
        Socket socket; // Composition
};

#endif