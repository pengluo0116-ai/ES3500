/**
 * SQLite3 数据库管理器实现
 * 
 * 注意:
 *   - SQL使用snprintf拼接字符串, 需对用户数据做单引号转义
 *   - sqlite3.c 以 .o 静态链接, 无需系统安装 libsqlite3
 *   - WAL模式: journal_mode=WAL, synchronous=NORMAL (性能与安全平衡)
 */
#include "db_manager.h"
#include "sqlite/sqlite3.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

DBManager::DBManager(const std::string& db_path)
    : m_db_path(db_path), m_db(nullptr) {}

DBManager::~DBManager() {
    if (m_db) sqlite3_close((sqlite3*)m_db);
}

int DBManager::init() {
    int rc = sqlite3_open(m_db_path.c_str(), (sqlite3**)&m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "[DB] Open failed: " << sqlite3_errmsg((sqlite3*)m_db) << std::endl;
        return -1;
    }
    exec("PRAGMA journal_mode=WAL;");              // WAL模式: 支持并发读写
    exec("PRAGMA synchronous=NORMAL;");             // 安全性: FULL→NORMAL 提升性能
    createTables();
    return 0;
}

void DBManager::createTables() {
    // 设备状态表: 每设备一行, INSERT OR REPLACE 更新
    exec(
        "CREATE TABLE IF NOT EXISTS device_status ("
        " device_id TEXT PRIMARY KEY, floor_no TEXT, door TEXT, status TEXT,"
        " speed TEXT, temp TEXT, hum TEXT, pm2d5 TEXT, pm10 TEXT,"
        " acc_x TEXT, acc_y TEXT, acc_z TEXT, amplitude TEXT, degree_fb TEXT, degree_lr TEXT,"
        " people TEXT, maintain TEXT, ebike_cnt INTEGER DEFAULT 0,"
        " run_num INTEGER DEFAULT 0, run_time INTEGER DEFAULT 0, grade INTEGER DEFAULT 0,"
        " has_alarm INTEGER DEFAULT 0, update_time TEXT);"
    );
    // 历史记录表: 每次上报追加一条
    exec(
        "CREATE TABLE IF NOT EXISTS device_history ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT, device_id TEXT,"
        " json_data TEXT, create_time TEXT);"
    );
    // 报警记录表
    exec(
        "CREATE TABLE IF NOT EXISTS alarm_records ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT, device_id TEXT,"
        " alarm_time TEXT, alarm_codes TEXT, alarm_desc TEXT, is_active INTEGER DEFAULT 1);"
    );
    exec("CREATE INDEX IF NOT EXISTS idx_hist ON device_history(device_id, create_time);");
    exec("CREATE INDEX IF NOT EXISTS idx_alarm ON alarm_records(device_id, alarm_time);");
}

void DBManager::exec(const std::string& sql) {
    std::lock_guard<std::mutex> lk(m_mutex);
    char* err = nullptr;
    int rc = sqlite3_exec((sqlite3*)m_db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK && err) {
        std::cerr << "[DB] Error: " << err << std::endl;
        sqlite3_free(err);
    }
}

// SQL字符串转义: 单引号 → 两个单引号 (SQLite标准)
static std::string escape_sql(const std::string& s) {
    std::string r;
    r.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '\'') r += "''";
        else r += c;
    }
    return r;
}

int DBManager::save(const ElevatorData& data, const std::string& raw_json) {
    const auto& s = data.state;
    const auto& c = data.count;
    const auto& a = data.alarm;

    // 获取当前时间字符串
    time_t now = time(nullptr);
    char tb[32];
    strftime(tb, sizeof(tb), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // 拼接SQL (使用转义后的字符串值)
    char sql[4096];
    snprintf(sql, sizeof(sql),
        "INSERT OR REPLACE INTO device_status (device_id,floor_no,door,status,speed,"
        "temp,hum,pm2d5,pm10,acc_x,acc_y,acc_z,amplitude,degree_fb,degree_lr,"
        "people,maintain,ebike_cnt,run_num,run_time,grade,has_alarm,update_time) "
        "VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s',"
        "'%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,'%s');",
        data.param.id.c_str(),
        s.floor_no.c_str(), s.door.c_str(), s.status.c_str(), s.speed.c_str(),
        s.temp.c_str(), s.hum.c_str(), s.pm2d5.c_str(), s.pm10.c_str(),
        s.acc_x.c_str(), s.acc_y.c_str(), s.acc_z.c_str(),
        s.amplitude.c_str(), s.degree_fb.c_str(), s.degree_lr.c_str(),
        s.people.c_str(), s.maintain.c_str(),
        c.ebike_in_cnt, c.run_num, c.run_time, c.grade,
        (a.has_alarm == "1" ? 1 : 0), tb);
    exec(sql);

    // 保存原始JSON到历史表 (用于追溯)
    if (!raw_json.empty()) {
        std::string esc = escape_sql(raw_json);
        char hs[8192];
        snprintf(hs, sizeof(hs),
            "INSERT INTO device_history (device_id,json_data,create_time) VALUES ('%s','%s','%s');",
            data.param.id.c_str(), esc.c_str(), tb);
        exec(hs);
    }
    return 0;
}

int DBManager::saveAlarm(const std::string& device_id, const AlarmResult& alarm) {
    time_t now = time(nullptr);
    char tb[32];
    strftime(tb, sizeof(tb), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char sql[4096];
    snprintf(sql, sizeof(sql),
        "INSERT INTO alarm_records (device_id,alarm_time,alarm_codes,alarm_desc,is_active) "
        "VALUES ('%s','%s','%s','%s',1);",
        device_id.c_str(), tb, alarm.codes.c_str(), alarm.descriptions.c_str());
    exec(sql);
    return 0;
}

// TODO: 以下查询函数为桩代码, 后续实现
ElevatorData DBManager::getLatest(const std::string&) { return ElevatorData(); }
std::vector<ElevatorData> DBManager::getAllLatest() { return {}; }
std::vector<AlarmRecord> DBManager::getActiveAlarms() { return {}; }
