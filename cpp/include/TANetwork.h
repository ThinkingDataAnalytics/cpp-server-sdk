//
// Created by TD on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TD_NETWORK_H
#define CPP_SERVER_SDK_TD_NETWORK_H

#include <iostream>

namespace TaSDK {
    using namespace std;

    class TANetwork {
    public:
        TANetwork();
        ~TANetwork();
        
        static int64_t post(const string &url, const string &appid, string &data, int64_t dataSize, string &strResponse, const string &certPath);
        static int64_t
        debug_post(const string &url, const string &appid, string &data, string &strResponse, int32_t dryRun,
                   const string &deviceId, const string &certPath);
    };
}

#endif //CPP_SERVER_SDK_TD_NETWORK_H
