//
// Created by TD on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TD_NETWORK_H
#define CPP_SERVER_SDK_TD_NETWORK_H

#include <iostream>

namespace thinkingDataAnalytics {

    class TDNetwork {
    public:
        TDNetwork();
        ~TDNetwork();
        
        static int64_t post(const std::string &url, const std::string &appid, std::string &data, int64_t dataSize, std::string &strResponse, const std::string &certPath);
        static int64_t
        debug_post(const std::string &url, const std::string &appid, std::string &data, std::string &strResponse, int32_t dryRun,
                   const std::string &deviceId, const std::string &certPath);
    };
}

#endif //CPP_SERVER_SDK_TD_NETWORK_H
