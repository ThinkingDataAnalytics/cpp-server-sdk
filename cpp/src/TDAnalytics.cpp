#include "TDAnalytics.h"

#if defined(_WIN32)

#include <iostream>

#else

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#endif

const static std::string TA_TRACK                      = "track";
const static std::string TA_TRACK_UPDATE               = "track_update";
const static std::string TA_TRACK_OVERWRITE            = "track_overwrite";

const static std::string TA_USER_SET                   = "user_set";
const static std::string TA_USER_SET_ONCE              = "user_setOnce";
const static std::string TA_USER_UNSET                 = "user_unset";
const static std::string TA_USER_DEL                   = "user_del";
const static std::string TA_USER_ADD                   = "user_add";
const static std::string TA_USER_APPEND                = "user_append";
const static std::string TA_USER_UNIQ_APPEND           = "user_uniq_append";


namespace thinkingDataAnalytics {

    TDAnalytics::TDAnalytics(TDConsumer &consumer): TDAnalytics(consumer, false) {

    }

    TDAnalytics::TDAnalytics(TDConsumer& consumer, bool enableUUID) : m_consumer(consumer), m_enableUUID(enableUUID) {
        TDLog::info("SDK initialization successful");
    }

    void TDAnalytics::track(const std::string& accountId, const std::string& distinctId, const std::string& eventName, const TDPropertiesNode& properties) {
        TDJsonObject allProperties;
        allProperties.MergeFrom(properties);
        m_mutex.lock();
        allProperties.MergeFrom(m_supperProperties);
        m_mutex.unlock();
        add(accountId, distinctId, TA_TRACK, eventName, "", allProperties);
    }

    void TDAnalytics::track_first(const std::string& accountId, const std::string& distinctId, const std::string& eventName, const TDPropertiesNode& properties) {
        if (((TDJsonObject)properties).ContainsWithKey("#first_check_id")) {
            track(accountId, distinctId, eventName, properties);
        }
        else {
            ErrorLog("#first_check_id key must set.")
        }
    }

    void TDAnalytics::flush() {
        TDLog::info("SDK flush.");
        m_consumer.flush();
    }

    void TDAnalytics::track_update(const std::string& accountId, const std::string& distinctId, const std::string& eventName, const std::string& eventId, const TDPropertiesNode& properties) {

        TDJsonObject allProperties;
        m_mutex.lock();
        allProperties.MergeFrom(m_supperProperties);
        m_mutex.unlock();
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_UPDATE, eventName, eventId, allProperties);
    }

    void TDAnalytics::track_overwrite(const std::string& accountId, const std::string& distinctId, const std::string& eventName, const std::string& eventId, const TDPropertiesNode& properties) {
        TDJsonObject allProperties;
        m_mutex.lock();
        allProperties.MergeFrom(m_supperProperties);
        m_mutex.unlock();
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_OVERWRITE, eventName, eventId, allProperties);
    }

    void TDAnalytics::user_set(const std::string& accountId, const std::string& distinctId, const TDPropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_SET, "", "", properties);
    }

    void TDAnalytics::user_setOnce(const std::string& accountId, const std::string& distinctId, const TDPropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_SET_ONCE, "", "", properties);
    }

    void TDAnalytics::user_add(const std::string& accountId, const std::string& distinctId, const TDPropertiesNode& properties) {
        for (auto iterator = properties.m_properties.begin();
            iterator != properties.m_properties.end(); ++iterator) {
            TDJsonObject::TDValueNode valueN = *(iterator->second);
            if (valueN.node_type_ == TDPropertiesNode::NUMBER || valueN.node_type_ == TDPropertiesNode::INT) {}
            else {
                ErrorLog("Only Number or int is allowed for user_add. Invalid property: " + iterator->first)
            }
        }
        add(accountId, distinctId, TA_USER_ADD, "", "", properties);
    }

    void TDAnalytics::user_append(const std::string& accountId, const std::string& distinctId, const TDPropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_APPEND, "", "", properties);
    }

    void TDAnalytics::user_uniqAppend(const std::string& accountId, const std::string& distinctId, const TDPropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_UNIQ_APPEND, "", "", properties);
    }

    void TDAnalytics::user_unset(const std::string& accountId, const std::string& distinctId, const TDPropertiesNode& properties) {
        add(accountId, distinctId, TA_USER_UNSET, "", "", properties);
    }

    void TDAnalytics::user_del(const std::string& accountId, const std::string& distinctId) {
        TDPropertiesNode properties;
        add(accountId, distinctId, TA_USER_DEL, "", "", properties);
    }

    void TDAnalytics::close() {
        m_consumer.close();
        TDLog::info("SDK close");
    }

    void TDAnalytics::add(const std::string& accountId, const std::string& distinctId, const std::string& eventType, const std::string& eventName, const std::string& eventId, const TDJsonObject& properties) {
        if (accountId.empty() && distinctId.empty()) {
            ErrorLog("accountId or distinctId must be provided.")
        }

        TDJsonObject propertiesDic = properties;
        TDJsonObject finalPropertiesDic;

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

        std::string record = TDJsonObject::ToJson(finalPropertiesDic);

        m_consumer.add(record);
    }

    bool TDAnalytics::transferWithStringMap(const std::string& key, TDJsonObject& sourceProperties, TDJsonObject& destinationProperties) {
        if (sourceProperties.ContainsWithKey(key)) {
            std::shared_ptr<TDJsonObject::TDValueNode> valueNode = sourceProperties.FindNode(key);
            if (valueNode != nullptr)
            {
                destinationProperties.m_properties[key] = valueNode;
                sourceProperties.RemoveNode(key);
                return true;
            }   
        }
        return false;
    }

    void TDAnalytics::setSupperProperties(const TDPropertiesNode& supperProperties) {
        m_mutex.lock();
        m_supperProperties.MergeFrom(supperProperties);
        m_mutex.unlock();
    }

    void TDAnalytics::clearSuperProperties() {
        m_mutex.lock();
        m_supperProperties.Clear();
        m_mutex.unlock();
    }
}
