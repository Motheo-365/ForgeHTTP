#include "Networking/Connection.h"
#include <utility>

Connection::Connection(Socket&& other) noexcept : socket(std::move(other)) {}

std::string Connection::read() {
    return socket.receive();
}

void Connection::write(const std::string& data) {
    socket.send(data);
}

void Connection::close() {
    socket.close();
}