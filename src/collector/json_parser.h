/**
 * JSON 解析器 - ES1500 电梯数据接口 V2.2A
 * 
 * 协议字段说明 (网关上报JSON格式):
 * {
 *   "Count":      { "EbikeInCnt":..., "RunNum":..., ... },   // 累计统计
 *   "Alarm":      { "HasAlarm":"0", "ErrorCode":[...], ... }, // 报警标记
 *   "Parameters": { "Id":"6001", "Time":"...", ... },         // 设备参数
 *   "State":      { "Floor":"5", "Door":"0", ... }            // 实时状态
 * }
 */
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <vector>
#include <cstring>
#include "json/json.h"                      // jsoncpp头文件

// ==================== 数据结构 ====================

// 累计统计数据
struct CountData {
    int ebike_in_cnt    = 0;                // 电动车进梯次数
    int buckle_cnt      = 0;                // 钢丝绳弯折次数
    int door_opened_cnt = 0;                // 开门次数
    int grade           = 0;                // 健康评分(0-100)
    int run_num         = 0;                // 运行次数
    int run_distance    = 0;                // 运行距离(米)
    int run_time        = 0;                // 运行时间(秒)
};

// 报警标记 (值为字符串 "0"/"1")
struct AlarmData {
    std::string amplitude;                  // 震动报警
    std::vector<int> error_code;            // 故障代码列表
    std::string down_alarm;                 // 蹲底
    std::string ebike;                      // 电动车入梯
    std::string manual_alarm;               // 手动报警
    std::string paused;                     // 长时间未运行
    std::string person_trapped;             // 困人
    std::string run_door;                   // 开门走梯
    std::string slant_fb;                   // 前后倾斜
    std::string slant_lr;                   // 左右倾斜
    std::string speed;                      // 超速
    std::string stp_door_repeat;            // 重复开关门
    std::string stp_door_unclose;           // 停梯不关门
    std::string stp_door_unopen;            // 停梯不开门
    std::string stuck;                      // 卡层
    std::string up_alarm;                   // 冲顶
    std::string has_alarm;                  // 综合报警标记
    std::string safe_loop;                  // 安全回路
};

// 设备参数
struct ParamData {
    std::string id;                         // 设备ID (如 "6001")
    std::string time;                       // 上报时间戳
    std::string version;                    // 协议版本
};

// 实时状态
struct StateData {
    std::string amplitude;                  // 振幅(g)
    std::string degree_fb;                  // 前后偏移角度
    std::string degree_lr;                  // 左右偏移角度
    std::string door;                       // 门状态: 0关门 1开门 ...
    std::string floor_no;                   // 当前楼层
    std::string acc_x;                      // X轴加速度
    std::string acc_y;                      // Y轴加速度
    std::string acc_z;                      // Z轴加速度
    std::string hum;                        // 湿度(%)
    std::string maintain;                   // 维保标志
    std::string people;                     // 有人/无人
    std::string speed;                      // 速度(m/s)
    std::string temp;                       // 温度(℃)
    std::string pm2d5;                      // PM2.5
    std::string pm10;                       // PM10
    std::string status;                     // 上下行: 0停止 1上行 2下行
};

// 完整电梯数据 (聚合以上四个结构体)
struct ElevatorData {
    CountData   count;
    AlarmData   alarm;
    ParamData   param;
    StateData   state;
};

// 报警判断结果
struct AlarmResult {
    bool        has_alarm = false;          // 是否存在报警
    std::string codes;                      // 报警代码列表, 逗号分隔
    std::string descriptions;               // 报警描述
};

// ==================== JSON 解析器 ====================

class JsonParser {
public:
    // 解析 ES1500 JSON 协议数据 → ElevatorData
    // @return 0成功, -1失败 (格式错误或JSON不合法)
    static int parse(const std::string& json_str, ElevatorData& data);

private:
    static void parseCount(const Json::Value& root, CountData& count);
    static void parseAlarm(const Json::Value& root, AlarmData& alarm);
    static void parseParams(const Json::Value& root, ParamData& param);
    static void parseState(const Json::Value& root, StateData& state);

    // 安全取值: key不存在或为null时返回默认值
    static std::string safeGetString(const Json::Value& obj, const std::string& key);
    static int safeGetInt(const Json::Value& obj, const std::string& key);
};

#endif
