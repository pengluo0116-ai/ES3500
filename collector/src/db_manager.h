/**
 * SQLite 数据库管理
 */
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include <vector>
#include "json_parser.h"

// 报警记录
struct AlarmRecord {
    std::string device_id;
    std::string alarm_time;
    std::string alarm_codes;
    std::string alarm_desc;
    int         is_active;  // 1=进行中, 0=已消除
};

class DBManager {
public:
    DBManager(const std::string& db_path);
    ~DBManager();

    int init();
    int save(const ElevatorData& data);
    int saveAlarm(const std::string& device_id, const AlarmResult& alarm);

    // 供 Qt 显示程序查询
    ElevatorData getLatest(const std::string& device_id);
    std::vector<ElevatorData> getAllLatest();
    std::vector<AlarmRecord>  getActiveAlarms();

private:
    std::string m_db_path;
    void*       m_db;  // sqlite3*

    void exec(const std::string& sql);
    void createTables();
};

#endif
