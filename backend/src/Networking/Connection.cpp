#include "Networking/Connection.h"
#include <utility>

Connection::Connection(Socket&& other) noexcept : socket(std::move(other)) {}

std::string Connection::read() {
    return socket.receive();
}

std::string Connection::peerAddress() const {
    return socket.peerAddress();
}

void Connection::write(const std::string& data) {
    socket.send(data);
}

void Connection::close() {
    socket.close();
}