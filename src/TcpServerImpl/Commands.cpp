#include <iostream>

#include "../TcpServer.h"

void TcpServer::stop() { m_running = false; }

void TcpServer::status() {
    spdlog::info("Server is running on {}:{}", m_host, m_port);
    spdlog::info("PID: {}", getpid());
    spdlog::info("The number of connections: {}", m_numberOfConnection);
    spdlog::info("The number of requests handled: {}",
                 m_numberOfRequestHandled);
    spdlog::info("The number of tasks queued: {}", m_threadpool.getTaskCount());
}

void TcpServer::help() {
    spdlog::info("Available commands:");
    spdlog::info("stop - stop the server");
    spdlog::info("status - show server status");
    spdlog::info("help - show this message");
}

void TcpServer::handleCommands() {
    std::string command;
    while (m_running) {
        std::cin >> command;
        if (m_commands.find(command) != m_commands.end())
            (this->*m_commands[command])();
        else
            spdlog::error("Unknown command: {}", command);
    }
}