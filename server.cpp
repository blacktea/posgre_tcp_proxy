#include "server.h"

#include <iostream>

Server::Server(boost::asio::io_service &io_service, unsigned short port, Server::AcceptHandler hndlr)
    : _acceptor{io_service, {ip::tcp::v4(), port}, true}
    , _socket{io_service}
    , _newConnHndr(std::move(hndlr))
{
    _accept();
}

void Server::_accept()
{
    _acceptor.async_accept(_socket, [this](const boost::system::error_code &ec)
    {
        if(ec) {
            std::cerr << "[Server] accept: " << ec.message() << std::endl;
        }
        else {
            if(_newConnHndr)
                _newConnHndr(std::move(this->_socket));
        }
        _accept();
    });
}
