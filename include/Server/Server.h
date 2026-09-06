#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"

// Top-level orchestrator. Knows about every other layer, but no layer below it knows about Server.
class Server {
    public:
        void start(int port);
        void stop();

    private:
        Socket listenSocket;
        ThreadPool pool;
        Router router;
        Middleware* middlewareChain;
        ServerEventPublisher events;
        
        void acceptConnections();
        void handleConnection(Connection c);
}; 

#endif