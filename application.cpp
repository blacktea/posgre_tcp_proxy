#include "application.h"

#include "server.h"
#include "client.h"
#include "connection.h"
#include "pgsqltcpproxy.h"

#include <iostream>

Application::Application(unsigned short proxyPort, std::string dbHost, unsigned short dbPort)
    : _proxyPort{proxyPort}
    , _dbHost{std::move(dbHost)}
    , _dbPort{dbPort}
    , _io_service{}
{

}

Application::~Application()
{
    _io_service.stop();
    for(auto& th: threads)
        if(th.joinable())
            th.join();
}

void Application::run()
{
    Server server{_io_service, _proxyPort, [this](tcp::socket clientSocket) {
            std::cout << "\nNew connection";
            auto clientConn = std::make_shared<Connection>( std::move(clientSocket) );
            auto clientHndlr = [clientConn = std::move(clientConn)](const boost::system::error_code& ec, tcp::socket dbSocket)
            {
                if(!ec && dbSocket.is_open()) {
                    std::cout << "\nDatabase connection is established" << std::endl;

                    auto dbConn = std::make_shared<Connection>(std::move(dbSocket));
                    std::make_shared<PgsqlTcpProxy>(std::move(clientConn), std::move(dbConn))->run();
                }
            };

            // Try to establish the connect with the database
            std::make_shared<ClientConnection>(_io_service, _dbHost, _dbPort, std::move(clientHndlr))->run();
        }};


    // create thread pool
    auto numThs = std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() - 1 : 1;

    for(auto i = 0u; i < numThs; ++i)
        threads.emplace_back([this] { _io_service.run(); });

    _io_service.run();

}
