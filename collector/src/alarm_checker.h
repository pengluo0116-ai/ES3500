/**
 * 报警检查器 - 基于 ES1500 协议 V2.2A
 */
#ifndef ALARM_CHECKER_H
#define ALARM_CHECKER_H

#include "json_parser.h"

class AlarmChecker {
public:
    static AlarmResult check(const ElevatorData& data);

private:
    static void checkErrorCodes(const AlarmData& alarm, AlarmResult& result);
    static void checkStateAlarms(const AlarmData& alarm, const StateData& state, AlarmResult& result);
    static const char* getErrorDescription(int code);
};

#endif
