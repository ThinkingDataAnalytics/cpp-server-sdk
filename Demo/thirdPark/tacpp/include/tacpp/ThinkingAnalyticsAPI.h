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

//#src <curl/curl.h>
#include <zlib.h>


#include "../../include/tacpp/TAConsumer.h"
#include "TAUtils.h"

using namespace std;

const string kPathSeparator =
#if defined _WIN32 || defined __CYGWIN__
        "\\";
#else
        "/";
#endif

namespace TaSDK {

    const string LIB_VERSION = "1.0.0";
    const string LIB_NAME = "tga_cpp_sdk";

    class LoggerConsumer : public TAConsumer {

    public:
        // 日志切分模式
        enum RotateMode {
            DAILY,             // 以天为单元
            HOURLY             // 以小时为单元
        };

    private:
        string fileName;                   // 文件名称
        int bufferSize;                     // messageBuffer 写入文件中的大小阈值, 即执行flush;
        int fileSize;                       // 单个文件大小
        string messageBuffer;                  // 缓存每条事件数据到内存中
        RotateMode rotateMode;

    public:
        class Config {

        public:
            string logDirectory;                           // 数据入库目录
            int bufferSize;                             // 内存缓存大小
            RotateMode rotateMode;                 // 默认以天为单元对日志进行分割
            string fileNamePrefix;                         // 日志文件前缀
            int fileSize;                             // 单个文件的大小阈值，默认值 0 代表 无穷大, 单位M

        public:
            Config(const string &logDir, const int bufferS = 8192);
        };

    public:
        LoggerConsumer(const Config &config);

        void add(const string &record);

        void flush();

        void close();

        string getFileName();
    };

    class DebugConsumer : public TAConsumer {

    private:
        HttpService *httpService;

    public:
        /*!
         * 创建指定接收端地址和 APP ID 的 DebugConsumer
         * @param serverUrl  接收端地址
         * @param appId      APP ID
         */
        DebugConsumer(const string &serverUrl, const string &appId);

        /*!
         * 创建指定接收端地址和 APP ID 的 DebugConsumer
         * @param serverUrl  接收端地址
         * @param appId      APP ID
         * @param writeData  是否入TA系统； true为入；false为不入，仅仅校验数据格式是否正确
         */
        DebugConsumer(const string &serverUrl, const string &appId, bool writeData);

        void add(const string &record);

        void flush();

        void close();

        ~DebugConsumer();
    };

    class ThinkingDataAnalytics {

        TAConsumer &consumer;

        bool enableUUID;

        // 公共属性
        PropertiesNode supperProperties;

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
