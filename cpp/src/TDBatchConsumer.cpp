//
// Created by TD on 2022/10/25.
//
#include "TDBatchConsumer.h"
#include "TDUtils.h"
#include <utility>

namespace thinkingDataAnalytics {

    TDBatchConsumer::TDBatchConsumer(std::string appid, const std::string& serverUrl, int32_t batchSize, bool debug, std::string certPath):
        m_appId(std::move(appid)), m_debug(debug), m_batchSize(batchSize), m_certPath(std::move(certPath))
    {
        m_serverUrl = serverUrl+"/sync_server";

        TDLog::info("appId: " + m_appId + " serverUrl: " + m_serverUrl);

        if (!m_certPath.empty()) {
            TDLog::info("CA cert path: " + m_certPath);
        }
    }

    TDBatchConsumer::~TDBatchConsumer(void) = default;


    void TDBatchConsumer::add(const std::string &record) {
        m_mutex.lock();

        m_dataList.emplace_back(record);

        TDLog::info("Enqueue data to buffer. Buffer count: " + std::to_string(m_dataList.size()));

        if (m_dataList.size() >= m_batchSize) {
            sendData();
        }

        m_mutex.unlock();
    }

    void TDBatchConsumer::flush() {
        TDLog::info("TDBatchConsumer flush data.");
        m_mutex.lock();
        sendData();
        m_mutex.unlock();
    }

    void TDBatchConsumer::sendData() {

        std::string strResponse;
        std::string data = "[";
        size_t size = m_dataList.size();

        if (size <= 0) return;

        for (size_t i = 0; i < size; i++) {
            data.append(m_dataList[i]);
            data.append(",");
        }
        data = data.substr(0, data.length() - 1);
        data.append("]");

        TDLog::info("Send data, request: " + data);

        int64_t code = thinkingDataAnalytics::TDNetwork::post(m_serverUrl, m_appId, data, int64_t(data.size()), strResponse, m_certPath);

        TDLog::info("Send data, response: " + strResponse);

        if (code == 200) {   
            m_dataList.erase(m_dataList.begin(), m_dataList.begin() + (int64_t)(size));
        }
    }

    void TDBatchConsumer::close() {
        flush();
        TDLog::info("TDBatchConsumer close.");
    }
}
