#ifndef HTTP_TCP_CONNECTION_H
#define HTTP_TCP_CONNECTION_H

#include <array>
#include <fstream>
#include <string>

#include "Constants.h"

class TcpConnection {
   public:
    TcpConnection(int socket);
    ~TcpConnection();

    std::string read();
    std::string readHeader();
    std::string readWholeBody(size_t size);
    void write(const std::string& message);

    bool isOpen() const;

   private:
    int m_socket;
    std::array<char, 16 * ONE_KILOBYTE> m_buffer;
    size_t m_bodySizeReadWhenReadingHeader = 0;
};

#endif