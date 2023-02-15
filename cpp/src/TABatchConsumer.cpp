//
// Created by wwango on 2022/10/25.
//
#include "TABatchConsumer.h"

namespace TaSDK {

    TABatchConsumer::TABatchConsumer(string appid, string serverUrl, int32_t batchSize, bool debug) {
        m_appId = appid;
        m_batchSize = batchSize;
        m_serverUrl = serverUrl+"/sync_server";
        m_network = TANetwork();
        m_debug = debug;
    }

    TABatchConsumer::~TABatchConsumer(void) {}


    void TABatchConsumer::add(const std::string &record) {
        m_dataList.push_back(record);

        if (m_dataList.size() >= m_batchSize) {
            flush();
        }
    }

    void TABatchConsumer::flush() {
        string strResponse;
        string datas = "[";
        int32_t size = m_dataList.size();

        if (size <= 0) return;

        for (int32_t i=0; i<size; i++) {
            datas.append(m_dataList[i]);
            datas.append(",");
        }
        datas = datas.substr(0, datas.length() - 1);
        datas.append("]");

        int64_t code = m_network.post(m_serverUrl, m_appId, datas, size, strResponse);
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

    void TABatchConsumer::close() {
        flush();
    }
};
