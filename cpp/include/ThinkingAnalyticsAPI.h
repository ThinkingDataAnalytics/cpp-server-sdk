//
// Created by 沈和平 on 2022/5/12.
//

#ifndef CPP_THINKINGANALYTICSAPI_H
#define CPP_THINKINGANALYTICSAPI_H

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/timeb.h>

#include <pthread.h>
#include <mutex>
#include <zlib.h>
#include "TAConsumer.h"
#include "TAUtils.h"

using namespace std;


namespace TaSDK {

    class ThinkingDataAnalytics {

        TAConsumer &consumer;

        bool enableUUID;

        // 公共属性
        PropertiesNode supperProperties;

        pthread_mutex_t mutex_t;

    public:
        ThinkingDataAnalytics(TAConsumer &consumer);

        ThinkingDataAnalytics(TAConsumer &consumer, bool enableUUID);

        /*!
         * 上报事件
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param eventName      事件名称
         * @param properties     事件属性
         */
        void track(const string &accountId, const string &distinctId, const string &eventName,
                   const PropertiesNode &properties);

        /*!
         * 首次事件
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param eventName      事件名称
         * @param properties     事件属性
         */
        void track_first(const string &accountId, const string &distinctId, const string &eventName,
                         const PropertiesNode &properties);

        /*!
         * 可更新事件
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param eventName      事件名称
         * @param eventId        事件ID
         * @param properties     事件属性
         */
        void
        track_update(const string &accountId, const string &distinctId, const string &eventName, const string &eventId,
                     const PropertiesNode &properties);


        /*!
         * 可重写事件
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param eventName      事件名称
         * @param eventId        事件ID
         * @param properties     事件属性
         */
        void track_overwrite(const string &accountId, const string &distinctId, const string &eventName,
                             const string &eventId, const PropertiesNode &properties);

        /*!
         * 设置用户属性. 如果属性已经存在，则覆盖; 否则，新创建用户属性
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param properties     事件属性
         */
        void user_set(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
         * 设置用户属性. 如果该属性已经存在，该操作无效.
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param properties     事件属性
         */
        void user_setOnce(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
         * 用户属性修改，只支持数字属性增加的接口
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param properties     事件属性
         */
        void user_add(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
         * 用户的数组类型的属性追加
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param properties     事件属性
         */
        void user_append(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
         * 用户的数组类型的属性去重追加
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param properties     事件属性
         */
        void user_uniqAppend(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
         * 删除用户指定的属性
         * @param accountId      账号 ID
         * @param distinctId     访客ID
         * @param properties     事件属性, key对应的value设为0，即删除用户该key属性
         */
        void user_unset(const string &accountId, const string &distinctId, const PropertiesNode &properties);

        /*!
        * 删除用户，此操作不可逆
        * @param accountId      账号 ID
        * @param distinctId     访客ID
        */
        void user_del(const string &accountId, const string &distinctId);

        /*!
         * 立即上报数据到接收端
         */
        void flush();

        /*!
         * 关闭并退出 sdk 所有线程，停止前会清空所有本地数据
         */
        void close();

        /*!
         * 设置公共属性
         * @param supperProperties 公共属性
         */
        void setSupperProperties(const PropertiesNode &supperProperties);

        /*!
         * 清除公共事件属性
         */
        void clearSuperProperties();

    private:
        void add(const string &accountId, const string &distinctId, const string &eventType, const string &eventName,
                 const string &eventId, TAJSONObject properties);

        bool
        transferWithStringMap(const string &key, TAJSONObject &sourceProperties, TAJSONObject &distinationProperties);
    };


} // TaSDK

#endif //CPP_THINKINGANALYTICSAPI_H
