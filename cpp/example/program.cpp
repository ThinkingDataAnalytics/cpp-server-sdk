#include <iostream>

#include "../include/TDAnalytics.h"
#include "../include/TDLoggerConsumer.h"
#include "../include/TDDebugConsumer.h"
#include "../include/TDBatchConsumer.h"

#include <thread>
#include <chrono>

using namespace thinkingDataAnalytics;

// enable debug
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

void asyncTrack1(TDAnalytics& te) {
    std::string accountId = "1";
    std::string eventName = "track_event";

    TDPropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");

    te.track(accountId, "", eventName, event_properties);
}

void asyncTrack2(TDAnalytics& te) {
    std::string accountId = "2";
    std::string eventName = "track_event";

    TDPropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");

    te.track(accountId, "", eventName, event_properties);
}

std::unique_ptr<TDConsumer> getLoggerConsumer() {
    TDLoggerConsumer::Config config = TDLoggerConsumer::Config("D:/log", 20, 500, TDLoggerConsumer::HOURLY);
    config.fileNamePrefix = "te";
    config.rotateMode = TDLoggerConsumer::HOURLY;
    std::unique_ptr<TDConsumer> ptr(new TDLoggerConsumer(config));
    return ptr;
}

std::unique_ptr<TDConsumer> getBatchConsumer() {
    std::unique_ptr<TDConsumer> ptr(new TDBatchConsumer("appId", "serverUrl", 20, true, "/test/cert.pem"));
    return ptr;
}

std::unique_ptr<TDConsumer> getDebugConsumer() {
    std::unique_ptr<TDConsumer> ptr(new TDDebugConsumer("appId", "serverUrl", "", "123456789"));
    return ptr;
}



int main(int argc, char *argv[]) {

#ifdef _DEBUG
    // enable debug. Detect memory
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    TDLog::enable = true;

//    std::unique_ptr<TDConsumer> consumer = getDebugConsumer();

    std::unique_ptr<TDConsumer> consumer = getLoggerConsumer();

//    std::unique_ptr<TDConsumer> consumer = getBatchConsumer();

    TDAnalytics te(*consumer, false);


    // setSuperProperties
    TDPropertiesNode superProperties;
    superProperties.SetString("test_superProperties", "test_super");
    te.setSupperProperties(superProperties);

    std::string accountId = "shp";
    std::string distinctId = "shp666";
    std::string eventName = "track_event";

    TDPropertiesNode properties;
    properties.SetString("name1", "数数科技");
    properties.SetString("name2", "logBugs");
    properties.SetString("name3", "name3");
    properties.SetNumber("test_number_int", 3);
    properties.SetNumber("test_number_double", 3.14);
    properties.SetBool("test_bool", true);
    properties.SetString("test_stl_string1", "string1");
    timeb t1 = {};
    ftime(&t1);
    properties.SetDateTime("time2", t1.time, t1.millitm);
    std::vector<std::string> list;
    list.emplace_back("item11");
    list.emplace_back("item21");
    properties.SetList("test_list1", list);

    TDPropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");
    event_properties.SetString("name2", "logBugs");
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
    test_list.emplace_back("item11");
    test_list.emplace_back("item21");
    event_properties.SetList("test_list1", test_list);

    event_properties.SetObject("obj", properties);

    std::vector<TDJsonObject> list_objs;
    list_objs.emplace_back(properties);
    list_objs.emplace_back(properties);
    event_properties.SetList("objs", list_objs);

    eventName = "first_event";
    event_properties.SetString("#first_check_id", "first_event");
    te.track_first(accountId, distinctId, eventName, event_properties);

    te.clearSuperProperties();


    eventName = "update_event";
    std::string updateEventId = "update_001";
    TDPropertiesNode update_event_properties;
    update_event_properties.SetString("price", "100");
    update_event_properties.SetBool("status", false);
    te.track_update(accountId, distinctId, eventName, updateEventId, update_event_properties);

    for (size_t i = 0; i < 20; i++)
    {
        te.track_first(accountId, distinctId, eventName, event_properties);
    }

    TDPropertiesNode update_event_new_properties;
    update_event_new_properties.SetBool("status", true);
    te.track_update(accountId, distinctId, eventName, updateEventId, update_event_new_properties);


    eventName = "overWrite_event";
    std::string overWriteEventId = "overWrite_001";
    TDPropertiesNode overWrite_event_properties;
    overWrite_event_properties.SetString("money", "99");
    overWrite_event_properties.SetString("code", "10");
    te.track_overwrite(accountId, distinctId, eventName, overWriteEventId, overWrite_event_properties);

    TDPropertiesNode overWrite_event_new_properties;
    overWrite_event_new_properties.SetString("money", "66");
    te.track_overwrite(accountId, distinctId, eventName, overWriteEventId, overWrite_event_new_properties);


    // user_set
    TDPropertiesNode userSet_properties;
    userSet_properties.SetString("name1", "test1");
    userSet_properties.SetString("name2", "test2");
    te.user_set(accountId, distinctId, userSet_properties);

    userSet_properties.Clear();
    userSet_properties.SetString("user_name", "123");
    te.user_set(accountId, distinctId, userSet_properties);

    // user_setOnce
    TDPropertiesNode userSetOnce_properties;
    userSetOnce_properties.SetString("user_one_name", "ABC");
    te.user_setOnce(accountId, distinctId, userSetOnce_properties);

    userSetOnce_properties.Clear();
    userSetOnce_properties.SetString("user_one_name", "xyz");
    userSetOnce_properties.SetString("user_age", "18");
    te.user_setOnce(accountId, distinctId, userSetOnce_properties);


    // user_add
    TDPropertiesNode userAdd_properties;
    userAdd_properties.SetNumber("cash", 66);
    te.user_add(accountId, distinctId, userAdd_properties);

    userAdd_properties.Clear();
    userAdd_properties.SetNumber("cash", 33);
    te.user_add(accountId, distinctId, userAdd_properties);


    // user_append
    TDPropertiesNode userAppend_properties;
    std::vector<std::string> userAppendListValue;
    userAppendListValue.emplace_back("11");
    userAppendListValue.emplace_back("33");
    userAppend_properties.SetList("arr1", userAppendListValue);
    te.user_append(accountId, distinctId, userAppend_properties);

    // user_uniq_append
    TDPropertiesNode userUniqAppend_properties;
    std::vector<std::string> userUniqAppendListValue;
    userUniqAppendListValue.emplace_back("55");
    userUniqAppendListValue.emplace_back("22");
    userUniqAppendListValue.emplace_back("33");
    userUniqAppendListValue.emplace_back("66");
    userUniqAppendListValue.emplace_back("55");
    userUniqAppend_properties.SetList("arr1", userUniqAppendListValue);
    te.user_uniqAppend(accountId, distinctId, userUniqAppend_properties);

    // user_unset
    TDPropertiesNode userUnset_properties;
    userUnset_properties.SetNumber("user_name", 0);
    te.user_unset(accountId, distinctId, userUnset_properties);

    te.flush();
    te.close();

    return 0;
}
