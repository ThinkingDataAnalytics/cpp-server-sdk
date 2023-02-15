
#include "TAUtils.h"

#include <zlib.h>
#include <algorithm>
#include <functional>


namespace TaSDK {



    string getUUID() {
        char str_uuid[80];
        static bool hassrand;
        if (hassrand != true) {
            srand(time(NULL));
            hassrand = true;
        }
        snprintf(str_uuid, sizeof(str_uuid),
                 "%x%x-%x-%x-%x-%x%x%x",
                 rand(),
                 rand(),
                 rand(),
                 ((rand() & 0x0fff) | 0x4000),
                 rand() % 0x3fff + 0x8000,
                 rand(),
                 rand(),
                 rand());
        return string(str_uuid);
    }


    bool assertProperties(const string &key) {
//        regex keyPattern("^(#[a-z][a-z0-9_]{0,49})|([a-z][a-z0-9_]{0,50})$", regex::icase|regex::ECMAScript);
//        smatch result;
//        return regex_match(key, result, keyPattern);
        return true;
    }

    void TAJSONObject::SetNumber(const string &property_name, double value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }

    void TAJSONObject::SetNumber(const string &property_name, int32_t value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(static_cast<int64_t>(value));
    }

    void TAJSONObject::SetNumber(const string &property_name, int64_t value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }

    static const size_t kStringPropertyValueMaxLength = 8192;

    bool CheckUtf8Valid(const string &str) {
        const unsigned char *bytes = (const unsigned char *) str.data();
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
            ErrorLog("Invalid key format: " + property_name);
        }

        properties_map_[property_name] = ValueNode(value);
    }

    void TAJSONObject::SetString(const string &property_name, const char *value) {
        SetString(property_name, string(value));
    }

    void TAJSONObject::SetBool(const string &property_name, bool value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }

    void TAJSONObject::SetObject(const string &property_name, const TAJSONObject &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }

    void TAJSONObject::SetList(const string &property_name,
                               const std::vector<string> &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }


    void TAJSONObject::SetList(const string &property_name,
                               const std::vector<TAJSONObject> &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }

    void TAJSONObject::SetDateTime(const string &property_name,
                                   const time_t seconds,
                                   int32_t milliseconds) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(seconds, milliseconds);
    }

    void TAJSONObject::SetDateTime(const string &property_name,
                                          const string &value) {
        if (!assertProperties(property_name)) {
            ErrorLog("Invalid key format: " + property_name);
        }
        properties_map_[property_name] = ValueNode(value);
    }

    void TAJSONObject::Clear() {
        properties_map_.clear();
    }

    void TAJSONObject::DumpNode(const TAJSONObject &node, string *buffer) {
        *buffer += '{';
        bool first = true;

        for (std::map<string, ValueNode>::const_iterator
                     iterator = node.properties_map_.begin();
             iterator != node.properties_map_.end(); ++iterator) {
            if (first) {
                first = false;
            } else {
                *buffer += ',';
            }
            *buffer += '"' + iterator->first + "\":";
            ValueNode::ToStr(iterator->second, buffer);
        }
        *buffer += '}';
    }

    void TAJSONObject::ValueNode::DumpString(const string &value, string *buffer) {
        *buffer += '"';
        for (std::string::size_type i = 0; i < value.length(); ++i) {
            char c = value[i];
            switch (c) {
                case '"':
                    *buffer += "\\\"";
                    break;
                case '\\':
                    *buffer += "\\\\";
                    break;
                case '\b':
                    *buffer += "\\b";
                    break;
                case '\f':
                    *buffer += "\\f";
                    break;
                case '\n':
                    *buffer += "\\n";
                    break;
                case '\r':
                    *buffer += "\\r";
                    break;
                case '\t':
                    *buffer += "\\t";
                    break;
                default:
                    *buffer += c;
                    break;
            }
        }
        *buffer += '"';
    }

    void TAJSONObject::ValueNode::DumpList(const std::vector<string> &value,
                                           string *buffer) {
        *buffer += '[';
        bool first = true;
        for (std::vector<string>::const_iterator iterator = value.begin();
             iterator != value.end(); ++iterator) {
            if (first) {
                first = false;
            } else {
                *buffer += ',';
            }
            DumpString(*iterator, buffer);
        }
        *buffer += ']';
    }

    void TAJSONObject::ValueNode::DumpList(const std::vector<TAJSONObject> &value, string *buffer) {
        *buffer += '[';
        bool first = true;
        for (std::vector<TAJSONObject>::const_iterator iterator = value.begin(); iterator != value.end(); ++iterator) {
            if (first) {
                first = false;
            } else {
                *buffer += ',';
            }
            DumpNode(*iterator, buffer);
        }
        *buffer += ']';
    }


#if defined(__linux__)
#define TD_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(__APPLE__)
#define TD_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(_WIN32)
#define TD_SDK_LOCALTIME(seconds, now) localtime_s((now), (seconds))
#define snprintf sprintf_s
#endif

   /*********************************  TAJSONObject  *********************************/
    void TAJSONObject::ValueNode::DumpDateTime(const time_t &seconds,
                                               int32_t milliseconds,
                                               string *buffer) {
        struct tm tm = {};
        TD_SDK_LOCALTIME(&seconds, &tm);
        char buff[64];
        snprintf(buff, sizeof(buff), "\"%04d-%02d-%02d %02d:%02d:%02d.%03d\"",
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec,
                 milliseconds);
        *buffer += buff;
    }

    string TAJSONObject::ToJson(const TAJSONObject &node) {
        string buffer;
        DumpNode(node, &buffer);
        return buffer;
    }

    void TAJSONObject::MergeFrom(const TAJSONObject &another_node) {
        for (std::map<string, ValueNode>::const_iterator
                     iterator = another_node.properties_map_.begin();
             iterator != another_node.properties_map_.end(); ++iterator) {
            properties_map_[iterator->first] = iterator->second;
        }
    }

    bool TAJSONObject::ContainsWithKey(const string &key) {
        map<string, ValueNode>::const_iterator iter = properties_map_.find(key);

        if(iter != properties_map_.end())
        {
            return true;
        } else {
            return false;
        }
    }

    void TAJSONObject::FindNode(const string &key, ValueNode &valueNode) {
        map<string, ValueNode>::const_iterator iter = properties_map_.find(key);
        if (iter->first == key) {
            valueNode = iter->second;
        }
    }

    bool TAJSONObject::RemoveNode(const string &key) {
        return properties_map_.erase(key);
    }

    TAJSONObject::TAJSONObject() {}
    /*********************************  TAJSONObject  *********************************/



    /*********************************  ValueNode  *********************************/
    TAJSONObject::ValueNode::ValueNode(double value) : node_type_(NUMBER) {
        value_.number_value = value;
    }

    TAJSONObject::ValueNode::ValueNode(int64_t value) : node_type_(INT) {
        value_.int_value = value;
    }

    TAJSONObject::ValueNode::ValueNode(const string &value)
            : node_type_(STRING),
              string_data_(value) {}

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


    void TAJSONObject::ValueNode::ToStr(const TAJSONObject::ValueNode &node,
                                          string *buffer) {
        switch (node.node_type_) {
            case NUMBER:
                DumpNumber(node.value_.number_value, buffer);
                break;
            case INT:
                DumpNumber(node.value_.int_value, buffer);
                break;
            case STRING:
                DumpString(node.string_data_, buffer);
                break;
            case LIST:
                DumpList(node.list_data_, buffer);
                break;
            case BOOL:
                *buffer += (node.value_.bool_value ? "true" : "false");
                break;
            case OBJECT:
                DumpNode(node.object_data_, buffer);
                break;
            case DATETIME:
                DumpDateTime(node.value_.date_time_value.seconds,
                             node.value_.date_time_value.milliseconds, buffer);
                break;
            case OBJECTS:
                DumpList(node.list_obj_, buffer);
                break;
            default:
                break;
        }
    }

    void TAJSONObject::ValueNode::DumpNumber(double value, string *buffer) {
        ostringstream buf;
        buf.imbue(locale("C"));
        buf << value;
        *buffer += buf.str();
    }

    void TAJSONObject::ValueNode::DumpNumber(int64_t value, string *buffer) {
        ostringstream buf;
        buf.imbue(locale("C"));
        buf << value;
        *buffer += buf.str();
    }
    /*********************************  ValueNode  *********************************/



    /*********************************  PropertiesNode  *********************************/
//    void PropertiesNode::SetObject(const string &property_name,
//                                   const TAJSONObject &value) {
//        cout << "PropertiesNode ~ SetObject" << endl;
//    }
    /*********************************  PropertiesNode  *********************************/





} // TaSDK