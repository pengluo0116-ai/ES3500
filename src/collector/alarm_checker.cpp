/**
 * 报警检查器实现 - ES1500 V2.2A
 * 
 * 报警类型共15种:
 *   通用: HasAlarm标记, ErrorCode故障码
 *   特种设备安全: 蹲底/冲顶/超速/卡层/开门走梯
 *   门系统: 重复开关门/停梯不关门/停梯不开门
 *   运行状态: 长时间未运行/电动车入梯/困人/手动报警
 *   姿态: 前后倾斜/左右倾斜/震动
 *   其他: 安全回路
 */
#include "alarm_checker.h"
#include <sstream>

AlarmResult AlarmChecker::check(const ElevatorData& data) {
    AlarmResult result;
    result.has_alarm = false;

    // ① HasAlarm 综合标记
    if (data.alarm.has_alarm == "1") {
        result.has_alarm = true;
    }

    // ② ErrorCode 故障码
    checkErrorCodes(data.alarm, result);

    // ③ 逐项检查状态报警标记
    checkStateAlarms(data.alarm, data.state, result);

    return result;
}

void AlarmChecker::checkErrorCodes(const AlarmData& alarm, AlarmResult& result) {
    if (alarm.error_code.empty()) return;

    result.has_alarm = true;
    std::ostringstream codes_ss, desc_ss;

    for (size_t i = 0; i < alarm.error_code.size(); i++) {
        int code = alarm.error_code[i];
        if (i > 0) { codes_ss << ","; desc_ss << "; "; }
        codes_ss << code;
        desc_ss << getErrorDescription(code);
    }

    if (!result.codes.empty()) {
        result.codes += "," + codes_ss.str();
        result.descriptions += "; " + desc_ss.str();
    } else {
        result.codes = codes_ss.str();
        result.descriptions = desc_ss.str();
    }
}

void AlarmChecker::checkStateAlarms(const AlarmData& alarm, const StateData& state,
                                     AlarmResult& result) {
    // 报警标记 → 中文名称映射表
    struct { const std::string& flag; const char* name; } checks[] = {
        {alarm.down_alarm,        "蹲底"},
        {alarm.ebike,             "电动车入梯"},
        {alarm.manual_alarm,      "手动报警"},
        {alarm.paused,            "长时间未运行"},
        {alarm.person_trapped,    "困人"},
        {alarm.run_door,          "开门走梯"},
        {alarm.slant_fb,          "前后倾斜"},
        {alarm.slant_lr,          "左右倾斜"},
        {alarm.speed,             "超速"},
        {alarm.stp_door_repeat,   "重复开关门"},
        {alarm.stp_door_unclose,  "停梯不关门"},
        {alarm.stp_door_unopen,   "停梯不开门"},
        {alarm.stuck,             "卡层"},
        {alarm.up_alarm,          "冲顶"},
        {alarm.safe_loop,         "安全回路"},
    };

    for (auto& c : checks) {
        if (c.flag == "1") {
            result.has_alarm = true;
            if (!result.descriptions.empty()) result.descriptions += "; ";
            result.descriptions += c.name;
        }
    }
}

// 故障代码 → 中文描述 (ES1500协议定义)
const char* AlarmChecker::getErrorDescription(int code) {
    switch (code) {
        // 机房监测主机
        case 1:   return "机房监测主机设备故障";

        // 平层传感器 (101-106)
        case 101: return "磁条缺失或未对准";
        case 102: return "设备未校准或楼层配置错误";
        case 103: return "平层传感器处于磁条边缘";
        case 104: return "平层传感器故障或电梯故障";
        case 105: return "平层传感器故障";
        case 106: return "楼层配置错误";

        // 开关门 (201-206)
        case 201: return "开关门传感器损坏或未安装";
        case 202: return "开关门传感器安装位置错误";
        case 203: return "该层开关门传感器感应到外门物体";
        case 204: return "停梯不开门时间设置过短";
        case 205: return "重复开关门";
        case 206: return "关门故障";

        // 人体温湿度三合一 (301)
        case 301: return "人体温湿度三合一传感器故障";

        // 姿态传感器 (401)
        case 401: return "姿态传感器故障";

        // 电流检测 (501)
        case 501: return "电流检测模块故障";

        // 温湿度 (601)
        case 601: return "温湿度传感器故障";

        // 极限传感器 (701)
        case 701: return "极限磁条安装位置有问题";

        default:  return "未知故障";
    }
}
