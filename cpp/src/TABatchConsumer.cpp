//
// Created by TD on 2022/10/25.
//
#include "TABatchConsumer.h"

#include <utility>

namespace TaSDK {

    TABatchConsumer::TABatchConsumer(string appid, const string& serverUrl, int32_t batchSize, bool debug, string certPath):
        m_appId(std::move(appid)), m_debug(debug), m_batchSize(batchSize), m_certPath(std::move(certPath))
    {
        m_serverUrl = serverUrl+"/sync_server";

        cout << "[ThinkingEngine] appid: " << m_appId << " serverUrl: " << m_serverUrl << endl;

        if (!m_certPath.empty()) {
            cout << "[ThinkingEngine] CACert path: " << m_certPath << endl;
        }
    }

    TABatchConsumer::~TABatchConsumer(void) = default;


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
        string data = "[";
        size_t size = m_dataList.size();

        if (size <= 0) return;

        cout << "[ThinkingEngine] flush data count: " << size << endl;

        for (size_t i = 0; i < size; i++) {
            data.append(m_dataList[i]);
            data.append(",");
        }
        data = data.substr(0, data.length() - 1);
        data.append("]");

        if (m_debug) {
            cout << "[ThinkingEngine] data:" << endl << data << endl;
        }

        int64_t code = TaSDK::TANetwork::post(m_serverUrl, m_appId, data, int64_t(data.size()), strResponse, m_certPath);

        cout << "[ThinkingEngine] code: " << code << " response: " << strResponse << endl;

        if (code == 200) {   
            m_dataList.erase(m_dataList.begin(), m_dataList.begin() + (int64_t)(size));
        }
    }

    void TABatchConsumer::close() {
        flush();
    }
}
