
#include "TAUtils.h"

#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <iomanip>
#include <utility>

namespace TaSDK {

#if defined(__linux__)
#define TD_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(__APPLE__)
#define TD_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(_WIN32)
#define TD_SDK_LOCALTIME(seconds, now) localtime_s((now), (seconds))
#define snprintf sprintf_s
#endif

    string getUUID() {
        static std::random_device rd;
        static std::uniform_int_distribution<uint64_t> dist(0ULL, 0xFFFFFFFFFFFFFFFFULL);
        uint64_t ab = dist(rd);
        uint64_t cd = dist(rd);
        uint32_t a, b, c, d;
        std::stringstream ss;
        ab = (ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
        cd = (cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
        a = (ab >> 32U);
        b = (ab & 0xFFFFFFFFU);
        c = (cd >> 32U);
        d = (cd & 0xFFFFFFFFU);
        ss << std::hex << std::nouppercase << std::setfill('0');
        ss << std::setw(8) << (a) << '-';
        ss << std::setw(4) << (b >> 16U) << '-';
        ss << std::setw(4) << (b & 0xFFFFU) << '-';
        ss << std::setw(4) << (c >> 16U) << '-';
        ss << std::setw(4) << (c & 0xFFFFU);
        ss << std::setw(8) << d;
        return ss.str();
    }

    string formatDateTime(const time_t& seconds, int32_t milliseconds) {
        struct tm tm = {};
        TD_SDK_LOCALTIME(&seconds, &tm);
        char buff[64];
        snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            milliseconds);
        return buff;
    }

    bool assertProperties(const string &key) {
//        regex keyPattern("^(#[a-z][a-z0-9_]{0,49})|([a-z][a-z0-9_]{0,50})$", regex::icase|regex::ECMAScript);
//        smatch result;
//        return regex_match(key, result, keyPattern);
        return true;
    }

    static const size_t kStringPropertyValueMaxLength = 8192;

    bool CheckUtf8Valid(const string &str) {
        const auto *bytes = (const unsigned char *) str.data();
        const unsigned char *begin = bytes;
        while (bytes - begin < (int) str.length()) {
            if ((bytes[0] == 0x09 || bytes[0] == 0x0A || bytes[0] == 0x0D ||
                 (0x20 <= bytes[0] && bytes[0] <= 0x7E))) {
                bytes += 1;
                continue;
            }
            if (((0xC2 <= bytes[0] && bytes[0] <= 0xDF)
                 && (0x80 <= bytes[1] && bytes[1] <= 0xBF))) {
                bytes += 2;
                continue;
            }
            if ((bytes[0] == 0xE0 && (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
                (((0xE1 <= bytes[0] && bytes[0] <= 0xEC) || bytes[0] == 0xEE
                  || bytes[0] == 0xEF) &&
                 (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                 && (0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
                (bytes[0] == 0xED && (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF))) {
                bytes += 3;
                continue;
            }
            if ((bytes[0] == 0xF0 && (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                 (0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
                ((0xF1 <= bytes[0] && bytes[0] <= 0xF3)
                 && (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                 && (0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
                (bytes[0] == 0xF4 && (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                 (0x80 <= bytes[3] && bytes[3] <= 0xBF))) {
                bytes += 4;
                continue;
            }
            return false;
        }
        return bytes - begin == str.length();
    }

    /*********************************  TAJSONObject  *********************************/

    TAJSONObject::TAJSONObject() = default;

    void TAJSONObject::SetNumber(const string& property_name, double value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetNumber(const string& property_name, int32_t value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(static_cast<int64_t>(value));
    }

    void TAJSONObject::SetNumber(const string& property_name, int64_t value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetString(const string &property_name, const string &value) {
        if (value.length() > kStringPropertyValueMaxLength) {
            std::cerr << "String property '" << property_name
                      << "' is too int64_t, value: " << value << std::endl;
            return;
        }
        if (!CheckUtf8Valid(value)) {
            std::cerr << "String property '" << property_name
                      << "' is not valid UTF-8 string, value: " << value
                      << std::endl;
            return;
        }
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }

        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetString(const string &property_name, const char *value) {
        SetString(property_name, string(value));
    }

    void TAJSONObject::SetBool(const string &property_name, bool value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetObject(const string &property_name, const TAJSONObject &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetList(const string &property_name,
                               const std::vector<string> &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetList(const string &property_name,
                               const std::vector<TAJSONObject> &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::SetDateTime(const string &property_name,
                                   const time_t seconds,
                                   int32_t milliseconds) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(seconds, milliseconds);
    }

    void TAJSONObject::SetDateTime(const string &property_name,
                                          const string &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name)
        }
        m_properties[property_name] = make_shared<ValueNode>(value);
    }

    void TAJSONObject::Clear() {
        m_properties.clear();
    }

    void TAJSONObject::DumpNode(const TAJSONObject& node, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
        writer.StartObject();

        for (const auto & pairValue : node.m_properties) {

            writer.Key(pairValue.first.c_str());

            ValueNode& contentNode = *(pairValue.second);

            switch (contentNode.node_type_) {
            case NUMBER: {
                writer.Double(contentNode.value_.number_value);
            } break;
            case INT: {
                writer.Int64(contentNode.value_.int_value);
            } break;
            case STRING: {
                writer.String(contentNode.m_data_string.c_str());
            } break;
            case LIST: {
                writer.StartArray();

                for (const string& var : contentNode.list_data_)
                {
                    writer.String(var.c_str());
                }

                writer.EndArray();
            } break;
            case BOOL: {
                writer.Bool(contentNode.value_.bool_value);
            } break;
            case OBJECT: {
                DumpNode(contentNode.object_data_, writer);
            } break;
            case DATETIME: {
                string str = formatDateTime(contentNode.value_.date_time_value.seconds, contentNode.value_.date_time_value.milliseconds);
                writer.String(str.c_str());
            } break;
            case OBJECTS: {
                writer.StartArray();

                for (const TAJSONObject& var : contentNode.list_obj_)
                {
                    DumpNode(var, writer);
                }

                writer.EndArray();
            } break;
            default:
                break;
            }
        }

        writer.EndObject();
    }

    string TAJSONObject::ToJson(const TAJSONObject& node) {

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);

        DumpNode(node, writer);

        string buffer = strBuf.GetString();

        return buffer;
    }

    void TAJSONObject::MergeFrom(const TAJSONObject& another_node) {
        for (const auto & pairValue : another_node.m_properties) {
            m_properties[pairValue.first] = pairValue.second;
        }
    }

    bool TAJSONObject::ContainsWithKey(const string& key) {
        map<string, shared_ptr<ValueNode>>::const_iterator iter = m_properties.find(key);

        if (iter != m_properties.end())
        {
            return true;
        }
        else {
            return false;
        }
    }
   
    shared_ptr<TAJSONObject::ValueNode> TAJSONObject::FindNode(const string& key) {
        map<string, shared_ptr<ValueNode>>::const_iterator iter = m_properties.find(key);
        if (iter->first == key) {
            return iter->second;
        }
        return nullptr;
    }

    bool TAJSONObject::RemoveNode(const string& key) {
        return m_properties.erase(key);
    }

    /*********************************  ValueNode  *********************************/
    
    TAJSONObject::ValueNode::ValueNode(double value) : node_type_(NUMBER) {
        value_.number_value = value;
    }

    TAJSONObject::ValueNode::ValueNode(int64_t value) : node_type_(INT) {
        value_.int_value = value;
    }

    TAJSONObject::ValueNode::ValueNode(string value)
            : node_type_(STRING),
              m_data_string(std::move(value)) {}

    TAJSONObject::ValueNode::ValueNode(bool value) : node_type_(BOOL) {
        value_.bool_value = value;
    }

    TAJSONObject::ValueNode::ValueNode(const TAJSONObject &value)
            : node_type_(OBJECT) {
        object_data_ = value;
    }

    TAJSONObject::ValueNode::ValueNode(const std::vector<string> &value)
            : node_type_(LIST),
              list_data_(value) {}

    TAJSONObject::ValueNode::ValueNode(const std::vector<TAJSONObject> &value)
            : node_type_(OBJECTS),
              list_obj_(value) {}

    TAJSONObject::ValueNode::ValueNode(time_t seconds, int32_t milliseconds)
            : node_type_(DATETIME) {
        value_.date_time_value.seconds = seconds;
        value_.date_time_value.milliseconds = milliseconds;
    }

} // TaSDK