#include "Networking/Socket.h"

int main() {
    cout << "ForgeHTTP starting...\n";

    try {
        Socket server;
        cout << "Socket created successfully\n";

        server.bind(8080);
        cout << "Socket bound to port 8080\n";

        server.listen(10);
        cout << "Socket is listening\n";
    }

    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    cout << "Server socket going out of scope...\n";
    return 0;
}