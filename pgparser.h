#ifndef PGPARSER_H
#define PGPARSER_H

#include <cstdint>

// The class parsers postgresql packets
class Pgparser {
public:
    enum class Stage { None = 0, HeaderStartup, HeaderRegular, Payload };
    struct Packet
    {
        std::uint8_t type;
        std::uint32_t header_len;
        const char *query;
        std::uint32_t query_len;

        const char *data;
        std::int32_t data_len;
    };
    static const std::size_t packet_len_size = 4;

    explicit Pgparser(Stage) noexcept;

    void parse(Packet &packet, const char* data, std::size_t len) noexcept;

    std::size_t get_header_size() const noexcept;

private:

    void _parseStartupHeader(Packet &packet, const char* data, std::size_t len) noexcept;

    void _parseRegularHeader(Packet &packet, const char* data, std::size_t len) noexcept;

    void _parsePayload(Packet &packet, const char* data) noexcept;


private:
    Stage _stage;
};
#endif // PGPARSER_H
