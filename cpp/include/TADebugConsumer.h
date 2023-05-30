//
// Created by TD on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TD_DEBUG_CONSUMER_H
#define CPP_SERVER_SDK_TD_DEBUG_CONSUMER_H
#include "TAConsumer.h"
#include <vector>
#include <mutex>
#include "TANetwork.h"

namespace TaSDK {

    using namespace std;

    class TADebugConsumer : public TAConsumer {
    public:
        TADebugConsumer(string appid, const string& serverUrl, string certPath = "", string deviceId = "");
        ~TADebugConsumer() override;
        void add(const string &record) override;
        void flush() override;
        void close() override;

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
}

#endif //CPP_SERVER_SDK_TD_DEBUG_CONSUMER_H
