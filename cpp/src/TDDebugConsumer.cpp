//
// Created by TD on 2022/10/25.
//

#include "TDDebugConsumer.h"
#include "TDUtils.h"
#include <utility>

namespace thinkingDataAnalytics {

    TDDebugConsumer::TDDebugConsumer(std::string appid, const std::string& serverUrl, std::string certPath, std::string deviceId): m_appId(std::move(appid)), m_deviceId(std::move(deviceId)), m_certPath(std::move(certPath)) {
        this->m_serverUrl = serverUrl+"/data_debug";
        m_batchSize = 1;

        TDLog::info("appId: " + m_appId + ", serverUrl: " + m_serverUrl + ", deviceId: " + m_deviceId);

        if (!m_certPath.empty()) {
            TDLog::info("CA cert path: " + m_certPath);
        }
    }

    TDDebugConsumer::~TDDebugConsumer(void) = default;

    void TDDebugConsumer::add(const std::string &record) {
        m_mutex.lock();

        m_dataList.push_back(record);
        if (m_dataList.size() >= m_batchSize) {
            sendData();
        }

        m_mutex.unlock();
    }

    void TDDebugConsumer::flush() {
        m_mutex.lock();
        sendData();
        m_mutex.unlock();
    }

    void TDDebugConsumer::close() {
        flush();
    }

    void TDDebugConsumer::sendData() {
        std::string strResponse;
        std::string data;
        size_t size = m_dataList.size();
        if (size <= 0) return;
        for (int32_t i = 0; i < size; i++) {
            data.append(m_dataList[i]);
            data.append(",");
        }
        data = data.substr(0, data.length() - 1);

        TDLog::info("Send data, request: " + data);

        int32_t dryRun = 1;
        int64_t code = thinkingDataAnalytics::TDNetwork::debug_post(m_serverUrl, m_appId, data, strResponse, dryRun, m_deviceId, m_certPath);

        TDLog::info("Send data, response: " + strResponse);

        m_dataList.erase(m_dataList.begin(), (m_dataList.begin() + (int64_t)size));
    }
}