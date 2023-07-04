#ifndef HTTP_TCP_CONNECTION_H
#define HTTP_TCP_CONNECTION_H

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "Constants.h"

class TcpConnection {
   public:
    TcpConnection(int socket);
    ~TcpConnection();

    std::string read();
    std::string read(size_t size);
    void write(const std::string& message);

    bool isOpen() const;

   private:
    int m_socket;
    std::vector<char> m_buffer;
};

#endif