#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <stdexcept>

#include <iostream>
#include <string>

// RAII wrapper around a POSIX file descriptor
class Socket {
    public:
        /*
            Constructs a socket via socket(AF_INET, SOCK_STREAM, 0) and stores the resulting file descriptor.
            Throw if creation fails.
        */
        Socket();
        explicit Socket(int fd);

        /*
            Closes the underlying file descriptor if it is still open.
            This is the class' core RAII guarantee --- a Socket going out of scope must never leak a file descriptor.
        */
        ~Socket();

        // Disable Copy operations (to prevent double-closing same file descriptor)
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        // Enable move operations
        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket&& other) noexcept;

        /*
            binds the socket to the given port on all interfaces.
            Sets SO_REUSEADDR before binding so the server can restart quickly without "address already in use" errors.
        */
        void bind(int port);

        /*
            Marks the socket as passive and ready to accept incoming connections, with the given backlog queue size.
        */
        void listen(int backlog);

        /*
            Blocks until a client connects, then returns a new Socket wrapping the accepted connection's file descriptor.
            The listening socket itself keeps listening.
        */
        Socket accept();

        // Closes the descriptor and makes this socket empty.
        void close();

        /*
            reads available bytes to the socket.
            Loops on recv() to handle writes, since a single call is not guaranteed to send the whole buffer.
        */
        std::string receive();

        /*
            Writes the given bytes to the socket.
            Loops on send() to handle partial writes, since single call is not guaranteed to send the whole buffer.
        */
        void send(const std::string& data);

    private:
        int fileDescriptor = -1;
};

#endif