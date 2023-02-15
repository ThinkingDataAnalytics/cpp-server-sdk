//
// Created by wwango on 2022/5/12.
//

#include <iostream>

#include "../include/ThinkingAnalyticsAPI.h"
#include "../include/TALoggerConsumer.h"

#include <pthread.h>
#include <thread>

using namespace std;
using namespace TaSDK;

int main(int argc, char *argv[]) {

    // Init SDK
    static LoggerConsumer::Config config = LoggerConsumer::Config("/Users/charles/Desktop/cpp_sdk_log");
    config.fileNamePrefix = "YYDS";
    config.rotateMode = LoggerConsumer::HOURLY;
    config.fileSize = 1;

    LoggerConsumer logConsumer = LoggerConsumer(config);
    ThinkingDataAnalytics tga = ThinkingDataAnalytics(logConsumer, true);
    ThinkingDataAnalytics &p_tga = tga;

    // setSuperProperties
    PropertiesNode superProperties;
    superProperties.SetString("test_superProperties", "test_super");
    p_tga.setSupperProperties(superProperties);


    // id info
    string accountId = "shp";
    string distincId = "shp666";
    string eventName = "track_envet";


    PropertiesNode _properties;
    _properties.SetString("name1", "XZ_debug");
    _properties.SetString("name2", "logbugs");
    _properties.SetString("name3", "name3");
    _properties.SetString("#uuid", "1234567890");
    _properties.SetNumber("test_number_int", 3);
    _properties.SetNumber("test_number_double", 3.14);
    _properties.SetBool("test_bool", true);
    std::string test_string1 = "string1";
    _properties.SetString("test_stl_string1", test_string1);
    timeb t1 = {};
    ftime(&t1);
    _properties.SetDateTime("time2", t1.time, t1.millitm);
    std::vector<std::string> list;
    list.push_back("item11");
    list.push_back("item21");
    _properties.SetList("test_list1", list);

    PropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");
    event_properties.SetString("name2", "logbugs");
    event_properties.SetString("name3", "name3");
    event_properties.SetString("#uuid", "1234567890");
    event_properties.SetNumber("test_number_int", 3);
    event_properties.SetNumber("test_number_double", 3.14);
    event_properties.SetBool("test_bool", true);
    std::string test_string = "string1";
    event_properties.SetString("test_stl_string1", test_string);
    timeb t = {};
    ftime(&t);
    event_properties.SetDateTime("time11", t.time, t.millitm);
    std::vector<std::string> test_list;
    test_list.push_back("item11");
    test_list.push_back("item21");
    event_properties.SetList("test_list1", test_list);

    event_properties.SetObject("obj", _properties);

    std::vector<TAJSONObject> list_objs;
    list_objs.push_back(_properties);
    list_objs.push_back(_properties);
    event_properties.SetList("objs", list_objs);
    
    p_tga.track(accountId, distincId, eventName, event_properties);

    eventName = "first_envet";
    event_properties.SetString("#first_check_id", "first_event");
    p_tga.track_first(accountId, distincId, eventName, event_properties);

    p_tga.clearSuperProperties();


    eventName = "update_event";
    string updateEventId = "update_001";
    PropertiesNode update_event_properties;
    update_event_properties.SetString("price", "100");
    update_event_properties.SetBool("status", 0);
    p_tga.track_update(accountId, distincId, eventName, updateEventId, update_event_properties);

    PropertiesNode update_event_new_properties;
    update_event_new_properties.SetBool("status", 1);
    p_tga.track_update(accountId, distincId, eventName, updateEventId, update_event_new_properties);


    eventName = "overWrite_event";
    string overWriteEventId = "overWrite_001";
    PropertiesNode overWrite_event_properties;
    overWrite_event_properties.SetString("money", "99");
    overWrite_event_properties.SetString("code", "10");
    p_tga.track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_properties);

    PropertiesNode overWrite_event_new_properties;
    overWrite_event_new_properties.SetString("money", "66");
    p_tga.track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_new_properties);


    // user_set
    PropertiesNode userSet_properties;
    userSet_properties.SetString("name1", "test1");
    userSet_properties.SetString("name2", "test2");
    p_tga.user_set(accountId, distincId, userSet_properties);

    userSet_properties.Clear();
    userSet_properties.SetString("user_name", "123");
    p_tga.user_set(accountId, distincId, userSet_properties);

    // user_setOnce
    PropertiesNode userSetOnce_properties;
    userSetOnce_properties.SetString("user_one_name", "ABC");
    p_tga.user_setOnce(accountId, distincId, userSetOnce_properties);

    userSetOnce_properties.Clear();
    userSetOnce_properties.SetString("user_one_name", "xyz");
    userSetOnce_properties.SetString("user_age", "18");
    p_tga.user_setOnce(accountId, distincId, userSetOnce_properties);


    // user_add
    PropertiesNode userAdd_properties;
    userAdd_properties.SetNumber("cash", 66);
    p_tga.user_add(accountId, distincId, userAdd_properties);

    userAdd_properties.Clear();
    userAdd_properties.SetNumber("cash", 33);
    p_tga.user_add(accountId, distincId, userAdd_properties);


    // user_append
    PropertiesNode userAppend_properties;
    vector<string> userAppenListValue;
    userAppenListValue.push_back("11");
    userAppenListValue.push_back("33");
    userAppend_properties.SetList("arr1", userAppenListValue);
    p_tga.user_append(accountId, distincId, userAppend_properties);

    // user_uniq_append
    PropertiesNode userUniqAppend_properties;
    vector<string> userUniqAppenListValue;
    userUniqAppenListValue.push_back("55");
    userUniqAppenListValue.push_back("22");
    userUniqAppenListValue.push_back("33");
    userUniqAppenListValue.push_back("66");
    userUniqAppenListValue.push_back("55");
    userUniqAppend_properties.SetList("arr1", userUniqAppenListValue);
    p_tga.user_uniqAppend(accountId, distincId, userUniqAppend_properties);

    // user_unset
    PropertiesNode userUnset_properties;
    userUnset_properties.SetNumber("user_name", 0);
    p_tga.user_unset(accountId, distincId, userUnset_properties);


    for (int count = 0; count <= 10000; count++) {
        PropertiesNode userAdd_properties;
        userAdd_properties.SetNumber("cash", count);
        p_tga.user_add(accountId, distincId, userAdd_properties);
    }
    // user_del
//    p_tga.user_del(accountId, distincId);

    p_tga.flush();

    return 0;
}

