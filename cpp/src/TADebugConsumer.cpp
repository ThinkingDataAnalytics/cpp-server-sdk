//
// Created by wwango on 2022/10/25.
//

#include "TADebugConsumer.h"

namespace TaSDK {

    TADebugConsumer::TADebugConsumer(string appid, string serverUrl, string certPath, string deviceId): m_appId(appid), m_deviceId(deviceId), m_certPath(certPath) {
        this->m_serverUrl = serverUrl+"/data_debug";
        m_batchSize = 1;
        m_network = TANetwork();

        cout << "[ThinkingEngine] appid: " << m_appId << " serverUrl: " << m_serverUrl << endl;

        if (m_deviceId.size()) {
            cout << "[ThinkingEngine] debug deviceId: " << m_deviceId << endl;
        }
        if (m_certPath.size()) {
            cout << "[ThinkingEngine] cacert path: " << m_certPath << endl;
        }
    }

    TADebugConsumer::~TADebugConsumer(void) {}

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
        string datas = "";
        size_t size = m_dataList.size();
        if (size <= 0) return;
        for (int32_t i = 0; i < size; i++) {
            datas.append(m_dataList[i]);
            datas.append(",");
        }
        datas = datas.substr(0, datas.length() - 1);
        
        int32_t dryRun = 1;
        int64_t code = m_network.debug_post(m_serverUrl, m_appId, datas, datas.size(), strResponse, dryRun, m_deviceId, m_certPath);

        cout << "[ThinkingEngine] code: " << code << " response: " << strResponse << endl;

        m_dataList.erase(m_dataList.begin(), m_dataList.begin() + (size));
    }
};