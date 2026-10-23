#include "Networking/Socket.h"
#include "HTTP/HttpError.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cerrno>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>

Socket::Socket() {
    fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (fileDescriptor == -1) {
        throw std::runtime_error("Failed to create socket");
    }
}

Socket::Socket(int fd) : fileDescriptor(fd) {
    timeval timeout{};
    timeout.tv_sec = 5;
    if (setsockopt(fileDescriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 ||
        setsockopt(fileDescriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        const int savedError = errno;
        ::close(fileDescriptor);
        fileDescriptor = -1;
        throw std::runtime_error(
            "Failed to configure connection timeout: " +
            std::string(std::strerror(savedError)));
    }
};

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
    if (port < 1 || port > 65535) {
        throw std::invalid_argument("Port must be between 1 and 65535");
    }
    int opt = 1;
    struct sockaddr_in address{};
    if (setsockopt(fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to enable address reuse");
    }
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
    int clientSocket;
    do {
        clientSocket = ::accept(fileDescriptor, nullptr, nullptr);
    } while (clientSocket < 0 && errno == EINTR);

    if (clientSocket < 0) {
        throw std::runtime_error("accept failed");
    }

    return Socket(clientSocket);
}

bool Socket::waitForConnection(int timeoutMilliseconds) {
    pollfd descriptor{};
    descriptor.fd = fileDescriptor;
    descriptor.events = POLLIN;
    const int result = ::poll(&descriptor, 1, timeoutMilliseconds);
    if (result == 0) return false;
    if (result < 0) {
        if (errno == EINTR) return false;
        throw std::runtime_error("poll failed while waiting for a connection");
    }
    if (descriptor.revents & (POLLERR | POLLNVAL)) {
        throw std::runtime_error("listening socket failed");
    }
    return (descriptor.revents & (POLLIN | POLLHUP)) != 0;
}

void Socket::shutdown() {
    if (fileDescriptor != -1) {
        ::shutdown(fileDescriptor, SHUT_RDWR);
    }
}

void Socket::close() {
    if (fileDescriptor != -1) {
        ::close(fileDescriptor);
        fileDescriptor = -1;
    }
}

std::string Socket::receive() {
    constexpr std::size_t maxHeaderBytes = 32768;
    constexpr std::size_t maxBodyBytes = 1024 * 1024;
    char buffer[4096];
    std::string message;
    std::size_t expectedBodyLength = 0;
    std::size_t delimiterEnd = std::string::npos;
    bool hasContentLength = false;
    bool transferEncoding = false;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

    while (true) {
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now()).count();
        if (remaining <= 0) {
            throw HttpError(408, "Request timed out");
        }
        pollfd descriptor{};
        descriptor.fd = fileDescriptor;
        descriptor.events = POLLIN;
        int ready;
        do {
            ready = ::poll(&descriptor, 1, static_cast<int>(remaining));
        } while (ready < 0 && errno == EINTR);
        if (ready == 0) {
            throw HttpError(408, "Request timed out");
        }
        if (ready < 0) {
            throw std::runtime_error("poll failed while reading request");
        }

        const ssize_t bytesRead = recv(fileDescriptor, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            if (bytesRead < 0) {
                if (errno == EINTR) continue;
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    throw HttpError(408, "Request timed out");
                }
                throw std::runtime_error("receive failed");
            }
            if (delimiterEnd != std::string::npos &&
                message.size() < delimiterEnd + expectedBodyLength) {
                throw HttpError(400, "Incomplete request body");
            }
            break;
        }

        message.append(buffer, bytesRead);

        const std::size_t crlfEnd = message.find("\r\n\r\n");
        const std::size_t lfEnd = message.find("\n\n");
        if (delimiterEnd == std::string::npos &&
            (crlfEnd != std::string::npos || lfEnd != std::string::npos)) {
            const bool crlf = crlfEnd != std::string::npos;
            const std::size_t headerEnd = crlf ? crlfEnd : lfEnd;
            delimiterEnd = headerEnd + (crlf ? 4 : 2);
            if (headerEnd > maxHeaderBytes) {
                throw HttpError(413, "HTTP headers exceed the configured limit");
            }

            std::istringstream headers(message.substr(0, headerEnd));
            std::string header;
            std::getline(headers, header);
            while (std::getline(headers, header)) {
                if (!header.empty() && header.back() == '\r') {
                    header.pop_back();
                }
                const std::size_t colon = header.find(':');
                if (colon == std::string::npos) {
                    continue;
                }
                std::string name = header.substr(0, colon);
                std::transform(name.begin(), name.end(), name.begin(),
                    [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                std::string value = header.substr(colon + 1);
                const std::size_t first = value.find_first_not_of(" \t");
                value = first == std::string::npos ? "" : value.substr(first);
                const std::size_t last = value.find_last_not_of(" \t");
                if (last != std::string::npos) value.erase(last + 1);
                if (name == "content-length") {
                    if (hasContentLength) {
                        throw HttpError(400, "Duplicate Content-Length header");
                    }
                    hasContentLength = true;
                    if (value.empty() ||
                        !std::all_of(value.begin(), value.end(), [](unsigned char c) {
                            return c >= '0' && c <= '9';
                        })) {
                        throw HttpError(400, "Invalid Content-Length");
                    }
                    try {
                        std::size_t parsed = 0;
                        expectedBodyLength = std::stoull(value, &parsed);
                        if (parsed != value.size()) {
                            throw HttpError(400, "Invalid Content-Length");
                        }
                    } catch (const std::invalid_argument&) {
                        throw HttpError(400, "Invalid Content-Length");
                    } catch (const std::out_of_range&) {
                        throw HttpError(413, "Request body exceeds the configured limit");
                    }
                } else if (name == "transfer-encoding") {
                    transferEncoding = true;
                }
            }
            if (expectedBodyLength > maxBodyBytes) {
                throw HttpError(413, "Request body exceeds the configured limit");
            }
            if (transferEncoding) {
                return message;
            }
        } else if (delimiterEnd == std::string::npos &&
                   message.size() > maxHeaderBytes) {
            throw HttpError(413, "HTTP headers exceed the configured limit");
        }

        if (delimiterEnd == std::string::npos) {
            continue;
        }

        if (message.size() >= delimiterEnd + expectedBodyLength) {
            break;
        }
    }

    return message;
}

std::string Socket::peerAddress() const {
    sockaddr_in address{};
    socklen_t length = sizeof(address);
    if (getpeername(fileDescriptor, reinterpret_cast<sockaddr*>(&address), &length) < 0) {
        return "unknown";
    }
    char buffer[INET_ADDRSTRLEN]{};
    if (inet_ntop(AF_INET, &address.sin_addr, buffer, sizeof(buffer)) == nullptr) {
        return "unknown";
    }
    return buffer;
}

void Socket::send(const std::string& data) {
    size_t totalSent = 0;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);

    while (totalSent < data.size())
    {
        const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now()).count();
        if (remaining <= 0) {
            throw HttpError(408, "Response write timed out");
        }
        pollfd descriptor{};
        descriptor.fd = fileDescriptor;
        descriptor.events = POLLOUT;
        int ready;
        do {
            ready = ::poll(&descriptor, 1, static_cast<int>(remaining));
        } while (ready < 0 && errno == EINTR);
        if (ready == 0) {
            throw HttpError(408, "Response write timed out");
        }
        if (ready < 0) {
            throw std::runtime_error("poll failed while writing response");
        }

        ssize_t bytesSent = ::send(
            fileDescriptor,
            data.c_str() + totalSent,
            data.size() - totalSent,
            MSG_NOSIGNAL
        );

        if (bytesSent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                throw HttpError(408, "Response write timed out");
            }
            throw std::runtime_error("send failed");
        }
        if (bytesSent == 0) {
            throw std::runtime_error("connection closed while sending response");
        }

        totalSent += bytesSent;
    }
}

Socket::~Socket() {
    close();
}