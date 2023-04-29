#ifndef HTTP_TCP_CONNECTION_H
#define HTTP_TCP_CONNECTION_H

#include "Constants.h"
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

class TcpConnection {
public:
    TcpConnection(int socket);
    ~TcpConnection();

    std::string read();
    void write(const std::string& message);

    bool isOpen() const;
private:
    int m_socket;
    std::array<char, 4 * ONE_MEGABYTE> m_buffer;
    
};

#endif