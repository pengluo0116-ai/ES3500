/**
 * 轻量级 HTTP Server - 接收网关 POST 数据
 * 基于 POSIX socket，无外部依赖
 */
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <functional>
#include <thread>

// 回调: (请求体JSON, 响应体) -> 返回码
using HttpCallback = std::function<int(const std::string&, std::string&)>;

class HttpServer {
public:
    HttpServer(int port);
    ~HttpServer();

    void setCallback(HttpCallback cb);
    int start();
    void stop();

private:
    int  m_port;
    int  m_sockfd;
    bool m_running;
    HttpCallback m_callback;
    std::thread m_thread;

    void acceptLoop();
    void handleClient(int client_fd);
    int  readRequest(int fd, std::string& method, std::string& path, std::string& body);
    int  sendResponse(int fd, const std::string& body);
};

#endif
