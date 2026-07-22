/**
 * ES2000 电梯报警主机 - 数据采集管理进程
 * 
 * 功能：HTTP Server 接收 STM32+7628 网关上报的电梯数据
 *      解析 JSON → 存储 SQLite → 报警判断
 * 协议：ES1500 电梯数据接口 V2.2A
 * 编译：g++ -std=c++11 -o collector *.cpp -lsqlite3 -ljsoncpp -lpthread
 */

#include "http_server.h"
#include "db_manager.h"
#include "json_parser.h"
#include "alarm_checker.h"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

#define COLLECTOR_PORT  9009
#define DB_PATH         "/root/es2000/es2000.db"
#define LOG_PATH        "/root/es2000/collector.log"

static volatile int g_running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        g_running = 0;
    }
}

int main() {
    std::cout << "=== ES2000 数据采集进程 v1.0 ===" << std::endl;
    std::cout << "监听端口: " << COLLECTOR_PORT << std::endl;
    std::cout << "数据库路径: " << DB_PATH << std::endl;

    // 注册信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    // 初始化数据库
    DBManager db(DB_PATH);
    if (db.init() != 0) {
        std::cerr << "[ERROR] 数据库初始化失败" << std::endl;
        return -1;
    }
    std::cout << "[OK] 数据库初始化完成" << std::endl;

    // 启动 HTTP 服务器
    HttpServer server(COLLECTOR_PORT);
    server.setCallback([&db](const std::string& json_str, std::string& response) {
        // 1. 解析 JSON
        ElevatorData data;
        int ret = JsonParser::parse(json_str, data);
        if (ret != 0) {
            response = "{\"error\":1,\"msg\":\"JSON parse error\"}";
            return -1;
        }

        // 2. 存储数据库
        ret = db.save(data);
        if (ret != 0) {
            response = "{\"error\":2,\"msg\":\"DB save error\"}";
            return -2;
        }

        // 3. 报警判断
        AlarmResult alarm = AlarmChecker::check(data);

        // 4. 记录报警
        if (alarm.has_alarm) {
            db.saveAlarm(data.id, alarm);
            std::cout << "[ALARM] 设备 " << data.id 
                      << " 报警: " << alarm.codes << std::endl;
        }

        // 5. 返回响应
        response = "{\"error\":0,\"alarm\":" + (alarm.has_alarm ? "1" : "0") + "}";
        return 0;
    });

    if (server.start() != 0) {
        std::cerr << "[ERROR] HTTP 服务启动失败" << std::endl;
        return -2;
    }
    std::cout << "[OK] HTTP 服务已启动" << std::endl;

    // 主循环等待
    while (g_running) {
        sleep(1);
    }

    std::cout << "[INFO] 正在关闭..." << std::endl;
    server.stop();
    std::cout << "[OK] 已关闭" << std::endl;

    return 0;
}
