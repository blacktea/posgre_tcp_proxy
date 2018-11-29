#include "client.h"
#include <iostream>

ClientConnection::ClientConnection(boost::asio::io_service &io_service, const std::string &host, unsigned short port, ClientConnection::ConnectionHandler hndlr)
    : _resolver{io_service}
    , _socket{io_service}
    , _host{host}
    , _port{port}
    , _connHndlr(std::move(hndlr))
{
}

void ClientConnection::run() {

    auto self = shared_from_this();
    _resolver.async_resolve({_host, std::to_string(_port)},
                            [self](const boost::system::error_code& ec, tcp::resolver::iterator iter)
    {
        if(ec) {
            std::cerr << "[Client] resolve. " << ec.message() << std::endl;
            if(self->_connHndlr)
                self->_connHndlr(ec, std::move(self->_socket));
        }
        else {

            self->endpoint_iterator = iter;
            self->async_connect();
        }
    });
}

void ClientConnection::async_connect()
{

    auto self = shared_from_this();
    _socket.async_connect(*endpoint_iterator,
                          [self](const boost::system::error_code& ec)
    {
        if(ec) {
            std::cerr << "[Client] connect. " << ec.message() << std::endl;
            if(self->endpoint_iterator == tcp::resolver::iterator()) {
                // we could not connect
                if(self->_connHndlr)
                    self->_connHndlr(ec, std::move(self->_socket));
            }
            else {
                ++self->endpoint_iterator;
                self->async_connect();
            }
        }
        else {
            if(self->_connHndlr)
                self->_connHndlr(ec, std::move(self->_socket));
        }
    });
}
