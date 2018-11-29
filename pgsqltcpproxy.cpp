#include "pgsqltcpproxy.h"
#include "stream.h"
#include "connection.h"

#include <iostream>

PgsqlTcpProxy::PgsqlTcpProxy(std::shared_ptr<Connection> clientSock, std::shared_ptr<Connection> dbSocket)
    : _clientSocket{std::move(clientSock)}
    , _dbSocket{std::move(dbSocket)}
{

}

void PgsqlTcpProxy::run()
{
    auto self = shared_from_this();
    std::shared_ptr<Stream> clientStream(new Stream(_clientSocket, Pgparser::Stage::HeaderStartup,
    [self](const Pgparser::Packet packet)
    {
        self->_printQuery(packet);

        self->_dbSocket->async_send(packet.data, packet.data_len, nullptr);

    })
    , [self = shared_from_this()](Stream *ptr) { self->_dbSocket->close(); delete ptr;});


    std::shared_ptr<Stream> dbStream(new Stream(_dbSocket,  Pgparser::Stage::HeaderRegular,
    [self](const Pgparser::Packet packet)
    {
        self->_clientSocket->async_send(packet.data, packet.data_len, nullptr);
    })
    , [self = shared_from_this()](Stream *ptr) { self->_clientSocket->close(); delete ptr; });

    clientStream->run();
    dbStream->run();
}

void PgsqlTcpProxy::stop()
{
}

void PgsqlTcpProxy::_printQuery(Pgparser::Packet packet)
{
    if(packet.query && packet.query_len != 0) {
        std::cout << "\nquery. ";
        std::cout.write(packet.query, packet.query_len);
        std::cout << std::endl;
    }
}
