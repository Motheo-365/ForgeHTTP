#include "Networking/Socket.h"

#include <netinet/in.h>
#include <unistd.h>

Socket::Socket() {
    fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (fileDescriptor == -1) {
        throw std::runtime_error("Failed to create socket");
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
        throw std::runtime_error("bind failed");
    }
}

void Socket::listen(int backlog) {
    if (::listen(fileDescriptor, backlog) < 0) {
        throw std::runtime_error("listen failed");
    }
}

Socket Socket::accept() {
    int clientSocket = ::accept(fileDescriptor, nullptr, nullptr);

    if (clientSocket < 0) {
        throw std::runtime_error("accept failed");
    }

    return Socket(clientSocket);
}

void Socket::close() {
    if (fileDescriptor != -1) {
        ::close(fileDescriptor);
        fileDescriptor = -1;
    }
}

std::string Socket::receive() {
    char buffer[4096] = { 0 };
    std::string message;
    std::size_t expectedBodyLength = 0;
    bool headersComplete = false;

    while (true) {
        const ssize_t bytesRead = recv(fileDescriptor, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            if (bytesRead < 0) {
                throw std::runtime_error("receive failed");
            }
            break;
        }

        message.append(buffer, bytesRead);

        const std::size_t headerEnd = message.find("\r\n\r\n");
        const std::size_t alternateHeaderEnd = message.find("\n\n");
        const std::size_t delimiterEnd = headerEnd != std::string::npos
            ? headerEnd + 4
            : alternateHeaderEnd != std::string::npos
                ? alternateHeaderEnd + 2
                : std::string::npos;

        if (delimiterEnd == std::string::npos) {
            continue;
        }

        if (!headersComplete) {
            headersComplete = true;
            const std::string headers = message.substr(0, delimiterEnd);
            const std::string marker = "Content-Length:";
            const std::size_t contentLengthStart = headers.find(marker);

            if (contentLengthStart != std::string::npos) {
                const std::size_t valueStart = contentLengthStart + marker.size();
                expectedBodyLength = std::stoul(headers.substr(valueStart));
            }
        }

        if (message.size() >= delimiterEnd + expectedBodyLength) {
            break;
        }
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
    close();
}