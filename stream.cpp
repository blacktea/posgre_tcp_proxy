#include "stream.h"
#include "connection.h"

Stream::Stream(std::shared_ptr<Connection> _connection, Pgparser::Stage stage, Stream::SinkHandler sink)
    : _conn{_connection}
    , _sink{std::move(sink)}
    , _packet{}
    , _parser{stage}
{

}

void Stream::run()
{
    _read_header();
}

void Stream::stop()
{
    _conn->close();
}

void Stream::_read_header()
{
#ifdef PG_PROXY_DEBUG
    std::cout << "\nheader size " << _parser.get_header_size() << std::endl;
#endif

    _conn->async_recv([self = shared_from_this()](const boost::system::error_code& ec, std::size_t bt)
    {
#ifdef PG_PROXY_DEBUG
        std::cout << "\nread header " << ec.message() << " bt " << bt << std::endl;
#endif
        if(!ec)
        {
            self->_packet = {};
            auto data = self->_conn->data();
            auto len = self->_conn->length();
            self->_parser.parse(self->_packet, data, len);
#ifdef PG_PROXY_DEBUG
            std::cout << "\n type " << (int)self->_packet.type
                      << " header_len" << self->_packet.header_len
                      << " data_len " << self->_packet.query_len
                      << std::endl;
#endif
            self->_read_payload();
        }
    }, _parser.get_header_size());
}

void Stream::_read_payload()
{
    _conn->async_recv([self = shared_from_this()](const boost::system::error_code& ec, std::size_t bt) mutable
    {
#ifdef PG_PROXY_DEBUG
        std::cout << "\npayload. " << ec.message() << " bt " << bt << std::endl;
#endif

        if(!ec) {
#ifdef PG_PROXY_DEBUG
            assert(self->_conn->length() == self->_packet.query_len + self->_packet.header_len);
#endif
            auto data = self->_conn->data();
            const auto len = self->_conn->length();

            self->_packet.data = data;
            self->_packet.data_len = len;

            self->_parser.parse(self->_packet, data, len);

            self->_sink(self->_packet);

            self->_conn->remove_bytes(len);

            self->run();
        }
    }, _packet.query_len);
}
