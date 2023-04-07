#ifndef CPP_THINKINGANALYTICSAPI_H
#define CPP_THINKINGANALYTICSAPI_H

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/timeb.h>

#include <mutex>
#include "TAConsumer.h"
#include "TAUtils.h"

using namespace std;

namespace TaSDK {

    class ThinkingDataAnalytics {
        

    private:
        TAConsumer& m_consumer;
        bool m_enableUUID;
        PropertiesNode m_supperProperties;
        mutex m_mutex;

    public:
        ThinkingDataAnalytics(TAConsumer &consumer);
        ThinkingDataAnalytics(TAConsumer &consumer, bool enableUUID);

        /*!
          * Report incident
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param eventName event name
          * @param properties event properties
          */
        void track(const string &accountId, const string &distinctId, const string &eventName, const PropertiesNode &properties);

        /*!
           * first event
           * @param accountId Account ID
           * @param distinctId visitor ID
           * @param eventName event name
           * @param properties event properties
           */
        void track_first(const string &accountId, const string &distinctId, const string &eventName, const PropertiesNode &properties);

        /*!
          * Updatable events
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param eventName event name
          * @param eventId event ID
          * @param properties event properties
          */
        void track_update(const string &accountId, const string &distinctId, const string &eventName, const string &eventId, const PropertiesNode &properties);


        /*!
         * overwrite event
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param eventName event name
          * @param eventId event ID
          * @param properties event properties
         */
        void track_overwrite(const string &accountId, const string &distinctId, const string &eventName, const string &eventId, const PropertiesNode &properties);

        /*!
          * Set the user attribute. If the attribute already exists, it is overwritten; otherwise, the newly created user attribute
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param properties event properties
         */
        void user_set(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
          * Set the user attribute. If the attribute already exists, the operation has no effect.
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param properties event properties
         */
        void user_setOnce(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
          * User attribute modification, only supports the interface for adding digital attributes
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param properties event properties
         */
        void user_add(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
          * The user's array type attribute append
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param properties event properties
         */
        void user_append(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
          * The attribute of the user's array type is deduplicated and appended
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param properties event properties
         */
        void user_uniqAppend(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
          * Delete user-specified attributes
          * @param accountId Account ID
          * @param distinctId visitor ID
          * @param properties event attribute, the value corresponding to the key is set to 0, that is, the key attribute of the user is deleted
         */
        void user_unset(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
        * Delete a user, this operation is irreversible
        * @param accountId Account ID
        * @param distinctId visitor ID
        */
        void user_del(const string &accountId, const string &distinctId);

        /*!
         * Immediately report data to the receiving end
         */
        void flush();

        /*!
         * Close and exit all threads of sdk, all local data will be cleared before stopping
         */
        void close();

        /*!
         * Set static public properties
         * @param supperProperties static public properties
         */
        void setSupperProperties(const PropertiesNode &supperProperties);

        /*!
         * Clear public event attributes
         */
        void clearSuperProperties();

    private:
        void add(const string &accountId, const string &distinctId, const string &eventType, const string &eventName, const string &eventId, TAJSONObject properties);

        bool transferWithStringMap(const string &key, TAJSONObject &sourceProperties, TAJSONObject &distinationProperties);
    };


} // TaSDK

#endif //CPP_THINKINGANALYTICSAPI_H
