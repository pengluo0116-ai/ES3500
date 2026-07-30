/**
 * 报警检查器 - 基于 ES1500 协议 V2.2A
 * 
 * 检查逻辑:
 *   1. HasAlarm 标记 == "1"
 *   2. ErrorCode 数组非空
 *   3. 各报警子标记 (ebike/down_alarm/...) == "1"
 */
#ifndef ALARM_CHECKER_H
#define ALARM_CHECKER_H

#include "json_parser.h"

class AlarmChecker {
public:
    // 检查电梯数据中的报警
    static AlarmResult check(const ElevatorData& data);

private:
    // 检查故障代码数组
    static void checkErrorCodes(const AlarmData& alarm, AlarmResult& result);
    // 逐项检查报警标记
    static void checkStateAlarms(const AlarmData& alarm, const StateData& state,
                                 AlarmResult& result);
    // 故障代码 → 中文描述映射
    static const char* getErrorDescription(int code);
};

#endif
