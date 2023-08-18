#ifndef HTTP_TCP_CONNECTION_H
#define HTTP_TCP_CONNECTION_H

#include <array>
#include <fstream>
#include <string>

#include "Constants.h"
#include "StreamFile.h"

class TcpConnection {
   public:
    TcpConnection(int socket);
    ~TcpConnection();

    std::string read();
    // StreamFile readTmp(size_t size);
    std::string readHeader();
    void write(const std::string& message);

    bool isOpen() const;

   private:
    int m_socket;
    std::array<char, 16 * ONE_KILOBYTE> m_buffer;
    size_t m_bodySizeReadWhenReadingHeader = 0;
};

#endif