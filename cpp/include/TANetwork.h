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
        long post(const string &url, const string &appid, string &data, int dataSize, string &strResponse);
        long debug_post(const string &url, const string &appid, string &data, int dataSize, string &strResponse, int dryRun);
    };
};

#endif //CPP_SERVER_SDK_TANETWORK_H
