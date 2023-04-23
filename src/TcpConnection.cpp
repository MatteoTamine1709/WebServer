#include "TcpConnection.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <exception>

#include <spdlog/spdlog.h>

TcpConnection::TcpConnection(int socket) : m_socket(socket) {}

TcpConnection::~TcpConnection() {
    close(m_socket);
}

std::string TcpConnection::read() {
    int bytes = ::read(m_socket, m_buffer.data(), m_buffer.size());
    if (bytes == -1)
        throw std::runtime_error("read() failed");
    if (bytes == 0 && !isOpen())
        throw std::runtime_error("connection closed");
    if (bytes > 0)
        spdlog::debug("read {} bytes", bytes);
    return std::string(m_buffer.data(), bytes);
}

void TcpConnection::write(const std::string& message) {
    int bytes = ::write(m_socket, message.data(), message.size());
    if (bytes == -1)
        throw std::runtime_error("write() failed");
    if (bytes > 0)
        spdlog::debug("wrote {} bytes", bytes);
}

bool TcpConnection::isOpen() const {
    int error_code = 0;
    socklen_t error_code_size = sizeof(error_code);

    // Check if the socket is open by getting the socket error status
    return getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size) == 0;
}