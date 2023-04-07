//
// Created by wwango on 2022/10/25.
//
#include "TABatchConsumer.h"

namespace TaSDK {

    TABatchConsumer::TABatchConsumer(string appid, string serverUrl, int32_t batchSize, bool debug, string certPath):
        m_appId(appid), m_debug(debug), m_batchSize(batchSize), m_certPath(certPath)
    {
        m_serverUrl = serverUrl+"/sync_server";
        m_network = TANetwork();

        cout << "[ThinkingEngine] appid: " << m_appId << " serverUrl: " << m_serverUrl << endl;

        if (m_certPath.size()) {
            cout << "[ThinkingEngine] cacert path: " << m_certPath << endl;
        }
    }

    TABatchConsumer::~TABatchConsumer(void) {}


    void TABatchConsumer::add(const std::string &record) {
        m_mutex.lock();

        m_dataList.emplace_back(record);
        if (m_dataList.size() >= m_batchSize) {
            sendData();
        }

        m_mutex.unlock();
    }

    void TABatchConsumer::flush() {
        m_mutex.lock();
        sendData();
        m_mutex.unlock();
    }

    void TABatchConsumer::sendData() {

        string strResponse;
        string datas = "[";
        size_t size = m_dataList.size();

        if (size <= 0) return;

        cout << "[ThinkingEngine] flush data count: " << size << endl;

        for (size_t i = 0; i < size; i++) {
            datas.append(m_dataList[i]);
            datas.append(",");
        }
        datas = datas.substr(0, datas.length() - 1);
        datas.append("]");

        if (m_debug) {
            cout << "[ThinkingEngine] data:" << endl << datas << endl;
        }

        int64_t code = m_network.post(m_serverUrl, m_appId, datas, datas.size(), strResponse, m_certPath);

        cout << "[ThinkingEngine] code: " << code << " response: " << strResponse << endl;

        if (code == 200) {   
            m_dataList.erase(m_dataList.begin(), m_dataList.begin() + (size));
        }
    }

    void TABatchConsumer::close() {
        flush();
    }
};
