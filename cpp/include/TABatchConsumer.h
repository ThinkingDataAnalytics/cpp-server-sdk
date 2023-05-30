//
// Created by TD on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TD_BATCH_CONSUMER_H
#define CPP_SERVER_SDK_TD_BATCH_CONSUMER_H
#include "TAConsumer.h"
#include <vector>
#include "TANetwork.h"
#include <mutex>

namespace TaSDK {

    using namespace std;

    class TABatchConsumer : public TAConsumer {
    public:
    
        TABatchConsumer(string appid, const string& serverUrl, int32_t batchSize = 20, bool debug = false, string certPath = "");
        ~TABatchConsumer() override;
        void add(const string &record) override;
        void flush() override;
        void close() override;

    private:
        bool m_debug;
        string m_serverUrl;
        string m_appId;
        int32_t m_batchSize;
        vector<string> m_dataList;
        TANetwork m_network;
        mutex m_mutex;
        string m_certPath;
        void sendData();
    };
}

#endif //CPP_SERVER_SDK_TD_BATCH_CONSUMER_H
