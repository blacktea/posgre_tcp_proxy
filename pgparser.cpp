#include "pgparser.h"
#include <algorithm>
#include <cassert>

namespace  {
const std::size_t startup_header_size = 4;
const std::size_t regular_header_size = 5;

static_assert (sizeof(std::uint32_t) == Pgparser::packet_len_size, "uint32_t is bigger than 4 bytes");

bool read_value(char *val, std::size_t val_sz, const char **src, std::size_t *src_sz)
{
    assert(val);
    assert(*src);
    assert(src_sz);

    for(; val_sz != 0 && val_sz <= *src_sz; --val_sz)
    {
        val[val_sz-1] = *(*src)++;
        (*src_sz)--;
    }

    if(val_sz != 0)
        return  false;

    return true;
}

template<typename T>
static
constexpr  char * to_char(T *v) noexcept
{
    return static_cast<char*>(static_cast<void*>(v));;
}

} // anonymous

Pgparser::Pgparser(Pgparser::Stage stage) noexcept
    : _stage{stage}
{

}

void Pgparser::parse(Packet &packet, const char *data, std::size_t len) noexcept
{
    switch (_stage) {

    case Stage::HeaderStartup: {
        _parseStartupHeader(packet, data, len);
        _stage = Stage::Payload;
    } break;
    case Stage::HeaderRegular: {
        _parseRegularHeader(packet, data, len);
        _stage = Stage::Payload;
    } break;
    case Stage::Payload: {
        _parsePayload(packet, data);
        _stage = Stage::HeaderRegular;
    } break;
    default:
        assert(false);
    }
}

std::size_t Pgparser::get_header_size() const noexcept
{
    return _stage == Stage::HeaderRegular ? regular_header_size : startup_header_size;
}

void Pgparser::_parseStartupHeader(Packet &packet, const char *data, std::size_t len) noexcept
{
    packet.type = 0;

    std::uint32_t packet_size = 0;
    read_value(to_char(&packet_size), sizeof(packet_size), &data, &len);

    packet.header_len = startup_header_size;

    packet.query = nullptr;
    packet.query_len = 0;
    if(packet_size >= packet_len_size)
        packet.query_len = packet_size - packet_len_size;
}

void Pgparser::_parseRegularHeader(Packet &packet, const char *data, std::size_t len) noexcept
{
    assert(data);
    assert(len >= regular_header_size);

    const char **ppdata = &data;
    std::size_t* plen = &len;
    read_value(to_char(&packet.type), sizeof(packet.type), ppdata, plen);

    std::uint32_t packet_size = 0;
    read_value(to_char(&packet_size), sizeof(packet_size), &data, &len);

    packet.header_len = regular_header_size;

    packet.query = nullptr;
    packet.query_len = 0;
    if(packet_size >= packet_len_size)
        packet.query_len = packet_size - packet_len_size;
}

void Pgparser::_parsePayload(Packet &packet, const char *data) noexcept
{
    packet.query = data + packet.header_len;
}

