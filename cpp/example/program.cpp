#include <iostream>

#include "../include/ThinkingAnalyticsAPI.h"
#include "../include/TALoggerConsumer.h"
#include "../include/TADebugConsumer.h"
#include "../include/TABatchConsumer.h"

#include <thread>
#include <chrono>

using namespace std::chrono;
using namespace std;
using namespace TaSDK;


// enable debug
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

void asyncTrack1(ThinkingDataAnalytics& te) {
    string accountId = "1";
    string eventName = "track_event";

    PropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");

    te.track(accountId, "", eventName, event_properties);
}

void asyncTrack2(ThinkingDataAnalytics& te) {
    string accountId = "2";
    string eventName = "track_event";

    PropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");

    te.track(accountId, "", eventName, event_properties);
}

unique_ptr<TAConsumer> getLoggerConsumer() {
    LoggerConsumer::Config config = LoggerConsumer::Config("H:/log", 20, 500, LoggerConsumer::HOURLY);
    config.fileNamePrefix = "te";
    config.rotateMode = LoggerConsumer::HOURLY;
    unique_ptr<TAConsumer> ptr(new LoggerConsumer(config));
    return ptr;
}

unique_ptr<TAConsumer> getBatchConsumer() {
    unique_ptr<TAConsumer> ptr(new TABatchConsumer("appId", "serverUrl", 20, true, "/test/cert.pem"));
    return ptr;
}

unique_ptr<TAConsumer> getDebugConsumer() {
    unique_ptr<TAConsumer> ptr(new TADebugConsumer("appId", "serverUrl", "", "123456789"));
    return ptr;
}

int main(int argc, char *argv[]) {

#ifdef _DEBUG
    // enable debug. Detect ​memory
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    //unique_ptr<TAConsumer> consumer = getDebugConsumer();

    unique_ptr<TAConsumer> consumer = getLoggerConsumer();

    //unique_ptr<TAConsumer> consumer = getBatchConsumer();

    ThinkingDataAnalytics te(*consumer, false);


    // setSuperProperties
    PropertiesNode superProperties;
    superProperties.SetString("test_superProperties", "test_super");
    te.setSupperProperties(superProperties);

    string accountId = "shp";
    string distinctId = "shp666";
    string eventName = "track_event";

    PropertiesNode properties;
    properties.SetString("name1", "XZ_debug");
    properties.SetString("name2", "logBugs");
    properties.SetString("name3", "name3");
    properties.SetString("#uuid", "aaa");
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

    PropertiesNode event_properties;
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

    std::vector<TAJSONObject> list_objs;
    list_objs.emplace_back(properties);
    list_objs.emplace_back(properties);
    event_properties.SetList("objs", list_objs);

    eventName = "first_event";
    event_properties.SetString("#first_check_id", "first_event");
    te.track_first(accountId, distinctId, eventName, event_properties);

    te.clearSuperProperties();


    eventName = "update_event";
    string updateEventId = "update_001";
    PropertiesNode update_event_properties;
    update_event_properties.SetString("price", "100");
    update_event_properties.SetBool("status", false);
    te.track_update(accountId, distinctId, eventName, updateEventId, update_event_properties);

    for (size_t i = 0; i < 20; i++)
    {
        te.track_first(accountId, distinctId, eventName, event_properties);
    }

    PropertiesNode update_event_new_properties;
    update_event_new_properties.SetBool("status", true);
    te.track_update(accountId, distinctId, eventName, updateEventId, update_event_new_properties);


    eventName = "overWrite_event";
    string overWriteEventId = "overWrite_001";
    PropertiesNode overWrite_event_properties;
    overWrite_event_properties.SetString("money", "99");
    overWrite_event_properties.SetString("code", "10");
    te.track_overwrite(accountId, distinctId, eventName, overWriteEventId, overWrite_event_properties);

    PropertiesNode overWrite_event_new_properties;
    overWrite_event_new_properties.SetString("money", "66");
    te.track_overwrite(accountId, distinctId, eventName, overWriteEventId, overWrite_event_new_properties);


    // user_set
    PropertiesNode userSet_properties;
    userSet_properties.SetString("name1", "test1");
    userSet_properties.SetString("name2", "test2");
    te.user_set(accountId, distinctId, userSet_properties);

    userSet_properties.Clear();
    userSet_properties.SetString("user_name", "123");
    te.user_set(accountId, distinctId, userSet_properties);

    // user_setOnce
    PropertiesNode userSetOnce_properties;
    userSetOnce_properties.SetString("user_one_name", "ABC");
    te.user_setOnce(accountId, distinctId, userSetOnce_properties);

    userSetOnce_properties.Clear();
    userSetOnce_properties.SetString("user_one_name", "xyz");
    userSetOnce_properties.SetString("user_age", "18");
    te.user_setOnce(accountId, distinctId, userSetOnce_properties);


    // user_add
    PropertiesNode userAdd_properties;
    userAdd_properties.SetNumber("cash", 66);
    te.user_add(accountId, distinctId, userAdd_properties);

    userAdd_properties.Clear();
    userAdd_properties.SetNumber("cash", 33);
    te.user_add(accountId, distinctId, userAdd_properties);


    // user_append
    PropertiesNode userAppend_properties;
    vector<string> userAppendListValue;
    userAppendListValue.emplace_back("11");
    userAppendListValue.emplace_back("33");
    userAppend_properties.SetList("arr1", userAppendListValue);
    te.user_append(accountId, distinctId, userAppend_properties);

    // user_uniq_append
    PropertiesNode userUniqAppend_properties;
    vector<string> userUniqAppendListValue;
    userUniqAppendListValue.emplace_back("55");
    userUniqAppendListValue.emplace_back("22");
    userUniqAppendListValue.emplace_back("33");
    userUniqAppendListValue.emplace_back("66");
    userUniqAppendListValue.emplace_back("55");
    userUniqAppend_properties.SetList("arr1", userUniqAppendListValue);
    te.user_uniqAppend(accountId, distinctId, userUniqAppend_properties);

    // user_unset
    PropertiesNode userUnset_properties;
    userUnset_properties.SetNumber("user_name", 0);
    te.user_unset(accountId, distinctId, userUnset_properties);

    te.flush();
    te.close();

    return 0;
}
