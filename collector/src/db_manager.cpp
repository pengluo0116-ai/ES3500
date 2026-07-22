/**
 * 数据库管理器实现
 * 表结构:
 *   device_status  - 设备最新状态 (每设备一行)
 *   device_history - 历史数据 (每次上报一条)
 *   alarm_records  - 报警记录
 */
#include "db_manager.h"
#include "sqlite3.h"
#include <sstream>
#include <iostream>
#include <ctime>

DBManager::DBManager(const std::string& db_path)
    : m_db_path(db_path), m_db(nullptr) {}

DBManager::~DBManager() {
    if (m_db) sqlite3_close((sqlite3*)m_db);
}

int DBManager::init() {
    int rc = sqlite3_open(m_db_path.c_str(), (sqlite3**)&m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] 打开失败: " << sqlite3_errmsg((sqlite3*)m_db) << std::endl;
        return -1;
    }

    // 启用 WAL 模式，提升并发性能
    exec("PRAGMA journal_mode=WAL;");
    exec("PRAGMA synchronous=NORMAL;");

    createTables();
    return 0;
}

void DBManager::createTables() {
    exec(
        "CREATE TABLE IF NOT EXISTS device_status ("
        "  device_id    TEXT PRIMARY KEY,"
        "  floor_no     TEXT,"
        "  door         TEXT,"
        "  status       TEXT,"
        "  speed        TEXT,"
        "  temp         TEXT,"
        "  hum          TEXT,"
        "  pm2d5        TEXT,"
        "  pm10         TEXT,"
        "  acc_x        TEXT,"
        "  acc_y        TEXT,"
        "  acc_z        TEXT,"
        "  amplitude    TEXT,"
        "  degree_fb    TEXT,"
        "  degree_lr    TEXT,"
        "  people       TEXT,"
        "  maintain     TEXT,"
        "  ebike_cnt    INTEGER DEFAULT 0,"
        "  run_num      INTEGER DEFAULT 0,"
        "  run_time     INTEGER DEFAULT 0,"
        "  grade        INTEGER DEFAULT 0,"
        "  has_alarm    INTEGER DEFAULT 0,"
        "  update_time  TEXT"
        ");"
    );

    exec(
        "CREATE TABLE IF NOT EXISTS device_history ("
        "  id           INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  device_id    TEXT,"
        "  json_data    TEXT,"
        "  create_time  TEXT"
        ");"
    );

    exec(
        "CREATE TABLE IF NOT EXISTS alarm_records ("
        "  id           INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  device_id    TEXT,"
        "  alarm_time   TEXT,"
        "  alarm_codes  TEXT,"
        "  alarm_desc   TEXT,"
        "  is_active    INTEGER DEFAULT 1"
        ");"
    );

    // 索引
    exec("CREATE INDEX IF NOT EXISTS idx_history_device ON device_history(device_id, create_time);");
    exec("CREATE INDEX IF NOT EXISTS idx_alarm_device ON alarm_records(device_id, alarm_time);");
}

void DBManager::exec(const std::string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec((sqlite3*)m_db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK && err) {
        std::cerr << "[DB] SQL错误: " << err << std::endl;
        sqlite3_free(err);
    }
}

int DBManager::save(const ElevatorData& data) {
    const auto& s = data.state;
    const auto& c = data.count;
    const auto& a = data.alarm;

    // 时间戳
    time_t now = time(nullptr);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    std::ostringstream sql;
    sql << "INSERT OR REPLACE INTO device_status ("
        << "device_id, floor_no, door, status, speed,"
        << "temp, hum, pm2d5, pm10,"
        << "acc_x, acc_y, acc_z, amplitude, degree_fb, degree_lr,"
        << "people, maintain, ebike_cnt, run_num, run_time, grade,"
        << "has_alarm, update_time"
        << ") VALUES ("
        << "'" << data.param.id << "',"
        << "'" << s.floor_no << "','" << s.door << "','" << s.status << "','" << s.speed << "',"
        << "'" << s.temp << "','" << s.hum << "','" << s.pm2d5 << "','" << s.pm10 << "',"
        << "'" << s.acc_x << "','" << s.acc_y << "','" << s.acc_z << "',"
        << "'" << s.amplitude << "','" << s.degree_fb << "','" << s.degree_lr << "',"
        << "'" << s.people << "','" << s.maintain << "',"
        << c.ebike_in_cnt << "," << c.run_num << "," << c.run_time << "," << c.grade << ","
        << (a.has_alarm == "1" ? 1 : 0) << ","
        << "'" << time_buf << "'"
        << ");";

    exec(sql.str());

    // 保存原始 JSON 历史
    // (此处简化，实际应传完整 JSON)
    std::ostringstream hist_sql;
    hist_sql << "INSERT INTO device_history (device_id, json_data, create_time) VALUES ("
             << "'" << data.param.id << "','','" << time_buf << "');";
    exec(hist_sql.str());

    return 0;
}

int DBManager::saveAlarm(const std::string& device_id, const AlarmResult& alarm) {
    time_t now = time(nullptr);
    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    std::ostringstream sql;
    sql << "INSERT INTO alarm_records (device_id, alarm_time, alarm_codes, alarm_desc, is_active) VALUES ("
        << "'" << device_id << "',"
        << "'" << time_buf << "',"
        << "'" << alarm.codes << "',"
        << "'" << alarm.descriptions << "',"
        << "1);";

    exec(sql.str());
    return 0;
}

ElevatorData DBManager::getLatest(const std::string& device_id) {
    ElevatorData data;
    // Qt 显示程序直接查询 SQLite，此处提供接口壳
    return data;
}

std::vector<ElevatorData> DBManager::getAllLatest() {
    std::vector<ElevatorData> result;
    return result;
}

std::vector<AlarmRecord> DBManager::getActiveAlarms() {
    std::vector<AlarmRecord> result;
    return result;
}
