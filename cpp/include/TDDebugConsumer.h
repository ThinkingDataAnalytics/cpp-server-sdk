//
// Created by TD on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TD_DEBUG_CONSUMER_H
#define CPP_SERVER_SDK_TD_DEBUG_CONSUMER_H
#include "TDConsumer.h"
#include <vector>
#include <mutex>
#include "TDNetwork.h"

namespace thinkingDataAnalytics {

    class TDDebugConsumer : public TDConsumer {
    public:
        /**
         * Construct
         * @param appid The app id of your project
         * @param serverUrl The server url of your project
         * @param certPath SSL certificate path. Default is null
         * @param deviceId Debug device id
         */
        TDDebugConsumer(std::string appid, const std::string& serverUrl, std::string certPath = "", std::string deviceId = "");
        ~TDDebugConsumer() override;
        void add(const std::string &record) override;
        void flush() override;
        void close() override;

    private:
        std::string m_serverUrl;
        std::string m_appId;
        std::string m_deviceId;
        int32_t m_batchSize;
        std::vector<std::string> m_dataList;
        TDNetwork m_network;
        std::mutex m_mutex;
        std::string m_certPath;
        void sendData();
    };
}

#endif //CPP_SERVER_SDK_TD_DEBUG_CONSUMER_H
