//
// Created by wwango on 2022/10/25.
//

#include "TANetwork.h"
#include "TAUtils.h"
#include <curl/curl.h>

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

    long TANetwork::post(const string &url, const string &appid, string &data, int dataSize, string &strResponse)
    {
        CURL *curl;
        CURLcode res;
        int timeout_seconds = 10;
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

        // 若使用 HTTPS，有两种配置方式，选用其中一种即可：

        // 1. 使用 CA 证书（下载地址 http://curl.haxx.se/ca/cacert.pem
        // ），去掉下面一行的注释，并指定证书路径，例如证书在当前目录下
        // curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");

        // 2. 不验证服务端证书，去掉下面两行的注释
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        {
            char buffer[64];

            header_list = curl_slist_append(header_list, "Content-Type: application/json");
            header_list = curl_slist_append(header_list, "compress: none");

            snprintf(buffer, 50, "appid: %s", appid.c_str());
            header_list = curl_slist_append(header_list, buffer);

            snprintf(buffer, 50, "TA-Integration-Type: %s", LIB_NAME.c_str());
            header_list = curl_slist_append(header_list, buffer);

            snprintf(buffer, 50, "TA-Integration-Version: %s", LIB_NAME.c_str());
            header_list = curl_slist_append(header_list, buffer);

            snprintf(buffer, 50, "TA-Integration-Count: %d", dataSize);
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
        long responseCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_cleanup(curl);
        return responseCode;
    }

    long TANetwork::debug_post(const string &url, const string &appid, string &data, int dataSize, string &strResponse, int dryRun)
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

        final_encodedata_ = string(curl_easy_escape(curl, data.c_str(), data.length()));

        final_datas_ = "appid=" + appid + "&source=server&" + "dryRun=" + to_string(dryRun) + "&data=" + final_encodedata_;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, final_datas_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, final_datas_.length());

        // 若使用 HTTPS，有两种配置方式，选用其中一种即可：

        // 1. 使用 CA 证书（下载地址 http://curl.haxx.se/ca/cacert.pem
        // ），去掉下面一行的注释，并指定证书路径，例如证书在当前目录下
        // curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");

        // 2. 不验证服务端证书，去掉下面两行的注释
//    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
//    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        {
            char buffer[64];
            header_list = curl_slist_append(header_list, buffer);
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

        long responseCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_cleanup(curl);
        return responseCode;
    }
}
