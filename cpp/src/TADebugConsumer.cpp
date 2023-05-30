//
// Created by TD on 2022/10/25.
//

#include "TADebugConsumer.h"

#include <utility>

namespace TaSDK {

    TADebugConsumer::TADebugConsumer(string appid, const string& serverUrl, string certPath, string deviceId): m_appId(std::move(appid)), m_deviceId(std::move(deviceId)), m_certPath(std::move(certPath)) {
        this->m_serverUrl = serverUrl+"/data_debug";
        m_batchSize = 1;

        cout << "[ThinkingEngine] appid: " << m_appId << " serverUrl: " << m_serverUrl << endl;

        if (!m_deviceId.empty()) {
            cout << "[ThinkingEngine] debug deviceId: " << m_deviceId << endl;
        }
        if (!m_certPath.empty()) {
            cout << "[ThinkingEngine] CA cert path: " << m_certPath << endl;
        }
    }

    TADebugConsumer::~TADebugConsumer(void) = default;

    void TADebugConsumer::add(const std::string &record) {
        m_mutex.lock();

        m_dataList.push_back(record);
        if (m_dataList.size() >= m_batchSize) {
            sendData();
        }

        m_mutex.unlock();
    }

    void TADebugConsumer::flush() {
        m_mutex.lock();
        sendData();
        m_mutex.unlock();
    }

    void TADebugConsumer::close() {
        flush();
    }

    void TADebugConsumer::sendData() {
        string strResponse;
        string data;
        size_t size = m_dataList.size();
        if (size <= 0) return;
        for (int32_t i = 0; i < size; i++) {
            data.append(m_dataList[i]);
            data.append(",");
        }
        data = data.substr(0, data.length() - 1);
        
        int32_t dryRun = 1;
        int64_t code = TaSDK::TANetwork::debug_post(m_serverUrl, m_appId, data, strResponse, dryRun, m_deviceId, m_certPath);

        cout << "[ThinkingEngine] code: " << code << " response: " << strResponse << endl;

        m_dataList.erase(m_dataList.begin(), (m_dataList.begin() + (int64_t)size));
    }
}