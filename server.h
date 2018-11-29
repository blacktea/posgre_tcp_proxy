#ifndef SERVER_H
#define SERVER_H

#include "common.h"


// Server listens %port% and accepts new connections.
// Accepting new connection, it calls registered callback passing socket
class  Server
{
public:
    using AcceptHandler = std::function<void(tcp::socket)>;

    explicit Server(ba::io_service& io_service, unsigned short port, AcceptHandler hndlr);
private:
    void _accept();

private:
    tcp::acceptor _acceptor;
    tcp::socket _socket;
    AcceptHandler _newConnHndr;
};

#endif // SERVER_H
