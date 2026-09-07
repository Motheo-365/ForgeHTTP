#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"

// Owns the full request lifecycle: listens, dispatches work, and shuts down cleanly.
//
// Top-level orchestrator. Knows about every other layer, but no layer below it knows about Server.
class Server {
    public:
        /*
            Binds and listens on the given port (falling back to the PORT environment variable when no port is passed), then enters the accept loop.
            Prints a short startup banner.
        */
        void start(int port = 0);

        /*
            Signals the accept loop to exit and shuts down the ThreadPool gracefully, letting in-flight requests finish before returning..
        */
        void stop();

    private:
        Socket listenSocket;
        ThreadPool pool;
        Router router;
        Middleware* middlewareChain;
        ServerEventPublisher events;
        bool running = false;
        
        /*
            The main loop: blocks on the listening Socket's accept(), wraps each new connection, and enqueues it on the ThreadPool rather than handling it inline — this is what makes the server concurrent.
        */
        void acceptConnections();

        /*
            Runs on a worker thread. Reads the raw request, parses it via HttpParser, runs it through the middleware chain, finds a matching Route via the Router, invokes its handler, and writes the resulting HttpResponse back through the Connection.
            Publishes RequestReceived/ResponseSent events along the way.
        */
        void handleConnection(Connection& c);
}; 

#endif