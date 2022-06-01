//
// Created by 沈和平 on 2022/5/13.
//

#ifndef CPP_TAUTILS_H
#define CPP_TAUTILS_H

#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <sstream>

#include "TANetworking.h"

#define ErrorLog(errorMsg) cerr << "ERROR: 【" << __FILE__ << " -> 行号: " << __LINE__ << " -> 函数: " << __FUNCTION__ << "】【错误信息: " << errorMsg << "】" << endl;

namespace TaSDK {
    using namespace std;

    /*!
     * 获取随机UUID
     * @return
     */
    string getUUID();

    /*!
     * TAJSONObject 类
     * 主要用于处理 TAJSONObject 类中的 map 属性 properties_map_
     */
    class TAJSONObject {
    public:
        void SetNumber(const string &property_name, int32_t value);

        void SetNumber(const string &property_name, int64_t value);

        void SetNumber(const string &property_name, double value);

        void SetString(const string &property_name, const string &value);

        void SetString(const string &property_name, const char *value);

        void SetBool(const string &property_name, bool value);

        void SetList(const string &property_name, const std::vector<string> &value);

        void SetList(const string &property_name, const std::vector<TAJSONObject> &value);

        void SetDateTime(const string &property_name, time_t seconds,
                         int milliseconds);

        // 字符串格式需要是: 2018-09-07 16:30:22.567
        void SetDateTime(const string &property_name, const string &value);

        void Clear();

        virtual void SetObject(const string &property_name, const TAJSONObject &value);

        static string ToJson(const TAJSONObject &node);

        TAJSONObject();

        class ValueNode;

        std::map<string, ValueNode> properties_map_;

        static void DumpNode(const TAJSONObject &node, string *buffer);

        void MergeFrom(const TAJSONObject &another_node);

        bool ContainsWithKey(const string &key);

        void FindNode(const string &key, ValueNode &valueNode);

        bool RemoveNode(const string &key);

        enum ValueNodeType {
            NUMBER,
            INT,
            STRING,
            LIST,
            DATETIME,
            BOOL,
            OBJECT,
            OBJECTS,
            UNKNOWN,
        };
    };

    /*!
     * ValueNode 类
     * 可以设置各种类型的value的集成
     */
    class TAJSONObject::ValueNode {
    public:

        ValueNodeType node_type_;

        ValueNode() : node_type_(UNKNOWN) {}

        explicit ValueNode(double value);

        explicit ValueNode(int64_t value);

        explicit ValueNode(const string &value);

        explicit ValueNode(bool value);

        explicit ValueNode(const TAJSONObject &value);

        explicit ValueNode(const std::vector<string> &value);

        explicit ValueNode(const std::vector<TAJSONObject> &value);

        ValueNode(time_t seconds, int milliseconds);

        static void ToStr(const ValueNode &node, string *buffer);

    private:
        static void DumpString(const string &value, string *buffer);

        static void DumpList(const std::vector<string> &value, string *buffer);

        static void DumpList(const std::vector<TAJSONObject> &value, string *buffer);

        static void DumpDateTime(const time_t &seconds, int milliseconds,
                                 string *buffer);

        static void DumpNumber(double value, string *buffer);

        static void DumpNumber(int64_t value, string *buffer);

        union UnionValue {
            double number_value;
            bool bool_value;
            struct {
                std::time_t seconds;
                int milliseconds;
            } date_time_value;
            int64_t int_value;

            UnionValue() { memset(this, 0, sizeof(UnionValue)); }
        } value_;

        string string_data_;
        std::vector<string> list_data_;
        std::vector<TAJSONObject> list_obj_;
        TAJSONObject object_data_;
    };

    class PropertiesNode : public TAJSONObject {
        void SetObject(const string &property_name, const TAJSONObject &value);
    };

    /*!
     * 网络类
     */
    class HttpService {
    public:
        HttpResponse *response;
        bool debugFlag_;
        bool writeData_;

        string LIB_VERSION;
        string LIB_NAME;

    public:
        HttpService(const string &server_url,
                    const string &appid, const bool debugFlag);

        bool send(const string &data);

    private:
        static bool gzipString(const string &str,
                               string *out_string,
                               int compression_level);

        static bool encodeToRequestBody(const string &data,
                                        string *request_body);

        static string base64Encode(const string &data);

        friend class DebugConsumer;

        static const int kRequestTimeoutSecond = 3;
        string server_url_;
        string appid_;
    };

} // TaSDK

#endif //CPP_TAUTILS_H
