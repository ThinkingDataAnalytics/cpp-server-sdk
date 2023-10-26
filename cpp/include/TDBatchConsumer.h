//
// Created by TD on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TD_BATCH_CONSUMER_H
#define CPP_SERVER_SDK_TD_BATCH_CONSUMER_H
#include "TDConsumer.h"
#include <vector>
#include "TDNetwork.h"
#include <mutex>

namespace thinkingDataAnalytics {

    class TDBatchConsumer : public TDConsumer {
    public:
        /**
         * Construct
         * @param appid The app id of your project
         * @param serverUrl The server url of your project
         * @param batchSize buffer size
         * @param debug Deprecate. Value will be ignored
         * @param certPath SSL certificate path. Default is null
         */
        TDBatchConsumer(std::string appid, const std::string& serverUrl, int32_t batchSize = 20, bool debug = false, std::string certPath = "");
        ~TDBatchConsumer() override;
        void add(const std::string &record) override;
        void flush() override;
        void close() override;

    private:
        bool m_debug;
        std::string m_serverUrl;
        std::string m_appId;
        int32_t m_batchSize;
        std::vector<std::string> m_dataList;
        TDNetwork m_network;
        std::mutex m_mutex;
        std::string m_certPath;
        void sendData();
    };
}

#endif //CPP_SERVER_SDK_TD_BATCH_CONSUMER_H
