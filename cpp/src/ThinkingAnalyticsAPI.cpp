#include "ThinkingAnalyticsAPI.h"

#if defined(_WIN32)

#include <iostream>

#else

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#endif

const static string TA_TRACK                      = "track";
const static string TA_TRACK_UPDATE               = "track_update";
const static string TA_TRACK_OVERWRITE            = "track_overwrite";

const static string TA_USER_SET                   = "user_set";
const static string TA_USER_SET_ONCE              = "user_setOnce";
const static string TA_USER_UNSET                 = "user_unset";
const static string TA_USER_DEL                   = "user_del";
const static string TA_USER_ADD                   = "user_add";
const static string TA_USER_APPEND                = "user_append";
const static string TA_USER_UNIQ_APPEND           = "user_uniq_append";


namespace TaSDK {

    ThinkingDataAnalytics::ThinkingDataAnalytics(TAConsumer &consumer): ThinkingDataAnalytics(consumer, false) {

    }

    ThinkingDataAnalytics::ThinkingDataAnalytics(TAConsumer& consumer, bool enableUUID) : m_consumer(consumer), m_enableUUID(enableUUID) {
        cout << "[ThinkingEngine] SDK Initialization successful" << endl;
    }

    void ThinkingDataAnalytics::track(const string& accountId, const string& distinctId, const string& eventName, const PropertiesNode& properties) {
        TAJSONObject allProperties;
        allProperties.MergeFrom(properties);
        m_mutex.lock();
        allProperties.MergeFrom(m_supperProperties);
        m_mutex.unlock();
        add(accountId, distinctId, TA_TRACK, eventName, "", allProperties);
    }

    void ThinkingDataAnalytics::track_first(const string& accountId, const string& distinctId, const string& eventName, const PropertiesNode& properties) {
        if (((TAJSONObject)properties).ContainsWithKey("#first_check_id")) {
            track(accountId, distinctId, eventName, properties);
        }
        else {
            ErrorLog("#first_check_id key must set.")
        }
    }

    void ThinkingDataAnalytics::flush() {
        m_consumer.flush();
    }

    void ThinkingDataAnalytics::track_update(const string& accountId, const string& distinctId, const string& eventName, const string& eventId, const PropertiesNode& properties) {

        TAJSONObject allProperties;
        m_mutex.lock();
        allProperties.MergeFrom(m_supperProperties);
        m_mutex.unlock();
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_UPDATE, eventName, eventId, allProperties);
    }

    void ThinkingDataAnalytics::track_overwrite(const string& accountId, const string& distinctId, const string& eventName, const string& eventId, const PropertiesNode& properties) {
        TAJSONObject allProperties;
        m_mutex.lock();
        allProperties.MergeFrom(m_supperProperties);
        m_mutex.unlock();
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_OVERWRITE, eventName, eventId, allProperties);
    }

    void ThinkingDataAnalytics::user_set(const string& accountId, const string& distinctId, const PropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_SET, "", "", properties);
    }

    void ThinkingDataAnalytics::user_setOnce(const string& accountId, const string& distinctId, const PropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_SET_ONCE, "", "", properties);
    }

    void ThinkingDataAnalytics::user_add(const string& accountId, const string& distinctId, const PropertiesNode& properties) {
        for (auto iterator = properties.m_properties.begin();
            iterator != properties.m_properties.end(); ++iterator) {
            TAJSONObject::ValueNode valueN = *(iterator->second);
            if (valueN.node_type_ == PropertiesNode::NUMBER || valueN.node_type_ == PropertiesNode::INT) {}
            else {
                ErrorLog("Only Number or int is allowed for user_add. Invalid property: " + iterator->first)
            }
        }
        add(accountId, distinctId, TA_USER_ADD, "", "", properties);
    }

    void ThinkingDataAnalytics::user_append(const string& accountId, const string& distinctId, const PropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_APPEND, "", "", properties);
    }

    void ThinkingDataAnalytics::user_uniqAppend(const string& accountId, const string& distinctId, const PropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_UNIQ_APPEND, "", "", properties);
    }

    void ThinkingDataAnalytics::user_unset(const string& accountId, const string& distinctId, const PropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_UNSET, "", "", properties);
    }

    void ThinkingDataAnalytics::user_del(const string& accountId, const string& distinctId) {
        PropertiesNode properties;
        add(accountId, distinctId, TA_USER_DEL, "", "", properties);
    }

    void ThinkingDataAnalytics::close() {
        m_consumer.close();
    }

    void ThinkingDataAnalytics::add(const string& accountId, const string& distinctId, const string& eventType, const string& eventName, const string& eventId, const TAJSONObject& properties) {
        if (accountId.empty() && distinctId.empty()) {
            ErrorLog("accountId or distinctId must be provided.")
        }

        TAJSONObject propertiesDic = properties;
        TAJSONObject finalPropertiesDic;

        bool transferUUIDFlag = transferWithStringMap("#uuid", propertiesDic, finalPropertiesDic);
        if (!transferUUIDFlag && m_enableUUID) {
            finalPropertiesDic.SetString("#uuid", getUUID());
        }

        if (!distinctId.empty()) {
            finalPropertiesDic.SetString("#distinct_id", distinctId);
        }
        if (!accountId.empty()) {
            finalPropertiesDic.SetString("#account_id", accountId);
        }

        transferWithStringMap("#app_id", propertiesDic, finalPropertiesDic);

        bool transferTimeFlag = transferWithStringMap("#time", propertiesDic, finalPropertiesDic);
        if (!transferTimeFlag) {
            timeb t{};
            ftime(&t);
            finalPropertiesDic.SetDateTime("#time", t.time, t.millitm);
        }

        transferWithStringMap("#ip", propertiesDic, finalPropertiesDic);
        transferWithStringMap("#first_check_id", propertiesDic, finalPropertiesDic);
        transferWithStringMap("#transaction_property", propertiesDic, finalPropertiesDic);
        transferWithStringMap("#import_tool_id", propertiesDic, finalPropertiesDic);

        finalPropertiesDic.SetString("#type", eventType);

        if (eventType == TA_TRACK || eventType == TA_TRACK_UPDATE || eventType == TA_TRACK_OVERWRITE) {
            if (!eventName.empty()) {
                finalPropertiesDic.SetString("#event_name", eventName);
            }
            else {
                ErrorLog("The event name must be provided.")
            }

            if (eventType == TA_TRACK_UPDATE || eventType == TA_TRACK_OVERWRITE) {
                if (!eventId.empty()) {
                    finalPropertiesDic.SetString("#event_id", eventId);
                }
                else {
                    ErrorLog("The event id must be provided.")
                }
            }

            propertiesDic.SetString("#lib", LIB_NAME);
            propertiesDic.SetString("#lib_version", LIB_VERSION);
        }

        finalPropertiesDic.SetObject("properties", propertiesDic);

        string record = TAJSONObject::ToJson(finalPropertiesDic);

        m_consumer.add(record);
    }

    bool ThinkingDataAnalytics::transferWithStringMap(const string& key, TAJSONObject& sourceProperties, TAJSONObject& destinationProperties) {
        if (sourceProperties.ContainsWithKey(key)) {
            shared_ptr<TAJSONObject::ValueNode> valueNode = sourceProperties.FindNode(key);
            if (valueNode != nullptr)
            {
                destinationProperties.m_properties[key] = valueNode;
                sourceProperties.RemoveNode(key);
                return true;
            }   
        }
        return false;
    }

    void ThinkingDataAnalytics::setSupperProperties(const PropertiesNode& supperProperties) {
        m_mutex.lock();
        m_supperProperties.MergeFrom(supperProperties);
        m_mutex.unlock();
    }

    void ThinkingDataAnalytics::clearSuperProperties() {
        m_mutex.lock();
        m_supperProperties.Clear();
        m_mutex.unlock();
    }
}
