#ifndef PGSQLTCPPROXY_H
#define PGSQLTCPPROXY_H

#include "pgparser.h"
#include <memory>

class Connection;
class PgsqlTcpProxy : public std::enable_shared_from_this<PgsqlTcpProxy>
{
public:

    PgsqlTcpProxy(std::shared_ptr<Connection> clientSock, std::shared_ptr<Connection> dbSocket);

    void run();
    void stop();
private:

    void _printQuery(Pgparser::Packet packet);

private:
    std::shared_ptr<Connection> _clientSocket;
    std::shared_ptr<Connection> _dbSocket;
};
#endif // PGSQLTCPPROXY_H
