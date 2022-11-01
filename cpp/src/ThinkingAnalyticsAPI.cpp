#include "ThinkingAnalyticsAPI.h"


#ifdef WIN32 //Windows

#else  // Linux
//#src <sys/io.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#include <iostream>
#include <fstream>
#endif

#include <string>


pthread_mutex_t ta_mutex;
//初始化互斥锁
//pthread_mutex_init(&ta_mutex, 0);


#define TA_LOCK(t) pthread_mutex_lock(t)
#define TA_UNLOCK(t) pthread_mutex_unlock(t)

const static string TA_TRACK                      = "track";
const static string TA_TRACK_UPDATE               = "track_update";
const static string TA_TRACK_OVERWRITE            = "track_overwrite";

const static string TA_USER_SET                   = "user_set";
const static string TA_USER_SET_ONCE              = "user_setOnce";
const static string TA_USER_UNSET                 = "user_unset";
const static string TA_USER_DEL                   = "user_del";
const static string TA_USER_ADD                   = "user_add";
const static string TA_USER_APPEND                = "user_append";
const static string TA_USER_UNIQ_APPEND            = "user_uniq_append";


namespace TaSDK {

    ThinkingDataAnalytics::ThinkingDataAnalytics(TAConsumer &consumer): consumer(consumer) {
        enableUUID = false;
        pthread_mutex_init(&mutex_t, 0);

        cout << " 0 - ThinkingDataAnalytics初始化成功" << endl;
    }

    ThinkingDataAnalytics::ThinkingDataAnalytics(TAConsumer &consumer, bool enableUUID): consumer(consumer), enableUUID(enableUUID) {
        pthread_mutex_init(&mutex_t, 0);
        cout << " 1 - ThinkingDataAnalytics初始化成功" << endl;
    }

    void ThinkingDataAnalytics::track(const string &accountId, const string &distinctId, const string &eventName, const PropertiesNode &properties) {
        TAJSONObject allProperties;
        pthread_mutex_lock(&mutex_t);
        allProperties.MergeFrom(supperProperties);
        pthread_mutex_unlock(&mutex_t);
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK, eventName, "", allProperties);
    }

    void ThinkingDataAnalytics::track_first(const string &accountId, const string &distinctId, const string &eventName, const PropertiesNode &properties) {

        if (((TAJSONObject)properties).ContainsWithKey("#first_check_id")) {
            track(accountId, distinctId, eventName, properties);
        } else {
            ErrorLog("#first_check_id key must set.");
        }
    }

    void ThinkingDataAnalytics::flush() {
        pthread_mutex_lock(&mutex_t);
        consumer.flush();
        pthread_mutex_unlock(&mutex_t);
    }

    void ThinkingDataAnalytics::track_update(const string &accountId, const string &distinctId, const string &eventName, const string &eventId, const PropertiesNode &properties) {

        TAJSONObject allProperties;
        pthread_mutex_lock(&mutex_t);
        allProperties.MergeFrom(supperProperties);
        pthread_mutex_unlock(&mutex_t);
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_UPDATE, eventName, eventId, allProperties);
    }

    void ThinkingDataAnalytics::track_overwrite(const string &accountId, const string &distinctId, const string &eventName, const string &eventId, const PropertiesNode &properties) {
        TAJSONObject allProperties;
        pthread_mutex_lock(&mutex_t);
        allProperties.MergeFrom(supperProperties);
        pthread_mutex_unlock(&mutex_t);
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_OVERWRITE, eventName, eventId, allProperties);
    }

    void ThinkingDataAnalytics::user_set(const string &accountId, const string &distinctId, const PropertiesNode &properties) {
        add(accountId, distinctId, TA_USER_SET, "", "", properties);
    }

    void ThinkingDataAnalytics::user_setOnce(const string &accountId, const string &distinctId, const PropertiesNode &properties) {
        add(accountId, distinctId, TA_USER_SET_ONCE, "", "", properties);
    }

    void ThinkingDataAnalytics::user_add(const string &accountId, const string &distinctId, const PropertiesNode &properties) {
        for (std::map<string, TAJSONObject::ValueNode>::const_iterator
                     iterator = properties.properties_map_.begin();
             iterator != properties.properties_map_.end(); ++iterator) {
            TAJSONObject::ValueNode valueN = iterator->second;
            if (valueN.node_type_ == PropertiesNode::NUMBER || valueN.node_type_ == PropertiesNode::INT) { }
            else {
                ErrorLog("Only Number or int is allowed for user_add. Invalid property: " + iterator->first);
            }
        }
        add(accountId, distinctId, TA_USER_ADD, "", "", properties);
    }

    void ThinkingDataAnalytics::user_append(const string &accountId, const string &distinctId, const PropertiesNode &properties) {
        add(accountId, distinctId, TA_USER_APPEND, "", "", properties);
    }

    void ThinkingDataAnalytics::user_uniqAppend(const string &accountId, const string &distinctId, const PropertiesNode &properties) {
        add(accountId, distinctId, TA_USER_UNIQ_APPEND, "", "", properties);
    }

    void ThinkingDataAnalytics::user_unset(const string &accountId, const string &distinctId, const PropertiesNode &properties) {
        add(accountId, distinctId, TA_USER_UNSET, "", "", properties);
    }

    void ThinkingDataAnalytics::user_del(const string &accountId, const string &distinctId) {
        PropertiesNode perperties;
        add(accountId, distinctId, TA_USER_DEL, "", "", perperties);
    }

    void ThinkingDataAnalytics::close() {
        consumer.close();
    }

    void ThinkingDataAnalytics::add(const string &accountId, const string &distinctId, const string &eventType, const string &eventName, const string &eventId, TAJSONObject properties) {
        if (accountId.size() <= 0 && distinctId.size() <= 0) {
            throw std::invalid_argument("accountId or distinctId must be provided.");
        }

        TAJSONObject propertiesDic;
        TAJSONObject finalPropertiesDic;

        // 处理UUID
        bool transferUUIDFlag = transferWithStringMap("#uuid", properties, finalPropertiesDic);
        if (!transferUUIDFlag && enableUUID) {
            finalPropertiesDic.SetString("#uuid", getUUID());
        }

        // 加入accountId or distinctId
        if (distinctId.size()) {
            finalPropertiesDic.SetString("#distinct_id", distinctId);
        }
        if (accountId.size()) {
            finalPropertiesDic.SetString("#account_id", accountId);
        }

        // 处理appId
        transferWithStringMap("#app_id", properties, finalPropertiesDic);

        // 处理time
        bool transforTimeFlag = transferWithStringMap("#time", properties, finalPropertiesDic);
        if (!transforTimeFlag) {
            timeb t;
            ftime(&t);
            finalPropertiesDic.SetDateTime("#time", t.time, t.millitm);
        }

        // 预置属性提取
        transferWithStringMap("#ip", properties, finalPropertiesDic);
        transferWithStringMap("#first_check_id", properties, finalPropertiesDic);
        transferWithStringMap("#transaction_property", properties, finalPropertiesDic);
        transferWithStringMap("#import_tool_id", properties, finalPropertiesDic);

        // 上传事件类型
        finalPropertiesDic.SetString("#type", eventType);

        if (eventType == TA_TRACK || eventType == TA_TRACK_UPDATE || eventType == TA_TRACK_OVERWRITE) {
            if (eventName.size()) {
                finalPropertiesDic.SetString("#event_name", eventName);
            } else {
                ErrorLog("The event name must be provided.");
            }

            // 对 可更新事件、可重写事件 的 eventId 校验
            if (eventType == TA_TRACK_UPDATE || eventType == TA_TRACK_OVERWRITE) {
                if (eventId.size()) {
                    finalPropertiesDic.SetString("#event_id", eventId);
                } else {
                    ErrorLog("The event id must be provided.");
                }
            }

            propertiesDic.SetString("#lib", LIB_NAME);
            propertiesDic.SetString("#lib_version", LIB_VERSION);
        }

        propertiesDic.MergeFrom(properties);

        finalPropertiesDic.SetObject("properties", propertiesDic);

        string record = finalPropertiesDic.ToJson(finalPropertiesDic);

        pthread_mutex_lock(&mutex_t);
        consumer.add(record);
        pthread_mutex_unlock(&mutex_t);
    }

    bool ThinkingDataAnalytics::transferWithStringMap(const string &key, TAJSONObject &sourceProperties, TAJSONObject &distinationProperties) {
        if (sourceProperties.ContainsWithKey(key)) {
            PropertiesNode::ValueNode vauleNode;
            sourceProperties.FindNode(key, vauleNode);
            string valueStr;
            vauleNode.ToStr(vauleNode, &valueStr);
            valueStr.erase(0, 1);
            valueStr.erase(valueStr.find("\""), 1);
            if (valueStr.size()) {
                distinationProperties.SetString(key, valueStr);
                sourceProperties.RemoveNode(key);
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    void ThinkingDataAnalytics::setSupperProperties(const PropertiesNode &supperProperties) {
        // TA_LOCK(&ta_mutex);
        ThinkingDataAnalytics::supperProperties = supperProperties;
        // TA_UNLOCK(&ta_mutex);
    }

    void ThinkingDataAnalytics::clearSuperProperties() {
        // TA_LOCK(&ta_mutex);
        supperProperties.Clear();
        // TA_UNLOCK(&ta_mutex);
    }

} // TaSDK