#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include <memory>

// The class connects to given host and port
// If it succeed it will call the callback
class ClientConnection : public std::enable_shared_from_this<ClientConnection>
{
public:
    using ConnectionHandler = std::function<void(const boost::system::error_code&, tcp::socket)>;

    explicit ClientConnection(ba::io_service& io_service, const std::string& host, unsigned short port, ConnectionHandler hndlr);

    void run();

private:
    void async_connect();

private:
    tcp::resolver _resolver;
    tcp::socket _socket;
    std::string _host;
    unsigned short _port;
    ConnectionHandler _connHndlr;

    tcp::resolver::iterator endpoint_iterator;
};
#endif // CLIENT_H
