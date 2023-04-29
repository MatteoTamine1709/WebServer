#include "TcpConnection.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>

#include <spdlog/spdlog.h>

TcpConnection::TcpConnection(int socket) : m_socket(socket) {
    spdlog::debug("new connection {}", m_socket);
}

TcpConnection::~TcpConnection() {
    close(m_socket);
}

std::string TcpConnection::read() {
    int bytes = ::recv(m_socket, m_buffer.data(), m_buffer.size(), 0);
    if (bytes == 0 && !isOpen())
        throw std::runtime_error("connection closed: " + std::string(strerror(errno)));
    if (bytes == -1)
        throw std::runtime_error("recv() failed: " + std::string(strerror(errno)));
    if (bytes == 0 && errno == EINVAL)
        close(m_socket);
    return std::string(m_buffer.data(), bytes);
}

void TcpConnection::write(const std::string& message) {
    int bytes = ::send(m_socket, message.data(), message.size(), 0);
    if (bytes == -1)
        throw std::runtime_error("send() failed: " + std::string(strerror(errno)));
    // if (bytes > 0)
        // spdlog::debug("sent {} bytes from {}", bytes, message.size());
}

bool TcpConnection::isOpen() const {
    int error_code = 0;
    socklen_t error_code_size = sizeof(error_code);

    // Check if the socket is open by getting the socket error status
    return getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size) == 0;
}