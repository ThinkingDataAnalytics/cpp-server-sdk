//
// Created by wwango on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TADEBUGCONSUMER_H
#define CPP_SERVER_SDK_TADEBUGCONSUMER_H
#include "TAConsumer.h"
#include <vector>
#include "TANetwork.h"

namespace TaSDK {

    using namespace std;

    class TADebugConsumer : public TAConsumer {
    public:
        TADebugConsumer(string appid, string serverUrl);
        ~TADebugConsumer();
        void add(const string &record);
        void flush();
        void close();

    private:
        bool m_debug;
        string m_serverUrl;
        string m_appId;
        int m_batchSize;
        vector<string> m_dataList;
        TANetwork m_network;
    };
};



#endif //CPP_SERVER_SDK_TADEBUGCONSUMER_H
