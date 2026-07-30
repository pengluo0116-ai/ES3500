/**
 * 轻量级 HTTP Server - 仅处理POST请求
 * 
 * 基于 POSIX socket API, 无第三方依赖
 * 每个连接在独立线程中处理, 支持并发
 * 通过回调函数将请求体传递给业务逻辑
 */
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <functional>
#include <thread>

// HTTP回调: (请求体JSON, 响应体JSON) -> 0成功/非0失败
using HttpCallback = std::function<int(const std::string&, std::string&)>;

class HttpServer {
public:
    HttpServer(int port);
    ~HttpServer();

    void setCallback(HttpCallback cb);      // 设置业务回调
    int start();                            // 启动监听 (非阻塞, 在后台线程accept)
    void stop();                            // 停止服务

private:
    int  m_port;                            // 监听端口
    int  m_sockfd;                          // 监听socket fd
    bool m_running;                         // 运行标志
    HttpCallback m_callback;                // 业务回调函数
    std::thread m_thread;                   // accept循环线程

    void acceptLoop();                      // 主循环: accept → 新线程处理
    void handleClient(int client_fd);       // 处理单个连接: read→callback→send
    int  readRequest(int fd, std::string& method, std::string& path, std::string& body);
    int  sendResponse(int fd, const std::string& body);
};

#endif
