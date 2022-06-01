//
// Created by 沈和平 on 2022/5/12.
//

#include "../include/tacpp/ThinkingAnalyticsAPI.h"

#ifdef WIN32 //Windows
#src <direct.h>
#src <io.h>
#else  // Linux
//#src <sys/io.h>
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
const static string TA_USER_UNIQ_APPEND            = "user_uniq_append";

mutex g_Mutex;
namespace TaSDK {

/*********************************  LoggerConsumer  *********************************/
    LoggerConsumer::LoggerConsumer(const Config &config) {
        if (config.logDirectory.size() <= 0) {
            ErrorLog("指定的目录路径logDirectory不能为空！");
            return;
        }
        fileName = config.logDirectory;

        // 判断文件夹是否存在
        if (access(fileName.c_str(), F_OK) == -1) {
#ifdef WIN32
            int flag = mkdir(dir.c_str());  //Windows创建文件夹
#else
            int flag = mkdir(fileName.c_str(), S_IRWXU);  //Linux创建文件夹
#endif
            if (flag == 0) {  //创建成功
                std::cout << "Create directory successfully." << std::endl;
            } else { //创建失败
                std::cout << "Fail to create directory." << std::endl;
                throw std::exception();
            }
        }

        if (config.fileNamePrefix.size()) {
            fileName = fileName + kPathSeparator + config.fileNamePrefix + "log.";
        } else {
            fileName = fileName + "log.";
        }

        // 基于当前系统的当前日期/时间
        time_t now = time(0);
        tm *ltm = localtime(&now);
        fileName += to_string( (1900 + ltm->tm_year));
        fileName += "-";
        fileName += to_string((1 + ltm->tm_mon));
        fileName += "-";
        fileName += to_string(ltm->tm_mday);

        if (config.rotateMode == HOURLY) {
            fileName += "-";
            fileName += to_string(ltm->tm_hour);
        }

        rotateMode = config.rotateMode;
        bufferSize = config.bufferSize;
        fileSize = config.fileSize;

        cout << "LoggerConsumer初始化成功" << endl;
    }

    LoggerConsumer::Config::Config(const string &logDir, const int bufferS): logDirectory(logDir), bufferSize(bufferS) {
        cout << "Config初始化成功" << endl;
    }

    void LoggerConsumer::add(const string &record) {
        // 加锁
        lock_guard<mutex> lock(g_Mutex);
        if (record.size() <= 0) {
            ErrorLog("Failed to add data, data is empty.");
        }
        messageBuffer += record;
        messageBuffer += "\n";

        if (sizeof(messageBuffer) >= bufferSize) {
            flush();
        }
    }

    string LoggerConsumer::getFileName() {
        int count = 0;
        string fullFileName = fileName + "_" + to_string(count);
        if (fileSize > 0) {
            ifstream inFile(fullFileName, ios::in | ios::binary);
            while (inFile){
                long l, m;
                l = inFile.tellg();
                inFile.seekg (0, ios::end);
                m = inFile.tellg();
                if (((m-l)/(1024*1024)) < fileSize) {
                    break;
                }
                ++count;
                fullFileName = fileName + "_" + to_string(count);
                inFile.close();
                inFile.open(fullFileName, ios::in | ios::binary);
            }
        }
        return fullFileName;
    }

    ofstream  outFile;                        // 打开文件
    string    currentFileNamePath;
    void LoggerConsumer::flush() {
        // 加锁
        lock_guard<mutex> lock(g_Mutex);

        if (messageBuffer.size() <= 0) return;

        string fileNamePath = getFileName();

        if (currentFileNamePath != fileNamePath) {
            outFile.close();
            outFile.open(fileNamePath, ios::out | ios::app);
            currentFileNamePath = fileNamePath;
        }

        if (outFile) {
            if( !(outFile << messageBuffer).bad() ) {
                messageBuffer = "";
            } else {
                ErrorLog("Data write fail.");
            }
        } else {
            ErrorLog("File open fail.");
        }
    }

    void LoggerConsumer::close() {
        flush();
        outFile.close();
    }
/*********************************  LoggerConsumer  *********************************/


/*********************************  DebugConsumer  *********************************/
    DebugConsumer::DebugConsumer(const string &serverUrl, const string &appId) {
        httpService = new HttpService(serverUrl, appId, true);
        httpService->writeData_ = true;
        httpService->LIB_VERSION = LIB_VERSION;
        httpService->LIB_NAME = LIB_NAME;
    }

    DebugConsumer::DebugConsumer(const string &serverUrl, const string &appId, bool writeData) {
        httpService = new HttpService(serverUrl, appId, true);
        httpService->writeData_ = writeData;
        httpService->LIB_VERSION = LIB_VERSION;
        httpService->LIB_NAME = LIB_NAME;
    }

    void DebugConsumer::add(const string &record) {

        bool send_result = httpService->send(record);
        if (!send_result) {
            ErrorLog("Debug 网络上传失败");
        } else {
//            cout << record << endl;
        }
    }

    void DebugConsumer::flush() {

    }

    void DebugConsumer::close() {
        flush();
        delete httpService;
        httpService = nullptr;
    }

    DebugConsumer::~DebugConsumer() {
        delete httpService;
        httpService = nullptr;
    }

/*********************************  DebugConsumer  *********************************/


/*********************************  ThinkingDataAnalytics  *********************************/
    ThinkingDataAnalytics::ThinkingDataAnalytics(TAConsumer &consumer): consumer(consumer) {
        enableUUID = false;
        cout << " 0 - ThinkingDataAnalytics初始化成功" << endl;
    }

    ThinkingDataAnalytics::ThinkingDataAnalytics(TAConsumer &consumer, bool enableUUID): consumer(consumer), enableUUID(enableUUID) {
        cout << " 1 - ThinkingDataAnalytics初始化成功" << endl;
    }

    void ThinkingDataAnalytics::track(const string &accountId, const string &distinctId, const string &eventName, const PropertiesNode &properties) {
        TAJSONObject allProperties;
        allProperties.MergeFrom(supperProperties);
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
        consumer.flush();
    }

    void ThinkingDataAnalytics::track_update(const string &accountId, const string &distinctId, const string &eventName, const string &eventId, const PropertiesNode &properties) {
        TAJSONObject allProperties;
        allProperties.MergeFrom(supperProperties);
        allProperties.MergeFrom(properties);
        add(accountId, distinctId, TA_TRACK_UPDATE, eventName, eventId, allProperties);
    }

    void ThinkingDataAnalytics::track_overwrite(const string &accountId, const string &distinctId, const string &eventName, const string &eventId, const PropertiesNode &properties) {
        TAJSONObject allProperties;
        allProperties.MergeFrom(supperProperties);
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

        consumer.add(record);
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
        ThinkingDataAnalytics::supperProperties = supperProperties;
    }

    void ThinkingDataAnalytics::clearSuperProperties() {
        supperProperties.Clear();
    }

/*********************************  ThinkingDataAnalytics  *********************************/

} // TaSDK