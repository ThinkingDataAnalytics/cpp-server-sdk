//
// Created by TD on 2022/10/25.
//

#include "TDNetwork.h"
#include "TDUtils.h"
#include <curl/curl.h>
#include <cinttypes>

static size_t OnWriteData(void* buffer, size_t size, size_t nMember, void* lpVoid)
{
    auto* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( nullptr == str || nullptr == buffer )
    {
        return -1;
    }

    char* pData = (char*)buffer;
    str->append(pData, size * nMember);
    return nMember;
}


namespace thinkingDataAnalytics {
    TDNetwork::TDNetwork(void) = default;

    TDNetwork::~TDNetwork(void) = default;

    int64_t TDNetwork::post(const std::string& url, const std::string& appid, std::string& data, int64_t dataSize, std::string& strResponse, const std::string& certPath)
    {
        CURL *curl;
        CURLcode res;
        int32_t timeout_seconds = 10;
        struct curl_slist *header_list = nullptr;

        curl = curl_easy_init();
        if (!curl) {
            fprintf(stderr, "curl_easy_init() failed\n");
            return -1;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());

        // ssl
        if (!certPath.empty())
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(curl, CURLOPT_CAINFO, certPath.data());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        {
            char buffer[64]{};

            header_list = curl_slist_append(header_list, "Content-Type: application/json");
            header_list = curl_slist_append(header_list, "compress: none");

            snprintf(buffer, 50, "appid: %s", appid.c_str());
            header_list = curl_slist_append(header_list, buffer);

            snprintf(buffer, 50, "TA-Integration-Type: %s", LIB_NAME.c_str());
            header_list = curl_slist_append(header_list, buffer);

            snprintf(buffer, 50, "TA-Integration-Version: %s", LIB_NAME.c_str());
            header_list = curl_slist_append(header_list, buffer);

            snprintf(buffer, 50, "TA-Integration-Count: %" PRId64, dataSize);
            header_list = curl_slist_append(header_list, buffer);

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        }

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_seconds);
        // dont want to get a sig alarm on timeout
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);

        res = curl_easy_perform(curl);
        curl_slist_free_all(header_list);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return -1;
        }
        int64_t responseCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_cleanup(curl);
        return responseCode;
    }

    int64_t
    TDNetwork::debug_post(const std::string &url, const std::string &appid, std::string &data, std::string &strResponse, int32_t dryRun,
                          const std::string &deviceId, const std::string &certPath)
    {
        CURL *curl;
        CURLcode res;
        struct curl_slist *header_list = nullptr;
        std::string final_data_;

        curl = curl_easy_init();
        if (!curl) {
            fprintf(stderr, "curl_easy_init() failed\n");
            return -1;
        }

        final_data_ = "appid=" + appid + "&source=server" + "&dryRun=" + std::to_string(dryRun) + "&data=" + data;
        if (!deviceId.empty())
        {
            final_data_ = final_data_ + "&deviceId=" + deviceId;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, final_data_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, final_data_.length());

        // ssl
        if (!certPath.empty())
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(curl, CURLOPT_CAINFO, certPath.data());
        }
        else
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        {
            char buffer[64]{};
            header_list = curl_slist_append(header_list, buffer);
            header_list = curl_slist_append(header_list, "Content-Type:application/x-www-form-urlencoded");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        }

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        // dont want to get a sig alarm on timeout
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);

        res = curl_easy_perform(curl);
        curl_slist_free_all(header_list);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return -1;
        }

        int64_t responseCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_cleanup(curl);
        return responseCode;
    }
}
