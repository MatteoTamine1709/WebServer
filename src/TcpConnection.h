#ifndef HTTP_TCP_CONNECTION_H
#define HTTP_TCP_CONNECTION_H

#include <array>
#include <fstream>
#include <string>

#include "Constants.h"
#include "HttpStatus.h"
#include "Result.h"

class TcpConnection {
   public:
    TcpConnection(int socket);
    ~TcpConnection();

    Result<std::string, HttpStatus> read();
    Result<std::string, HttpStatus> readHeader();
    Result<std::string, HttpStatus> readSize(size_t size);
    void write(const std::string& message);

    bool isOpen() const;

   private:
    int m_socket;
    std::array<char, MAX_HEADER_SIZE> m_buffer;
    size_t m_bufferSize = 0;
};

#endif