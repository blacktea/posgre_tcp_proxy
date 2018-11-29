#ifndef APPLICATION_H
#define APPLICATION_H


#include "common.h"

#include <thread>

class Application
{
public:
    explicit Application(unsigned short proxyPort, std::string dbHost, unsigned short dbPort);

    ~Application();

    void run();

private:

    unsigned short _proxyPort;
    std::string _dbHost;
    unsigned short _dbPort;

    ba::io_service _io_service;
    std::vector<std::thread> threads;
};
#endif // APPLICATION_H
