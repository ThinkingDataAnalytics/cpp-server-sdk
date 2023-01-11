//
// Created by wwango on 2022/10/25.
//

#include "TADebugConsumer.h"

namespace TaSDK {

    TADebugConsumer::TADebugConsumer(string appid, string serverUrl) {
        m_appId = appid;
        m_serverUrl = serverUrl+"/data_debug";
        m_batchSize = 1;
        m_network = TANetwork();
        m_debug = true;
    }

    TADebugConsumer::~TADebugConsumer(void) {}

    void TADebugConsumer::add(const std::string &record) {
        m_dataList.push_back(record);

        if (m_dataList.size() >= m_batchSize) {
            flush();
        }
    }

    void TADebugConsumer::flush() {
        string strResponse;
        string datas = "[";
        int size = m_dataList.size();

        if (size <= 0) return;

        for (int i=0; i<size; i++) {
            datas.append(m_dataList[i]);
            datas.append(",");
        }
        datas = datas.substr(0, datas.length() - 1);
        datas.append("]");

        int dryRun = 1;
        long code = m_network.debug_post(m_serverUrl, m_appId, datas, size, strResponse, dryRun);
        if (code == 200) {
            if (m_debug) {
                fprintf(stdout, "thinkingdata network success: \n");
                fprintf(stdout, "appid:%s , serverUrl: %s \n", m_appId.c_str(), m_serverUrl.c_str());
                fprintf(stdout, "datas: %s \n", datas.c_str());
            }
            m_dataList.erase(m_dataList.begin(), m_dataList.begin()+(size));
        } else {
            if (m_debug) {
                fprintf(stderr, "thinkingdata network failed, errorcode: %ld \n error_message: %s\n", code,strResponse.c_str());
            }
        }
    }

    void TADebugConsumer::close() {
        flush();
    }
};