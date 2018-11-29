#ifndef STREAM_H
#define STREAM_H

#include "pgparser.h"

#include <memory>


class Connection;

class Stream : public std::enable_shared_from_this<Stream>
{
    using SinkHandler = std::function<void (const Pgparser::Packet)>;
public:

    Stream(std::shared_ptr<Connection> _connection, Pgparser::Stage stage, SinkHandler);
    void run();
    void stop();

private:

    void _read_header();
    void _read_payload();

private:

    std::shared_ptr<Connection> _conn;
    SinkHandler _sink;
    Pgparser::Packet _packet;
    Pgparser _parser;
};

#endif // STREAM_H
