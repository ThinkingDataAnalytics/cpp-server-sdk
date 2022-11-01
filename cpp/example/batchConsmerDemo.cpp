//
// Created by wwango on 2022/10/25.
//

#include "../include/ThinkingAnalyticsAPI.h"
#include "TABatchConsumer.h"

using namespace TaSDK;

TaSDK::ThinkingDataAnalytics *p_tga;

int shushuInit() {

    string accountId = "wangdaji";
    string distincId = "wangdaji_distincid";

    string eventName;

    //上传事件
    TaSDK::PropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");;
    event_properties.SetNumber("test_number_int", 3);
    event_properties.SetBool("test_bool", true);
    std::vector<std::string> list;
    list.push_back("item1");
    list.push_back("item2");
    event_properties.SetList("list", list);
    (*p_tga).track(accountId, distincId, "eventName", event_properties);

    // 对象
    PropertiesNode json;
    json.SetString("name", "value");;
    event_properties.SetObject("obj", json);

    // 对象组
    std::vector<TaSDK::TAJSONObject> list_objs;
    PropertiesNode json1;
    json1.SetString("name", "value1");;
    list_objs.push_back(json);
    list_objs.push_back(json1);
    event_properties.SetList("objs", list_objs);

    (*p_tga).track(accountId, distincId, "eventName", event_properties);

    (*p_tga).flush();

    // 首次事件
    TaSDK::PropertiesNode properties;
    eventName = "firstEvent";
    properties.SetString("#first_check_id", "first_check_id");
    (*p_tga).track_first(accountId, distincId, eventName, properties);

    // 可更新事件
    eventName = "updateEvent";
    string updateEventId = "1242423423234";
    TaSDK::PropertiesNode update_event_properties;
    update_event_properties.SetString("price", "100");
    update_event_properties.SetBool("status", 0);
    (*p_tga).track_update(accountId, distincId, eventName, updateEventId, update_event_properties);

    // 可重写事件
    eventName = "overWriteEvent";
    string overWriteEventId = "overWrite_001";
    TaSDK::PropertiesNode overWrite_event_properties;
    overWrite_event_properties.SetString("money", "99");
    overWrite_event_properties.SetString("code", "10");
    (*p_tga).track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_properties);

    (*p_tga).flush();

/********************************用户属性****************************************/
    // user_set
    TaSDK::PropertiesNode userSet_properties;
    userSet_properties.SetString("user_set", "value");
    //上传用户属性
    (*p_tga).user_set(accountId, distincId, userSet_properties);

    // user_setOnce
    TaSDK::PropertiesNode userSetOnce_properties;
    userSetOnce_properties.SetString("user_setOnce", "value");
    (*p_tga).user_setOnce(accountId, distincId, userSetOnce_properties);

    // user_add
    TaSDK::PropertiesNode userAdd_properties;
    userAdd_properties.SetNumber("user_add", 66);
    (*p_tga).user_add(accountId, distincId, userAdd_properties);

    // user_append
    TaSDK::PropertiesNode userAppend_properties;
    vector<string> userAppenListValue;
    userAppenListValue.push_back("11");
    userAppend_properties.SetList("user_append", userAppenListValue);
    (*p_tga).user_append(accountId, distincId, userAppend_properties);

    // user_uniq_append
    TaSDK::PropertiesNode userUniqAppend_properties;
    vector<string> userUniqAppenListValue;
    userUniqAppenListValue.push_back("55");
    // 为arr1的数组类型去重追加属性
    userUniqAppend_properties.SetList("user_uniqAppend", userUniqAppenListValue);
    (*p_tga).user_uniqAppend(accountId, distincId, userUniqAppend_properties);

    // user_unset
    TaSDK::PropertiesNode userUnset_properties;
    userUnset_properties.SetNumber("user_unset", 0);
    // 清空该用户user_name属性值
    (*p_tga).user_unset(accountId, distincId, userUnset_properties);


    // user_del
    // (*p_tga).user_del(accountId, distincId);

    (*p_tga).flush();

    return 1;
}

int main(int argc, char *argv[]) {

    TABatchConsumer batchConsumer = TABatchConsumer("1b1c1fef65e3482bad5c9d0e6a823356","http://receiver.ta.thinkingdata.cn", 10);
    ThinkingDataAnalytics tga = ThinkingDataAnalytics(batchConsumer, true);

    // 设置公共属性
    TaSDK::PropertiesNode superProperties;
    superProperties.SetString("super_kkk", "super_vvv");
    tga.setSupperProperties(superProperties);

    p_tga = &tga;

    shushuInit();

    return 0;
}
