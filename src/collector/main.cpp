/**
 * ES3500 电梯报警主机 - 数据采集进程 (main入口)
 * 
 * 功能: HTTP Server 接收网关(STM32+7628)上报的电梯运行数据
 * 流程: JSON解析 -> SQLite存储 -> 报警判断 -> 返回响应
 * 协议: ES1500 电梯数据接口 V2.2A
 * 
 * 编译: 见 .github/workflows/cross-compile.yml (armhf 32位交叉编译)
 * 部署: USB/install.sh 一键安装到 /root/es3500/bin/collector
 * 测试: curl -X POST http://127.0.0.1:9009/ -d '{"Parameters":{"Id":"6001"},"State":{"Floor":"5"}}'
 */
#include "http_server.h"
#include "db_manager.h"
#include "json_parser.h"
#include "alarm_checker.h"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

#define COLLECTOR_PORT  9009                        // HTTP监听端口
#define DB_PATH         "/root/es3500/es3500.db"    // SQLite数据库路径

static volatile int g_running = 1;                  // 全局运行标志(signal-safe)

// 信号处理: 收到SIGINT(Ctrl+C)或SIGTERM(systemctl stop)时安全退出
static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM)
        g_running = 0;
}

int main() {
    std::cout << "=== ES3500 Collector v1.0 ===" << std::endl;
    std::cout << "Port: " << COLLECTOR_PORT << std::endl;
    std::cout << "DB: " << DB_PATH << std::endl;

    // 注册信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);                       // 防止写已关闭socket时SIGPIPE崩溃

    // ① 初始化数据库 (建表/建索引)
    DBManager db(DB_PATH);
    if (db.init() != 0) {
        std::cerr << "[ERROR] DB init failed" << std::endl;
        return -1;
    }
    std::cout << "[OK] DB ready" << std::endl;

    // ② 创建HTTP服务器 + 注册请求回调 (lambda捕获db引用)
    HttpServer server(COLLECTOR_PORT);
    server.setCallback([&db](const std::string& json_str, std::string& response) {
        // 2a. JSON解析 → ElevatorData结构体
        ElevatorData data;
        if (JsonParser::parse(json_str, data) != 0) {
            response = "{\"error\":1,\"msg\":\"JSON parse error\"}";
            return -1;
        }

        // 2b. 写入SQLite (设备状态表 + 原始JSON历史表)
        db.save(data, json_str);

        // 2c. 报警判断 (故障码 + 各项报警标记)
        AlarmResult alarm = AlarmChecker::check(data);
        if (alarm.has_alarm) {
            db.saveAlarm(data.param.id, alarm);
            std::cout << "[ALARM] " << data.param.id 
                      << " codes:" << alarm.codes << std::endl;
        }

        // 2d. 返回JSON响应 {"error":0,"alarm":0|1}
        response = std::string("{\"error\":0,\"alarm\":") 
                 + (alarm.has_alarm ? "1" : "0") + "}";
        return 0;
    });

    // ③ 启动HTTP服务 (在后台线程accept连接)
    if (server.start() != 0) {
        std::cerr << "[ERROR] HTTP start failed" << std::endl;
        return -2;
    }
    std::cout << "[OK] Listening on :" << COLLECTOR_PORT << std::endl;

    // ④ 主循环等待终止信号
    while (g_running) sleep(1);

    // ⑤ 优雅关闭
    std::cout << "[INFO] Shutting down..." << std::endl;
    server.stop();
    std::cout << "[OK] Stopped" << std::endl;
    return 0;
}
