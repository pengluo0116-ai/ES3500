/**
 * HTTP Server 实现 - 基于POSIX socket的轻量POST服务器
 * 
 * 架构: 
 *   main thread → acceptLoop() → 每个连接新线程 handleClient()
 *   
 * HTTP协议支持:
 *   - 仅POST方法
 *   - 解析 Content-Length 头确定请求体大小
 *   - 响应固定为 application/json; charset=utf-8
 * 
 * 注意: 
 *   - strtok() 非线程安全,但仅在handleClient线程内使用,无并发问题
 *   - sendmsg() 使用 MSG_NOSIGNAL 避免 SIGPIPE
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

#define BUFFER_SIZE 8192          // 请求缓冲区大小 (足够容纳HTTP头+JSON体)

HttpServer::HttpServer(int port)
    : m_port(port), m_sockfd(-1), m_running(false) {}

HttpServer::~HttpServer() { stop(); }

void HttpServer::setCallback(HttpCallback cb) { m_callback = cb; }

int HttpServer::start() {
    // 创建TCP socket
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) { perror("socket"); return -1; }

    // 设置 SO_REUSEADDR: 重启时立即复用端口 (避免 TIME_WAIT 等待)
    int opt = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定 0.0.0.0:port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);

    if (bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(m_sockfd); return -2;
    }
    if (listen(m_sockfd, 10) < 0) {                // backlog=10
        perror("listen"); close(m_sockfd); return -3;
    }

    // 启动accept线程 (detach, 不阻塞主线程)
    m_running = true;
    m_thread = std::thread(&HttpServer::acceptLoop, this);
    m_thread.detach();
    return 0;
}

void HttpServer::stop() {
    m_running = false;
    if (m_sockfd >= 0) { close(m_sockfd); m_sockfd = -1; }
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
        // 每个连接在独立线程处理 (detach, 无需join)
        std::thread t(&HttpServer::handleClient, this, client_fd);
        t.detach();
    }
}

void HttpServer::handleClient(int client_fd) {
    std::string method, path, body;

    // ① 读取HTTP请求
    if (readRequest(client_fd, method, path, body) != 0) {
        close(client_fd); return;                   // 读取失败=静默关闭
    }

    // ② 仅接受POST
    if (method != "POST") {
        sendResponse(client_fd, "{\"error\":-1,\"msg\":\"Only POST supported\"}");
        close(client_fd); return;
    }

    // ③ 执行业务回调
    std::string response;
    try {
        if (m_callback)
            m_callback(body, response);
        else
            response = "{\"error\":0}";
    } catch (...) {
        response = "{\"error\":-99,\"msg\":\"Internal error\"}";
    }

    // ④ 返回响应
    sendResponse(client_fd, response);
    close(client_fd);
}

// 解析HTTP请求: 方法 + 路径 + 请求体
int HttpServer::readRequest(int fd, std::string& method,
                             std::string& path, std::string& body) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // 读取HTTP头 (循环读直到遇到 \r\n\r\n)
    int total = 0, n;
    while (total < BUFFER_SIZE - 1) {
        n = recv(fd, buffer + total, BUFFER_SIZE - 1 - total, 0);
        if (n <= 0) return -1;                      // 连接关闭或错误
        total += n;
        buffer[total] = '\0';
        if (strstr(buffer, "\r\n\r\n")) break;       // 找到头结束标记
    }

    // ★ 先定位body (在strtok破坏buffer前!)
    char* body_start = strstr(buffer, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;

    // 解析请求行: "POST / HTTP/1.1"
    char* line = strtok(buffer, "\r\n");
    if (!line) return -1;

    char m[16], p[256], proto[16];
    if (sscanf(line, "%15s %255s %15s", m, p, proto) != 3) return -1;
    method = m; path = p;

    // 解析 Content-Length 头
    int content_length = 0;
    while ((line = strtok(NULL, "\r\n")) != NULL) {
        if (strncmp(line, "Content-Length:", 15) == 0) {
            content_length = atoi(line + 16);       // 跳过"Content-Length:" (15字符+1空格)
            break;
        }
    }

    if (content_length <= 0) return 0;              // 无请求体

    // 审计修复: 限制请求体最大1MB，防止恶意超大Content-Length
    if (content_length > 1024 * 1024) return -1;

    int already_read = total - (body_start - buffer);   // 已读入的请求体字节数
    if (already_read > 0)
        body.assign(body_start, already_read);

    // 继续读取剩余请求体
    int remaining = content_length - already_read;
    while (remaining > 0) {
        n = recv(fd, buffer, remaining < BUFFER_SIZE ? remaining : BUFFER_SIZE - 1, 0);
        if (n <= 0) return -1;
        body.append(buffer, n);
        remaining -= n;
    }
    return 0;
}

// 发送HTTP响应 (header + body, 使用writev等价sendmsg一次发送)
int HttpServer::sendResponse(int fd, const std::string& body) {
    char header[512];
    int len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n", body.size());

    struct iovec iov[2];
    iov[0].iov_base = header;
    iov[0].iov_len = len;
    iov[1].iov_base = (void*)body.c_str();
    iov[1].iov_len = body.size();

    struct msghdr msg = {0};
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;

    ssize_t sent = sendmsg(fd, &msg, MSG_NOSIGNAL);     // MSG_NOSIGNAL: 不触发SIGPIPE
    return sent > 0 ? 0 : -1;
}
