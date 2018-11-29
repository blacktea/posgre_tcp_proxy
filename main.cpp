#include <iostream>

#include "application.h"

template<typename T>
T value_cast(const std::string &val_str)
{
    std::stringstream ss(val_str);
    T val;
    ss >> val;
    return val;
}

void help()
{
    std::cout << "\nPostreSql tcp proxy command line arguments:\n"
              << "\n--proxy-port - %port%(2244)"
              << "\n--db-host - ip-address/host-name of database(localhost)"
              << "\n--db-port - port of database (5432)";
}

int main(int argc, char *argv[])
{
    if(argc < 7) {
        help();
        return 1;
    }

    try {

        std::string dbHost;
        unsigned short dbPort{0};
        unsigned short proxyPort{0};


        for(auto i = 2; i < argc; i += 2)
        {
            if(strcmp("--proxy-port", argv[i-1]) == 0 )
                proxyPort = value_cast<unsigned short>(argv[i]);
            else if(strcmp("--db-host", argv[i-1]) == 0)
                dbHost = argv[i];
            else if(strcmp("--db-port", argv[i-1]) == 0)
                dbPort = value_cast<unsigned short>(argv[i]);
        }

        std::cout << "proxy-port: " << proxyPort
                  << " db-host: " << dbHost
                  << " db-port: " << dbPort
                  << std::endl;


        Application app(proxyPort, dbHost, dbPort);
        app.run();

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}
