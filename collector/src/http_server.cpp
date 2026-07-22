/**
 * HTTP Server 实现 - 轻量级仅处理 POST
 */
#include "http_server.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>

#define BUFFER_SIZE 8192
#define MAX_HEADERS 32

HttpServer::HttpServer(int port)
    : m_port(port), m_sockfd(-1), m_running(false) {}

HttpServer::~HttpServer() { stop(); }

void HttpServer::setCallback(HttpCallback cb) { m_callback = cb; }

int HttpServer::start() {
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);

    if (bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(m_sockfd);
        return -2;
    }

    if (listen(m_sockfd, 10) < 0) {
        perror("listen");
        close(m_sockfd);
        return -3;
    }

    m_running = true;
    m_thread = std::thread(&HttpServer::acceptLoop, this);
    m_thread.detach();

    return 0;
}

void HttpServer::stop() {
    m_running = false;
    if (m_sockfd >= 0) {
        close(m_sockfd);
        m_sockfd = -1;
    }
}

void HttpServer::acceptLoop() {
    while (m_running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(m_sockfd, (struct sockaddr*)&client_addr, &addr_len);

        if (client_fd < 0) {
            if (m_running) perror("accept");
            continue;
        }

        // 简单处理：每个连接一个线程
        std::thread t(&HttpServer::handleClient, this, client_fd);
        t.detach();
    }
}

void HttpServer::handleClient(int client_fd) {
    std::string method, path, body;
    if (readRequest(client_fd, method, path, body) != 0) {
        close(client_fd);
        return;
    }

    // 只处理 POST
    if (method != "POST") {
        sendResponse(client_fd, "{\"error\":-1,\"msg\":\"Only POST supported\"}");
        close(client_fd);
        return;
    }

    // 业务处理
    std::string response;
    if (m_callback) {
        m_callback(body, response);
    } else {
        response = "{\"error\":0}";
    }

    sendResponse(client_fd, response);
    close(client_fd);
}

int HttpServer::readRequest(int fd, std::string& method,
                             std::string& path, std::string& body) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // 读取 HTTP 头
    int total = 0;
    int n;
    while (total < BUFFER_SIZE - 1) {
        n = recv(fd, buffer + total, BUFFER_SIZE - 1 - total, 0);
        if (n <= 0) return -1;
        total += n;
        buffer[total] = '\0';
        if (strstr(buffer, "\r\n\r\n")) break;
    }

    // 解析第一行: METHOD PATH HTTP/1.1
    char* line = strtok(buffer, "\r\n");
    if (!line) return -1;

    char m[16], p[256], proto[16];
    if (sscanf(line, "%15s %255s %15s", m, p, proto) != 3) return -1;
    method = m;
    path = p;

    // 解析 Content-Length
    int content_length = 0;
    while ((line = strtok(NULL, "\r\n")) != NULL) {
        if (strncmp(line, "Content-Length:", 15) == 0) {
            content_length = atoi(line + 16);
            break;
        }
    }

    if (content_length <= 0) return 0;

    // 查找 body 起始位置
    char* body_start = strstr(buffer, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;

    // 已读到的 body 部分
    int header_end = body_start - buffer;
    int already_read = total - header_end;

    if (already_read > 0) {
        body.assign(body_start, already_read);
    }

    // 读取剩余 body
    int remaining = content_length - already_read;
    while (remaining > 0) {
        n = recv(fd, buffer, remaining < BUFFER_SIZE ? remaining : BUFFER_SIZE - 1, 0);
        if (n <= 0) break;
        body.append(buffer, n);
        remaining -= n;
    }

    return 0;
}

int HttpServer::sendResponse(int fd, const std::string& body) {
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: application/json; charset=utf-8\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;

    std::string resp = oss.str();
    int n = send(fd, resp.c_str(), resp.size(), 0);
    return n > 0 ? 0 : -1;
}
