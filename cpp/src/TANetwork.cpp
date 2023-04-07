//
// Created by wwango on 2022/10/25.
//

#include "TANetwork.h"
#include "TAUtils.h"
#include <curl/curl.h>
#include <inttypes.h>

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
    return nmemb;
}


namespace TaSDK {
    TANetwork::TANetwork(void)
    {

    }

    TANetwork::~TANetwork(void)
    {

    }

    int64_t TANetwork::post(const string& url, const string& appid, string& data, int64_t dataSize, string& strResponse, const string& certPath)
    {
        CURL *curl;
        CURLcode res;
        int32_t timeout_seconds = 10;
        struct curl_slist *header_list = NULL;

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
        if (certPath.size())
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

        if (timeout_seconds > 0) {
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_seconds);
            // dont want to get a sig alarm on timeout
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        }

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

    int64_t TANetwork::debug_post(const string &url, const string &appid, string &data, int64_t dataSize, string &strResponse, int32_t dryRun, string deviceId, const string& certPath)
    {
        CURL *curl;
        CURLcode res;
        struct curl_slist *header_list = NULL;
        string final_encodedata_;
        string final_datas_;

        curl = curl_easy_init();
        if (!curl) {
            fprintf(stderr, "curl_easy_init() failed\n");
            return -1;
        }

        final_datas_ = "appid=" + appid + "&source=server" + "&dryRun=" + to_string(dryRun) + "&data=" + data;
        if (deviceId.size() > 0)
        {
            final_datas_ = final_datas_ + "&deviceId=" + deviceId;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, final_datas_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, final_datas_.length());

        // ssl
        if (certPath.size())
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
