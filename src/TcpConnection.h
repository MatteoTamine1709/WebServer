#ifndef HTTP_TCP_CONNECTION_H
#define HTTP_TCP_CONNECTION_H

#include <memory>
#include <string>
#include <vector>

class TcpConnection {
public:
    TcpConnection(int socket);
    ~TcpConnection();

    std::string read();
    void write(const std::string& message);

    bool isOpen() const;

private:
    int m_socket;
    std::vector<char> m_buffer;
};

#endif