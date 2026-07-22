/**
 * JSON 解析器 - ES1500 电梯数据接口 V2.2A
 */
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include <cstring>
#include "json/json.h"

// ==================== 数据结构 ====================

/* 累计数据 */
struct CountData {
    int ebike_in_cnt    = 0;    // 电动车进梯次数
    int buckle_cnt      = 0;    // 钢丝绳弯折次数
    int door_opened_cnt = 0;    // 开门次数
    int grade           = 0;    // 健康评分
    int run_num         = 0;    // 运行次数
    int run_distance    = 0;    // 运行距离
    int run_time        = 0;    // 运行时间
};

/* 报警数据 */
struct AlarmData {
    std::string amplitude;      // 震动
    std::vector<int> error_code; // 故障代码列表
    std::string down_alarm;     // 蹲底
    std::string ebike;          // 电动车入梯
    std::string manual_alarm;   // 手动报警
    std::string paused;         // 长时间未运行
    std::string person_trapped; // 困人
    std::string run_door;       // 开门走梯
    std::string slant_fb;       // 前后倾斜
    std::string slant_lr;       // 左右倾斜
    std::string speed;          // 超速
    std::string stp_door_repeat;// 重复开关门
    std::string stp_door_unclose;// 停梯不关门
    std::string stp_door_unopen; // 停梯不开门
    std::string stuck;          // 卡层
    std::string up_alarm;       // 冲顶
    std::string has_alarm;      // 有无报警
    std::string safe_loop;      // 安全回路
};

/* 设备参数 */
struct ParamData {
    std::string id;             // 设备ID
    std::string time;           // 时间戳
    std::string version;        // 协议版本
};

/* 实时状态 */
struct StateData {
    std::string amplitude;      // 振幅(g)
    std::string degree_fb;      // 前后偏移角度
    std::string degree_lr;      // 左右偏移角度
    std::string door;           // 开关门状态 0:关门 1:开门 ...
    std::string floor_no;       // 楼层
    std::string acc_x;          // X轴加速度(g)
    std::string acc_y;          // Y轴加速度(g)
    std::string acc_z;          // Z轴加速度(g)
    std::string hum;            // 湿度
    std::string maintain;       // 维保标志
    std::string people;         // 有人/无人
    std::string speed;          // 速度(m/s)
    std::string temp;           // 温度(℃)
    std::string pm2d5;          // PM2.5
    std::string pm10;           // PM10
    std::string status;         // 上下行: 0停止 1上 2下
};

/* 完整电梯数据 */
struct ElevatorData {
    CountData   count;
    AlarmData   alarm;
    ParamData   param;
    StateData   state;
};

/* 报警结果 */
struct AlarmResult {
    bool        has_alarm = false;
    std::string codes;          // 报警代码列表，逗号分隔
    std::string descriptions;   // 报警描述
};

// ==================== JSON 解析器 ====================

class JsonParser {
public:
    /**
     * 解析 ES1500 JSON 协议数据
     * @param json_str  原始 JSON 字符串
     * @param data      输出的电梯数据
     * @return 0成功, -1失败
     */
    static int parse(const std::string& json_str, ElevatorData& data);

private:
    static void parseCount(const Json::Value& root, CountData& count);
    static void parseAlarm(const Json::Value& root, AlarmData& alarm);
    static void parseParams(const Json::Value& root, ParamData& param);
    static void parseState(const Json::Value& root, StateData& state);
    
    static std::string safeGetString(const Json::Value& obj, const std::string& key);
    static int safeGetInt(const Json::Value& obj, const std::string& key);
};

#endif
