#include "src/TcpServer.h"

int main(int, char**) {
    TcpServer &server = TcpServer::getInstance();

    server.run();

    return 0;
}
