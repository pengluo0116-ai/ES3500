/**
 * ES3500 SQLite 数据库管理器
 * 
 * 管理三张表:
 *   device_status  - 设备最新状态 (INSERT OR REPLACE)
 *   device_history - 原始JSON历史记录
 *   alarm_records  - 报警事件记录
 * 
 * WAL模式: 支持并发读写, 无需锁
 */
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include <vector>
#include "json_parser.h"

// 报警记录 (用于查询)
struct AlarmRecord {
    std::string device_id;          // 设备ID (如 "6001")
    std::string alarm_time;         // 报警时间 (yyyy-MM-dd HH:mm:ss)
    std::string alarm_codes;        // 故障代码列表, 逗号分隔
    std::string alarm_desc;         // 故障描述
    int         is_active;          // 1=进行中, 0=已解除
};

class DBManager {
public:
    explicit DBManager(const std::string& db_path);
    ~DBManager();

    int init();                                         // 打开DB + 建表 + 设置WAL

    int save(const ElevatorData& data,                  // 保存设备数据 + JSON历史
             const std::string& raw_json = "");

    int saveAlarm(const std::string& device_id,         // 保存报警记录
                  const AlarmResult& alarm);

    ElevatorData getLatest(const std::string& device_id);           // 查询单设备最新状态
    std::vector<ElevatorData> getAllLatest();                       // 查询全部设备
    std::vector<AlarmRecord> getActiveAlarms();                     // 查询进行中的报警

private:
    std::string m_db_path;      // 数据库文件路径
    void*       m_db;           // sqlite3* 句柄 (void* 避免头文件依赖)

    void exec(const std::string& sql);                  // 执行SQL (无返回值)
    void createTables();                                // 建表DDL
};

#endif
