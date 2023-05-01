#include "../TcpServer.h"
#include <iostream>

void TcpServer::stop() {
    kill(getpid(), SIGTERM);
}

void TcpServer::status() {
    spdlog::info("Server is running on {}:{}", m_host, m_port);
    spdlog::info("PID: {}", getpid());
    spdlog::info("The number of active connections: {}", m_numberConnections);
    spdlog::info("The number of handled requests: {}", m_numberRequests);
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