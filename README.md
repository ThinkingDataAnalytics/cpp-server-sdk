# 1.1 集成 SDK
将代码下载后，进入cpp文件夹，include文件夹包含所有头文件，src文件夹包含所有源文件，可用以下两种方式进行logger consumer功能的集成：

## 使用CMake方式进行集成
```c

### 引入头文件
include_directories(include)

### 添加tacpp库文件
add_library(tacpp src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TALoggerConsumer.cpp)

### 向工程中添加tacpp库
add_executable(program example/program.cpp)
target_link_libraries(program tacpp)
```

## 使用源代码进行集成
将include文件夹和src文件夹引入到工程。


# 1.2 初始化 SDK
您可以通过以下方法获得 SDK 实例：
```c++
/*
* LoogerConsumer
* */
    TaSDK::LoggerConsumer::Config config = TaSDK::LoggerConsumer::Config("../../../../cpp_sdk_dev_log");
    config.fileNamePrefix = "";
    config.rotateMode = TaSDK::LoggerConsumer::HOURLY;
    config.fileSize = 1;

    TaSDK::LoggerConsumer logConsumer = TaSDK::LoggerConsumer(config);
    TaSDK::ThinkingDataAnalytics tga = TaSDK::ThinkingDataAnalytics(logConsumer, true);
```
LoogerConsumer将会批量实时地生成日志文件，文件默认以小时切分，需要搭配 LogBus 进行上传。
YOUR_LOG_PATH 需要更改为日志存储的目录，您只需将 LogBus 的监听文件夹地址设置为此处的地址，即可使用 LogBus 进行数据的监听上传。


# 二、上报数据


## 2.1 发送事件
您可以调用track来上传事件，建议您根据先前梳理的文档来设置事件的属性以及发送信息的条件，此处以用户付费作为范例：
```C++

string accountId = "shp";
string distincId = "shp666";
string eventName = "track_envet";

TaSDK::PropertiesNode event_properties;
event_properties.SetString("name1", "XZ_debug");
event_properties.SetString("name2", "logbugs");
event_properties.SetString("name3", "name3");
event_properties.SetNumber("test_number_int", 3);
event_properties.SetNumber("test_number_double", 3.14);
event_properties.SetBool("test_bool", true);
std::string test_string = "测试字符串1";
event_properties.SetString("test_stl_string1", test_string);
timeb t = {};
ftime(&t);
event_properties.SetDateTime("time11", t.time, t.millitm);
std::vector<std::string> test_list;
test_list.push_back("item11");
test_list.push_back("item21");
event_properties.SetList("test_list1", test_list);
// track事件
(*p_tga).track(accountId, distincId, eventName, event_properties);


// track事件
(*p_tga).track(accountId, distincId, eventName, event_properties);
```

## 对象和对象组：

```C++
string accountId = "shp";
string distincId = "shp666";
string eventName = "track_envet";


TaSDK::PropertiesNode _properties;
_properties.SetString("name1", "XZ_debug");
_properties.SetString("name2", "logbugs");
_properties.SetString("name3", "name3");
_properties.SetNumber("test_number_int", 3);
_properties.SetNumber("test_number_double", 3.14);
_properties.SetBool("test_bool", true);
std::string test_string1 = "测试字符串1";
_properties.SetString("test_stl_string1", test_string1);
timeb t1 = {};
ftime(&t1);
_properties.SetDateTime("time2", t1.time, t1.millitm);
std::vector<std::string> list;
list.push_back("item11");
list.push_back("item21");
_properties.SetList("test_list1", list);


TaSDK::PropertiesNode event_properties;
event_properties.SetString("name1", "XZ_debug");
event_properties.SetString("name2", "logbugs");
event_properties.SetString("name3", "name3");
event_properties.SetNumber("test_number_int", 3);
event_properties.SetNumber("test_number_double", 3.14);
event_properties.SetBool("test_bool", true);
std::string test_string = "测试字符串1";
event_properties.SetString("test_stl_string1", test_string);
timeb t = {};
ftime(&t);
event_properties.SetDateTime("time11", t.time, t.millitm);
std::vector<std::string> test_list;
test_list.push_back("item11");
test_list.push_back("item21");
event_properties.SetList("test_list1", test_list);

// 对象
event_properties.SetObject("obj", _properties);

// 对象组
std::vector<TaSDK::TAJSONObject> list_objs;
list_objs.push_back(_properties);
list_objs.push_back(_properties);
event_properties.SetList("objs", list_objs);


// track事件
(*p_tga).track(accountId, distincId, eventName, event_properties);

```


## 可更新事件、可重写事件

```c++
// 可更新事件
eventName = "update_event";
string updateEventId = "update_001";
TaSDK::PropertiesNode update_event_properties;
update_event_properties.SetString("price", "100");
update_event_properties.SetBool("status", 0);
// 上报后事件属性 status 为 3, price 为 100
(*p_tga).track_update(accountId, distincId, eventName, updateEventId, update_event_properties);

TaSDK::PropertiesNode update_event_new_properties;
update_event_new_properties.SetBool("status", 1);
// 上报事件后, 属性 status 被更新为 5, price 不变
(*p_tga).track_update(accountId, distincId, eventName, updateEventId, update_event_new_properties);


// 可重写事件
eventName = "overWrite_event";
string overWriteEventId = "overWrite_001";
TaSDK::PropertiesNode overWrite_event_properties;
overWrite_event_properties.SetString("money", "99");
overWrite_event_properties.SetString("code", "10");
// 上报后事件属性 code 为 10, money 为 99
(*p_tga).track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_properties);

TaSDK::PropertiesNode overWrite_event_new_properties;
overWrite_event_new_properties.SetString("money", "66");
// 上报后事件属性 money 被更新为 66, code 属性被删除
(*p_tga).track_overwrite(accountId, distincId, eventName, overWriteEventId, overWrite_event_new_properties);

```


## 2.2 设置公共事件属性
对于一些需要出现在所有事件中的属性属性，您可以调用setSupperProperties来设置公共事件属性，我们推荐您在发送事件前，先设置公共事件属性。
```c++
TaSDK::PropertiesNode superProperties;
superProperties.SetString("test_superProperties", "test_super");
(*p_tga).setSupperProperties(superProperties);
```

# 三、用户属性
TA 平台目前支持的用户属性设置接口为ta_user_set、ta_user_setOnce、ta_user_add、ta_user_del、ta_user_append。


## 3.1 user_set
对于一般的用户属性，您可以调用user_set来进行设置，使用该接口上传的属性将会覆盖原有的属性值，如果之前不存在该用户属性，则会新建该用户属性，类型与传入属性的类型一致：
```C++
    // user_set
    TaSDK::PropertiesNode userSet_properties;
    userSet_properties.SetString("name1", "test1");
    userSet_properties.SetString("name2", "test2");
    //上传用户属性
    (*p_tga).user_set(accountId, distincId, userSet_properties);

    userSet_properties.Clear();
    userSet_properties.SetString("user_name", "123");
    //再次上传用户属性，此时"user_name"的值会被覆盖为"123"
    (*p_tga).user_set(accountId, distincId, userSet_properties);

```


## 3.2 user_setOnce
如果您要上传的用户属性只要设置一次，则可以调用user_setOnce来进行设置，当该属性之前已经有值的时候，将会忽略这条信息：

```C++
    // user_setOnce
    TaSDK::PropertiesNode userSetOnce_properties;
    userSetOnce_properties.SetString("user_one_name", "ABC");
    //上传用户属性, 新建"user_one_name"，值为"ABC"
    (*p_tga).user_setOnce(accountId, distincId, userSetOnce_properties);

    userSetOnce_properties.Clear();
    userSetOnce_properties.SetString("user_one_name", "xyz");
    userSetOnce_properties.SetString("user_age", "18");
    // 再次上传用户属性，此时"user_one_name"的值不会被覆盖，仍为"ABC"；"user_age"的值为18
    (*p_tga).user_setOnce(accountId, distincId, userSetOnce_properties);


```


## 3.3 user_add
当您要上传数值型的属性时，您可以调用user_add来对该属性进行累加操作，如果该属性还未被设置，则会赋值 0 后再进行计算，可传入负值，等同于相减操作。

```C++
    // user_add
    TaSDK::PropertiesNode userAdd_properties;
    userAdd_properties.SetNumber("cash", 66);
    // 上传用户属性，此时"cash"的值为66
    (*p_tga).user_add(accountId, distincId, userAdd_properties);

    userAdd_properties.Clear();
    userAdd_properties.SetNumber("cash", 33);
    //再次上传用户属性，此时"cash"的值会累加为99
    (*p_tga).user_add(accountId, distincId, userAdd_properties);

```
设置的属性key为字符串，Value 只允许为数值。


## 3.4 user_del
如果您要删除某个用户，可以调用user_del将这名用户删除，您将无法再查询该名用户的用户属性，但该用户产生的事件仍然可以被查询到
```c++
    // user_del
    (*p_tga).user_del(accountId, distincId);
```

##3.5 user_append
```c++
   // user_append
    TaSDK::PropertiesNode userAppend_properties;
    vector<string> userAppenListValue;
    userAppenListValue.push_back("11");
    userAppenListValue.push_back("33");
    userAppend_properties.SetList("arr1", userAppenListValue);
    // 为arr1的数组类型追加属性
    (*p_tga).user_append(accountId, distincId, userAppend_properties);

```

## 3.6 user_unset
当您需要清空某个用户的用户属性的值时，可以调用 user_unset 进行清空。
```c++
    // user_unset
    TaSDK::PropertiesNode userUnset_properties;
    userUnset_properties.SetNumber("user_name", 0);
    // 清空该用户user_name属性值
    (*p_tga).user_unset(accountId, distincId, userUnset_properties);

```


# 四、其他操作

## 4.1 立即提交数据
```C++
(*p_tga).flush();
```
立即提交数据到相应的接收器

