
#ifndef CPP_TAUTILS_H
#define CPP_TAUTILS_H

#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <sstream>
#include <cstring>

#define ErrorLog(errorMsg) cerr << "ERROR: 【" << __FILE__ << " -> line number: " << __LINE__ << " -> function name: " << __FUNCTION__ << "】【error message: " << errorMsg << "】" << endl;

const std::string kPathSeparator =
#if defined _WIN32 || defined __CYGWIN__
        "\\";
#else
"/";
#endif


namespace TaSDK {
    using namespace std;

    const string LIB_VERSION = "1.2.2";
    const string LIB_NAME = "tga_cpp_sdk";

    /*!
     * Get random UUID
     * @return
     */
    string getUUID();

    /*!
    * TAJSONObject class
     * Mainly used to process the map attribute properties_map_ in the TAJSONObject class
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

        ValueNode(time_t seconds, int32_t milliseconds);

        static void ToStr(const ValueNode &node, string *buffer);

    private:
        static void DumpString(const string &value, string *buffer);

        static void DumpList(const std::vector<string> &value, string *buffer);

        static void DumpList(const std::vector<TAJSONObject> &value, string *buffer);

        static void DumpDateTime(const time_t &seconds, int32_t milliseconds,
                                 string *buffer);

        static void DumpNumber(double value, string *buffer);

        static void DumpNumber(int64_t value, string *buffer);

        union UnionValue {
            double number_value;
            bool bool_value;
            struct {
                std::time_t seconds;
                int32_t milliseconds;
            } date_time_value;
            int64_t int_value;

            UnionValue() { memset(this, 0, sizeof(UnionValue)); }
        } value_;

        string string_data_;
        std::vector<string> list_data_;
        std::vector<TAJSONObject> list_obj_;
        TAJSONObject object_data_;
    };

    class PropertiesNode : public TAJSONObject {};

} // TaSDK

#endif //CPP_TAUTILS_H
