#include "Networking/Socket.h"

#include <netinet/in.h>
#include <unistd.h>

Socket::Socket() {
    fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (fileDescriptor == -1) {
        throw runtime_error("Failed to create socket");
    }
}

Socket::Socket(int fd) : fileDescriptor(fd) {};

// Move Constructor
Socket::Socket(Socket&& other) noexcept : fileDescriptor(other.fileDescriptor) {
    other.fileDescriptor = -1;
}

// Move Assignment Operator
Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        // Close current descriptor if open
        if (fileDescriptor != -1) {
            ::close(fileDescriptor);
        }

        // Steal resources from other
        fileDescriptor = other.fileDescriptor;
        other.fileDescriptor = -1;
    }
    return *this;
}

void Socket::bind(int port) {
    int opt = 1;
    struct sockaddr_in address;
    setsockopt(fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind
    if (::bind(fileDescriptor, (struct sockaddr *)&address, sizeof(address)) < 0) {
        throw runtime_error("bind failed");
    }
}

void Socket::listen(int backlog) {
    if (::listen(fileDescriptor, backlog) < 0) {
        throw runtime_error("listen failed");
    }
}

Socket Socket::accept() {
    int clientSocket = ::accept(fileDescriptor, nullptr, nullptr);

    if (clientSocket < 0) {
        throw runtime_error("accept failed");
    }

    return Socket(clientSocket);
}

string Socket::receive() {
    char buffer[1024] = { 0 };
    string message;
    ssize_t bytesRead;

    while ((bytesRead = recv(fileDescriptor, buffer, sizeof(buffer), 0)) > 0) {
        message.append(buffer, bytesRead);

        if (bytesRead < sizeof(buffer)) break;
    }

    if (bytesRead < 0) {
        throw runtime_error("receive failed");
    }

    return message;
}

void Socket::send(const std::string& data) {
    size_t totalSent = 0;

    while (totalSent < data.size())
    {
        ssize_t bytesSent = ::send(
            fileDescriptor,
            data.c_str() + totalSent,
            data.size() - totalSent,
            0
        );

        if (bytesSent < 0)
        {
            throw std::runtime_error("send failed");
        }

        totalSent += bytesSent;
    }
}

Socket::~Socket() {
    if (fileDescriptor != -1) {
        close(fileDescriptor);
    }
}