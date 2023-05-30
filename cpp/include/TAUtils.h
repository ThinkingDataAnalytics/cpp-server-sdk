
#ifndef CPP_TD_UTILS_H
#define CPP_TD_UTILS_H

#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <sstream>
#include <cstring>
#include "TAJsonParse.h"

#define ErrorLog(errorMsg) cerr << "ERROR: [" << __FILE__ << " -> line number: " << __LINE__ << " -> function name: " << __FUNCTION__ << "][error message: " << errorMsg << "" << "]" << endl;

const std::string kPathSeparator =
#if defined _WIN32 || defined __CYGWIN__
        "\\";
#else
"/";
#endif


namespace TaSDK {
    using namespace std;

    const string LIB_VERSION = "1.2.8-beta.1";
    const string LIB_NAME = "tga_cpp_sdk";

    /*!
     * Get random UUID
     * @return
     */
    string getUUID();

    string formatDateTime(const time_t& seconds, int32_t milliseconds);

    /*!
    * TAJSONObject class
     * Mainly used to process the map attribute m_properties in the TAJSONObject class
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

        void SetObject(const string &property_name, const TAJSONObject &value);

        void SetList(const string &property_name, const std::vector<TAJSONObject> &value);

        void SetDateTime(const string &property_name, time_t seconds,
                         int32_t milliseconds);

        void SetDateTime(const string &property_name, const string &value);

        void Clear();

        static string ToJson(const TAJSONObject &node);

        TAJSONObject();

        class ValueNode;

        std::map<string, shared_ptr<ValueNode>> m_properties;

        static void DumpNode(const TAJSONObject& node, rapidjson::Writer<rapidjson::StringBuffer> &writer);

        void MergeFrom(const TAJSONObject &another_node);

        bool ContainsWithKey(const string &key);

        shared_ptr<ValueNode> FindNode(const string& key);

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

    class TAJSONObject::ValueNode {
    public:

        ValueNodeType node_type_;

        ValueNode() : node_type_(UNKNOWN) {}

        explicit ValueNode(double value);

        explicit ValueNode(int64_t value);

        explicit ValueNode(string value);

        explicit ValueNode(bool value);

        explicit ValueNode(const TAJSONObject &value);

        explicit ValueNode(const std::vector<string> &value);

        explicit ValueNode(const std::vector<TAJSONObject> &value);

        ValueNode(time_t seconds, int32_t milliseconds);

        TAJSONObject object_data_;
        string m_data_string;
        std::vector<string> list_data_;
        std::vector<TAJSONObject> list_obj_;

        union UnionValue {
            double number_value{};
            bool bool_value;
            struct {
                std::time_t seconds;
                int32_t milliseconds;
            } date_time_value;
            int64_t int_value;

            UnionValue() { memset(this, 0, sizeof(UnionValue)); }
        } value_;
    };

    class PropertiesNode : public TAJSONObject {};

} // TaSDK

#endif //CPP_TD_UTILS_H
