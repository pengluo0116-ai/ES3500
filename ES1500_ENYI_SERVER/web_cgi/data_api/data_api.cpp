/**
 * ES2000 数据 API CGI - 为 Web 前端提供 JSON 数据接口
 * 
 * 命令:
 *   ?cmd=list      返回所有电梯最新状态
 *   ?cmd=alarms    返回活跃报警列表
 *   ?cmd=history&id=6001  返回指定设备历史
 * 
 * 编译: g++ -std=c++11 -o data_api.cgi data_api.cpp -lsqlite3 -ljsoncpp
 */

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "json/json.h"
#include "sqlite3.h"

#define DB_PATH "/root/es2000/es2000.db"

// ============ 工具函数 ============

static void sendJson(const std::string& json) {
    std::cout << "Content-type: application/json; charset=utf-8\r\n";
    std::cout << "Access-Control-Allow-Origin: *\r\n";
    std::cout << "\r\n";
    std::cout << json << std::endl;
}

static void sendError(const std::string& msg) {
    Json::Value resp;
    resp["error"] = 1;
    resp["msg"] = msg;
    Json::FastWriter writer;
    sendJson(writer.write(resp));
}

static std::string getQueryString() {
    const char* qs = getenv("QUERY_STRING");
    return qs ? std::string(qs) : "";
}

static std::string getParam(const std::string& qs, const std::string& key) {
    std::string search = key + "=";
    size_t pos = qs.find(search);
    if (pos == std::string::npos) return "";

    pos += search.length();
    size_t end = qs.find("&", pos);
    if (end == std::string::npos) end = qs.length();

    return qs.substr(pos, end - pos);
}

// ============ 数据库查询 ============

static sqlite3* openDB() {
    sqlite3* db = nullptr;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        return nullptr;
    }
    sqlite3_busy_timeout(db, 3000);
    return db;
}

// 查询所有电梯最新状态
static Json::Value queryAllStatus(sqlite3* db) {
    Json::Value result(Json::arrayValue);
    const char* sql =
        "SELECT device_id, floor_no, door, status, speed, "
        "temp, hum, pm2d5, pm10, people, maintain, "
        "ebike_cnt, run_num, run_time, grade, has_alarm, "
        "update_time, acc_x, acc_y, acc_z, amplitude "
        "FROM device_status ORDER BY device_id";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Json::Value dev;
        dev["device_id"]   = (const char*)sqlite3_column_text(stmt, 0);
        dev["floor_no"]    = (const char*)sqlite3_column_text(stmt, 1);
        dev["door"]        = (const char*)sqlite3_column_text(stmt, 2);
        dev["status"]      = (const char*)sqlite3_column_text(stmt, 3);
        dev["speed"]       = (const char*)sqlite3_column_text(stmt, 4);
        dev["temp"]        = (const char*)sqlite3_column_text(stmt, 5);
        dev["hum"]         = (const char*)sqlite3_column_text(stmt, 6);
        dev["pm2d5"]       = (const char*)sqlite3_column_text(stmt, 7);
        dev["pm10"]        = (const char*)sqlite3_column_text(stmt, 8);
        dev["people"]      = (const char*)sqlite3_column_text(stmt, 9);
        dev["maintain"]    = (const char*)sqlite3_column_text(stmt, 10);
        dev["ebike_cnt"]   = sqlite3_column_int(stmt, 11);
        dev["run_num"]     = sqlite3_column_int(stmt, 12);
        dev["run_time"]    = sqlite3_column_int(stmt, 13);
        dev["grade"]       = sqlite3_column_int(stmt, 14);
        dev["has_alarm"]   = sqlite3_column_int(stmt, 15);
        dev["update_time"] = (const char*)sqlite3_column_text(stmt, 16);
        dev["acc_x"]       = (const char*)sqlite3_column_text(stmt, 17);
        dev["acc_y"]       = (const char*)sqlite3_column_text(stmt, 18);
        dev["acc_z"]       = (const char*)sqlite3_column_text(stmt, 19);
        dev["amplitude"]   = (const char*)sqlite3_column_text(stmt, 20);

        result.append(dev);
    }

    sqlite3_finalize(stmt);
    return result;
}

// 查询活跃报警
static Json::Value queryAlarms(sqlite3* db) {
    Json::Value result(Json::arrayValue);
    const char* sql =
        "SELECT device_id, alarm_time, alarm_codes, alarm_desc "
        "FROM alarm_records WHERE is_active=1 "
        "ORDER BY alarm_time DESC LIMIT 50";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return result;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Json::Value alarm;
        alarm["device_id"]  = (const char*)sqlite3_column_text(stmt, 0);
        alarm["alarm_time"] = (const char*)sqlite3_column_text(stmt, 1);
        alarm["alarm_codes"]= (const char*)sqlite3_column_text(stmt, 2);
        alarm["alarm_desc"] = (const char*)sqlite3_column_text(stmt, 3);
        result.append(alarm);
    }

    sqlite3_finalize(stmt);
    return result;
}

// 查询历史
static Json::Value queryHistory(sqlite3* db, const std::string& device_id) {
    Json::Value result(Json::arrayValue);
    const char* sql =
        "SELECT device_id, json_data, create_time "
        "FROM device_history WHERE device_id=? "
        "ORDER BY create_time DESC LIMIT 100";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return result;

    sqlite3_bind_text(stmt, 1, device_id.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Json::Value rec;
        rec["device_id"]  = (const char*)sqlite3_column_text(stmt, 0);
        rec["create_time"]= (const char*)sqlite3_column_text(stmt, 2);
        result.append(rec);
    }

    sqlite3_finalize(stmt);
    return result;
}

// ============ 主入口 ============

int main() {
    std::string qs = getQueryString();
    std::string cmd = getParam(qs, "cmd");

    sqlite3* db = openDB();
    if (!db) {
        sendError("数据库连接失败");
        return 1;
    }

    Json::Value resp;
    Json::FastWriter writer;

    if (cmd == "list") {
        resp["error"] = 0;
        resp["data"] = queryAllStatus(db);
        sendJson(writer.write(resp));

    } else if (cmd == "alarms") {
        resp["error"] = 0;
        resp["data"] = queryAlarms(db);
        sendJson(writer.write(resp));

    } else if (cmd == "history") {
        std::string id = getParam(qs, "id");
        if (id.empty()) {
            sendError("缺少设备ID参数");
        } else {
            resp["error"] = 0;
            resp["device_id"] = id;
            resp["data"] = queryHistory(db, id);
            sendJson(writer.write(resp));
        }

    } else {
        // 默认：返回状态概览
        resp["error"] = 0;
        resp["total"] = queryAllStatus(db).size();
        resp["alarms"] = queryAlarms(db).size();
        resp["status"] = "ES2000 Data API v1.0";
        resp["usage"] = "?cmd=list|alarms|history&id=6001";
        sendJson(writer.write(resp));
    }

    sqlite3_close(db);
    return 0;
}
