/**
 * JSON 解析器实现 - ES1500 协议 V2.2A
 */
#include "json_parser.h"
#include <sstream>

std::string JsonParser::safeGetString(const Json::Value& obj, const std::string& key) {
    if (obj.isMember(key) && !obj[key].isNull()) {
        return obj[key].asString();
    }
    return "";
}

int JsonParser::safeGetInt(const Json::Value& obj, const std::string& key) {
    if (obj.isMember(key) && !obj[key].isNull()) {
        return obj[key].asInt();
    }
    return 0;
}

int JsonParser::parse(const std::string& json_str, ElevatorData& data) {
    Json::Reader reader;
    Json::Value root;
    
    try {
        if (!reader.parse(json_str, root)) {
            return -1;
        }

        if (root.isMember("Count"))
            parseCount(root["Count"], data.count);
        if (root.isMember("Alarm"))
            parseAlarm(root["Alarm"], data.alarm);
        if (root.isMember("Parameters"))
            parseParams(root["Parameters"], data.param);
        if (root.isMember("State"))
            parseState(root["State"], data.state);

        return 0;
    } catch (...) {
        return -1;
    }
}

void JsonParser::parseCount(const Json::Value& root, CountData& count) {
    count.ebike_in_cnt    = safeGetInt(root, "EbikeInCnt");
    count.buckle_cnt      = safeGetInt(root, "BuckleCnt");
    count.door_opened_cnt = safeGetInt(root, "DoorOpenedCnt");
    count.grade           = safeGetInt(root, "Grade");
    count.run_num         = safeGetInt(root, "RunNum");
    count.run_distance    = safeGetInt(root, "RunDistance");
    count.run_time        = safeGetInt(root, "RunTime");
}

void JsonParser::parseAlarm(const Json::Value& root, AlarmData& alarm) {
    alarm.amplitude        = safeGetString(root, "Amplitude");
    alarm.down_alarm       = safeGetString(root, "DownAlarm");
    alarm.ebike            = safeGetString(root, "Ebike");
    alarm.manual_alarm     = safeGetString(root, "ManualAlarm");
    alarm.paused           = safeGetString(root, "Paused");
    alarm.person_trapped   = safeGetString(root, "PersonTrapped");
    alarm.run_door         = safeGetString(root, "RunDoor");
    alarm.slant_fb         = safeGetString(root, "SlantFb");
    alarm.slant_lr         = safeGetString(root, "SlantLr");
    alarm.speed            = safeGetString(root, "Speed");
    alarm.stp_door_repeat  = safeGetString(root, "StpDoorRepeat");
    alarm.stp_door_unclose = safeGetString(root, "StpDoorUnClose");
    alarm.stp_door_unopen  = safeGetString(root, "StpDoorUnOpen");
    alarm.stuck            = safeGetString(root, "Stuck");
    alarm.up_alarm         = safeGetString(root, "UpAlarm");
    alarm.has_alarm        = safeGetString(root, "HasAlarm");
    alarm.safe_loop        = safeGetString(root, "SafeLoop");

    // 解析故障代码数组
    if (root.isMember("ErrorCode") && root["ErrorCode"].isArray()) {
        const Json::Value& codes = root["ErrorCode"];
        for (unsigned int i = 0; i < codes.size(); i++) {
            alarm.error_code.push_back(codes[i].asInt());
        }
    }
}

void JsonParser::parseParams(const Json::Value& root, ParamData& param) {
    param.id      = safeGetString(root, "Id");
    param.time    = safeGetString(root, "Time");
    param.version = safeGetString(root, "Version");
}

void JsonParser::parseState(const Json::Value& root, StateData& state) {
    state.amplitude = safeGetString(root, "Amplitude");
    state.degree_fb = safeGetString(root, "DegreeFb");
    state.degree_lr = safeGetString(root, "DegreeLr");
    state.door      = safeGetString(root, "Door");
    state.floor_no  = safeGetString(root, "Floor");
    state.acc_x     = safeGetString(root, "AccX");
    state.acc_y     = safeGetString(root, "AccY");
    state.acc_z     = safeGetString(root, "AccZ");
    state.hum       = safeGetString(root, "Hum");
    state.maintain  = safeGetString(root, "Maintain");
    state.people    = safeGetString(root, "People");
    state.speed     = safeGetString(root, "Speed");
    state.temp      = safeGetString(root, "Temp");
    state.pm2d5     = safeGetString(root, "Pm2d5");
    state.pm10      = safeGetString(root, "Pm10");
    state.status    = safeGetString(root, "Status");
}
