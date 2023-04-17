#include <iostream>
#include "src/TcpServer.h"

int main(int, char**) {
    TcpServer &server = TcpServer::getInstance("127.0.0.1", "8081");

    server.run();

    std::cout << "Cleannnn" << std::endl;
    return 0;
}
