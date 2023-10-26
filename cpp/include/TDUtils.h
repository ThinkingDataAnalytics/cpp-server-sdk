
#ifndef CPP_TD_UTILS_H
#define CPP_TD_UTILS_H

#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <sstream>
#include <cstring>
#include "TDJsonParse.h"

#define ErrorLog(errorMsg) std::cerr << "ERROR: [" << __FILE__ << " -> line number: " << __LINE__ << " -> function name: " << __FUNCTION__ << "][error message: " << errorMsg << "" << "]" << std::endl;

const std::string kPathSeparator =
#if defined _WIN32 || defined __CYGWIN__
        "\\";
#else
"/";
#endif


namespace thinkingDataAnalytics {
    const std::string LIB_VERSION = "2.0.0-beta.1";
    const std::string LIB_NAME = "tga_cpp_sdk";

    /**
     * Get random UUID
     * @return uuid v4 string
     */
    std::string getUUID();

    std::string formatDateTime(const time_t& seconds, int32_t milliseconds);

    /*!
    * TDJsonObject class
     * Mainly used to process the map attribute m_properties in the TDJsonObject class
     */
    class TDJsonObject {
    public:
        void SetNumber(const std::string &property_name, int32_t value);

        void SetNumber(const std::string &property_name, int64_t value);

        void SetNumber(const std::string &property_name, double value);

        void SetString(const std::string &property_name, const std::string &value);

        void SetString(const std::string &property_name, const char *value);

        void SetBool(const std::string &property_name, bool value);

        void SetList(const std::string &property_name, const std::vector<std::string> &value);

        void SetObject(const std::string &property_name, const TDJsonObject &value);

        void SetList(const std::string &property_name, const std::vector<TDJsonObject> &value);

        void SetDateTime(const std::string &property_name, time_t seconds,
                         int32_t milliseconds);

        void SetDateTime(const std::string &property_name, const std::string &value);

        void Clear();

        static std::string ToJson(const TDJsonObject &node);

        TDJsonObject();

        class TDValueNode;

        std::map<std::string, std::shared_ptr<TDValueNode>> m_properties;

        static void DumpNode(const TDJsonObject& node, rapidjson::Writer<rapidjson::StringBuffer> &writer);

        void MergeFrom(const TDJsonObject &another_node);

        bool ContainsWithKey(const std::string &key);

        std::shared_ptr<TDValueNode> FindNode(const std::string& key);

        bool RemoveNode(const std::string &key);

        enum ValueNodeType {
            NUMBER,
            INT,
            STRING,
            LIST,
            DATETIME,
            TD_BOOL,
            OBJECT,
            OBJECTS,
            UNKNOWN,
        };
    };

    class TDJsonObject::TDValueNode {
    public:

        ValueNodeType node_type_;

        TDValueNode() : node_type_(UNKNOWN) {}

        explicit TDValueNode(double value);

        explicit TDValueNode(int64_t value);

        explicit TDValueNode(std::string value);

        explicit TDValueNode(bool value);

        explicit TDValueNode(const TDJsonObject &value);

        explicit TDValueNode(const std::vector<std::string> &value);

        explicit TDValueNode(const std::vector<TDJsonObject> &value);

        TDValueNode(time_t seconds, int32_t milliseconds);

        TDJsonObject object_data_;
        std::string m_data_string;
        std::vector<std::string> list_data_;
        std::vector<TDJsonObject> list_obj_;

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

    /**
     * Event properties node
     */
    class TDPropertiesNode : public TDJsonObject {};

    class TDLog {
    public:
       static bool enable;
       static void info(const std::string& message);
    };
}

#endif //CPP_TD_UTILS_H
