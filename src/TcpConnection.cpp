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

std::string TcpConnection::read() {
    if (m_bodySizeReadWhenReadingHeader) {
        std::string str(m_buffer.data(), m_bodySizeReadWhenReadingHeader);
        m_bodySizeReadWhenReadingHeader = 0;
        return str;
    }
    int bytes = ::read(m_socket, m_buffer.data(), m_buffer.size());
    if (bytes == 0 || bytes == -1) {
        close(m_socket);
        spdlog::debug("connection closed {}", m_socket);
        return "";
    }
    return std::string(m_buffer.data(), bytes);
}

// StreamFile TcpConnection::readTmp(size_t size) {
//     if (size == 0) return StreamFile();
//     StreamFile tmpFile;
//     tmpFile.create();
//     size_t count = 0;
//     do {
//         int bytes = ::read(m_socket, m_buffer.data(), m_buffer.size());
//         if (bytes == 0 || bytes == -1) {
//             close(m_socket);
//             spdlog::debug("connection closed {}", m_socket);
//             return tmpFile;
//         }
//         tmpFile.write(m_buffer.data(), bytes);
//         count += bytes;
//     } while (count < size);
//     return tmpFile;
// }

std::string TcpConnection::readHeader() {
    std::string header;
    // Read until we find the end of the header (two consecutive CRLF)
    while (header.find("\r\n\r\n") == std::string::npos) {
        int bytes = ::read(m_socket, m_buffer.data(), m_buffer.size());
        if (bytes == 0 || bytes == -1) {
            close(m_socket);
            spdlog::debug("connection closed {}", m_socket);
            return "";
        }
        header += std::string(m_buffer.data(), bytes);
    }

    // If there was data read after the header, put it back in the buffer
    size_t end = header.find("\r\n\r\n");
    if (end != header.size() - 4) {
        std::string data = header.substr(end + 4);
        if (data.size()) m_bodySizeReadWhenReadingHeader = data.size();
        std::copy(data.begin(), data.end(), m_buffer.begin());
    }

    // Remove the data after the header
    header = header.substr(0, end);

    return header;
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