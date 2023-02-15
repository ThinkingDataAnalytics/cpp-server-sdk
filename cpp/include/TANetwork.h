//
// Created by wwango on 2022/10/25.
//

#ifndef CPP_SERVER_SDK_TANETWORK_H
#define CPP_SERVER_SDK_TANETWORK_H

#include <iostream>

namespace TaSDK {
    using namespace std;

    class TANetwork {
    public:
        TANetwork(void);
        ~TANetwork(void);
        
        int post(const string &url, const string &appid, string &data, int32_t dataSize, string &strResponse);
        int64_t debug_post(const string &url, const string &appid, string &data, int32_t dataSize, string &strResponse, int32_t dryRun);
    };
};

#endif //CPP_SERVER_SDK_TANETWORK_H
