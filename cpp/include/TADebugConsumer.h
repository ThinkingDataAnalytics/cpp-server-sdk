//
// Created by wwango on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TADEBUGCONSUMER_H
#define CPP_SERVER_SDK_TADEBUGCONSUMER_H
#include "TAConsumer.h"
#include <vector>
#include <mutex>
#include "TANetwork.h"

namespace TaSDK {

    using namespace std;

    class TADebugConsumer : public TAConsumer {
    public:
        TADebugConsumer(string appid, string serverUrl, string certPath = "", string deviceId = "");
        ~TADebugConsumer();
        void add(const string &record);
        void flush();
        void close();

    private:
        string m_serverUrl;
        string m_appId;
        string m_deviceId;
        int32_t m_batchSize;
        vector<string> m_dataList;
        TANetwork m_network;
        mutex m_mutex;
        string m_certPath;
        void sendData();
    };
};

#endif //CPP_SERVER_SDK_TADEBUGCONSUMER_H
