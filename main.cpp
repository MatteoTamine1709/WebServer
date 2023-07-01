#include "src/TcpServer.h"

int main(int, char**) {
    TcpServer::getInstance().run();

    return 0;
}
