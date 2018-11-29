#include "connection.h"


#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/shared_array.hpp>

Connection::Connection(tcp::socket socket)
    : _socket(std::move(socket))
    , _strand(_socket.get_io_service())
{
    boost::system::error_code ec;
   _socket.set_option(tcp::no_delay(true), ec);
}



void Connection::close()
{
    auto self = shared_from_this();
    _socket.get_io_service().post(_strand.wrap(
    [self]{

        if(self->_socket.is_open()) {
            boost::system::error_code ec{};
            self->_socket.shutdown(tcp::socket::shutdown_both, ec);
            self->_socket.cancel(ec);
            self->_socket.close(ec);
        }
    }));

}

void Connection::async_send(const char *data, const std::size_t len, Connection::CompleteHandler hndlr)
{
    if(len == 0)
        return;

    auto buf = new (std::nothrow) char[len];
    if(!buf)
        return;

    std::copy(data, data + len, buf);
    boost::shared_array<char> sharedBuffer(buf);

    auto self = shared_from_this();
    auto writeCb = _strand.wrap([hndlr, self](const boost::system::error_code& ec, std::size_t bt)
    {
        if(hndlr)
            hndlr(ec, bt);
    });

    ba::async_write(_socket, ba::buffer(sharedBuffer.get(), len), writeCb);
}

void Connection::async_recv(Connection::CompleteHandler hndlr, size_t readAtLeastBytes)
{
    auto self = shared_from_this();
    auto readCb = _strand.wrap([hndlr, self](const boost::system::error_code& ec, std::size_t bt)
    {
        if(hndlr)
            hndlr(ec, bt);
    });

    ba::async_read(_socket, _buffer, ba::transfer_exactly(readAtLeastBytes), readCb);
}

void Connection::remove_bytes(std::size_t n)
{
    auto len = std::min(n, _buffer.size());
    _buffer.consume(len);
}

const char *Connection::data() const
{
    return ba::buffer_cast<const char*>(_buffer.data());
}

std::size_t Connection::length() const
{
    return _buffer.size();
}

