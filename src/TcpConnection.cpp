#include "TcpConnection.h"

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <exception>
#include <iostream>

TcpConnection::TcpConnection(int socket) : m_socket(socket) {
    spdlog::debug("new connection {}", m_socket);
}

TcpConnection::~TcpConnection() { close(m_socket); }

Result<std::string, HttpStatus> TcpConnection::read() {
    int bytes = ::read(m_socket, m_buffer.data() + m_bufferSize,
                       m_buffer.size() - m_bufferSize);
    if (bytes == 0 || bytes == -1)
        return Result<std::string, HttpStatus>::err(BAD_REQUEST);
    return Result<std::string, HttpStatus>::ok(
        std::string(m_buffer.data(), bytes));
}

Result<std::string, HttpStatus> TcpConnection::readHeader() {
    const auto& result = this->read();
    if (!result.isOk())
        return Result<std::string, HttpStatus>::err(result.err().code);

    const std::string& header = result.ok();
    // If there was data read after the header, put it back in the buffer
    size_t end = header.find("\r\n\r\n");

    // This would mean that the header is > 16KB
    if (end == std::string::npos)
        return Result<std::string, HttpStatus>::err(PAYLOAD_TOO_LARGE);
    if (end != header.size() - 4) {
        std::string data = header.substr(end + 4);
        if (data.size()) {
            m_bufferSize = data.size();
            std::copy(data.begin(), data.end(), m_buffer.begin());
        }
    }

    // Remove the data after the header
    return Result<std::string, HttpStatus>::ok(header.substr(0, end));
}

Result<std::string, HttpStatus> TcpConnection::readSize(size_t size) {
    size_t bytes_read = 0;
    std::string buffer;
    while (bytes_read < size) {
        const auto& result = this->read();
        if (!result.isOk())
            return Result<std::string, HttpStatus>::err(result.err().code);

        buffer += result.ok();
        bytes_read += result.ok().size();
        m_bufferSize = 0;
    }
    return Result<std::string, HttpStatus>::ok(buffer);
}

void TcpConnection::write(const std::string& message) {
    if (message.empty()) return;
    if (!this->isOpen()) return;
    int bytes = ::send(m_socket, message.data(), message.size(), 0);
    if (bytes == -1)
        throw std::runtime_error("send() failed: " +
                                 std::string(strerror(errno)));
}

bool TcpConnection::isOpen() const {
    int error_code = 0;
    socklen_t error_code_size = sizeof(error_code);

    // Check if the socket is open by getting the socket error status
    return getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &error_code,
                      &error_code_size) == 0;
}