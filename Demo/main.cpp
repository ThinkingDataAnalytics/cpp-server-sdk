#include <iostream>

#include "thirdPark/tacpp/include/tacpp/ThinkingAnalyticsAPI.h"

int main() {

    /*
    * LoogerConsumer
    * */
    string LOG_DIRECTORY = "../trackLog";
    TaSDK::LoggerConsumer::Config config = TaSDK::LoggerConsumer::Config(LOG_DIRECTORY);
    config.fileNamePrefix = "test1";
    config.rotateMode = TaSDK::LoggerConsumer::HOURLY;
    config.fileSize = 1;

    TaSDK::LoggerConsumer logConsumer = TaSDK::LoggerConsumer(config);
    TaSDK::ThinkingDataAnalytics tga = TaSDK::ThinkingDataAnalytics(logConsumer, true);

    /*
     * DebugConsumer
     **/
//    TaSDK::DebugConsumer debugConsumer = TaSDK::DebugConsumer("https://global-receiver-ta.thinkingdata.cn", "1b1c1fef65e3482bad5c9d0e6a823356");
//    TaSDK::ThinkingDataAnalytics tga = TaSDK::ThinkingDataAnalytics(debugConsumer, false);

    string accountId = "shushu";
    string distincId = "shushu666";
    string eventName = "track_envet";


    /******************************** track事件 ****************************************/
    TaSDK::PropertiesNode event_properties;
    event_properties.SetString("name1", "XZ_debug");
    event_properties.SetString("name2", "logbugs");
    event_properties.SetString("name3", "name3");
    event_properties.SetString("#uuid", "1234567890");
    event_properties.SetNumber("test_number_int", 3);
    event_properties.SetNumber("test_number_double", 3.14);
    event_properties.SetBool("test_bool", true);
    std::string test_string = "测试字符串1";
    event_properties.SetString("test_stl_string1", test_string);
    event_properties.SetDateTime("test_time1", time(nullptr), 0);
    timeb t = {};
    ftime(&t);
    event_properties.SetDateTime("#time", t.time, t.millitm);
    std::vector<std::string> test_list;
    test_list.push_back("item11");
    test_list.push_back("item21");
    event_properties.SetList("test_list1", test_list);

    // 设置公共属性
    TaSDK::PropertiesNode superProperties;
    superProperties.SetString("test_superProperties", "test_super");
    tga.setSupperProperties(superProperties);


    // track事件
    tga.track(accountId, distincId, eventName, event_properties);

    // 清除公共属性
    tga.clearSuperProperties();

    // 首次事件
    eventName = "first_envet";
    event_properties.SetString("#first_check_id", "first_event");
    tga.track_first(accountId, distincId, eventName, event_properties);


    // 可更新事件
    eventName = "update_event";
    string updateEventId = "update_001";
    TaSDK::PropertiesNode update_event_properties;
    update_event_properties.SetString("price", "100");
    update_event_properties.SetString("status", "3");
    // 上报后事件属性 status 为 3, price 为 100
    tga.track_update(accountId, distincId, eventName, updateEventId, update_event_properties);

    TaSDK::PropertiesNode update_event_new_properties;
    update_event_new_properties.SetString("status", "5");
    // 上报事件后, 属性 status 被更新为 5, price 不变
    tga.track_update(accountId, distincId, eventName, updateEventId, update_event_new_properties);


    // 可重写事件
    eventName = "overWrite_event";
    string overWriteEventId = "overWrite_001";
    TaSDK::PropertiesNode overWrite_event_properties;
    overWrite_event_properties.SetString("money", "99");
    overWrite_event_properties.SetString("code", "10");
    // 上报后事件属性 code 为 10, money 为 99
    tga.track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_properties);

    TaSDK::PropertiesNode overWrite_event_new_properties;
    overWrite_event_new_properties.SetString("money", "66");
    // 上报后事件属性 money 被更新为 66, code 属性被删除
    tga.track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_new_properties);


/********************************用户属性****************************************/
    // user_set
    TaSDK::PropertiesNode userSet_properties;
    userSet_properties.SetString("name1", "test1");
    userSet_properties.SetString("name2", "test2");
    //上传用户属性
    tga.user_set(accountId, distincId, userSet_properties);

    userSet_properties.Clear();
    userSet_properties.SetString("user_name", "123");
    //再次上传用户属性，此时"user_name"的值会被覆盖为"123"
    tga.user_set(accountId, distincId, userSet_properties);

    // user_setOnce
    TaSDK::PropertiesNode userSetOnce_properties;
    userSetOnce_properties.SetString("user_one_name", "ABC");
    //上传用户属性, 新建"user_one_name"，值为"ABC"
    tga.user_setOnce(accountId, distincId, userSetOnce_properties);

    userSetOnce_properties.Clear();
    userSetOnce_properties.SetString("user_one_name", "xyz");
    userSetOnce_properties.SetString("user_age", "18");
    // 再次上传用户属性，此时"user_one_name"的值不会被覆盖，仍为"ABC"；"user_age"的值为18
    tga.user_setOnce(accountId, distincId, userSetOnce_properties);


    // user_add
    TaSDK::PropertiesNode userAdd_properties;
    userAdd_properties.SetNumber("cash", 66);
    // 上传用户属性，此时"cash"的值为66
    tga.user_add(accountId, distincId, userAdd_properties);

    userAdd_properties.Clear();
    userAdd_properties.SetNumber("cash", 33);
    //再次上传用户属性，此时"cash"的值会累加为99
    tga.user_add(accountId, distincId, userAdd_properties);


    // user_append
    TaSDK::PropertiesNode userAppend_properties;
    vector<string> userAppenListValue;
    userAppenListValue.push_back("11");
    userAppenListValue.push_back("33");
    userAppend_properties.SetList("arr1", userAppenListValue);
    // 为arr1的数组类型追加属性
    tga.user_append(accountId, distincId, userAppend_properties);



    // user_uniq_append
    TaSDK::PropertiesNode userUniqAppend_properties;
    vector<string> userUniqAppenListValue;
    userUniqAppenListValue.push_back("55");
    userUniqAppenListValue.push_back("22");
    userUniqAppenListValue.push_back("33");
    userUniqAppenListValue.push_back("66");
    userUniqAppenListValue.push_back("55");
    // 为arr1的数组类型去重追加属性
    userUniqAppend_properties.SetList("arr1", userUniqAppenListValue);
    tga.user_uniqAppend(accountId, distincId, userUniqAppend_properties);



    // user_unset
    TaSDK::PropertiesNode userUnset_properties;
    userUnset_properties.SetNumber("user_name", 0);
    // 清空该用户name1、name2属性值
    tga.user_unset(accountId, distincId, userUnset_properties);


    // user_del
     tga.user_del(accountId, distincId);

    tga.flush();

    return 0;
}
