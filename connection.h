#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"

#include <functional>

#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>


// The class is a wrapper over socket, provides async read/write methods
class Connection : public std::enable_shared_from_this<Connection>
{
public:
    using CompleteHandler = std::function<void(const boost::system::error_code& ec, std::size_t bt)>;

    explicit Connection(tcp::socket socket);

    void close();

    void async_send(const char* data, const std::size_t len, CompleteHandler hndlr);

    void async_recv(CompleteHandler hndlr, size_t readAtLeastBytes);

    void remove_bytes(std::size_t n);

    const char* data() const;

    std::size_t length() const;

private:
    tcp::socket _socket;
    ba::strand _strand;
    ba::streambuf _buffer;
};
#endif // CONNECTION_H
