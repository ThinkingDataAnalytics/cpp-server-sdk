//
// Created by 沈和平 on 2022/5/15.
//

#include "../include/tacpp/TANetworking.h"

#define TA_SAFE_MALLOC(n) ta_safe_malloc((n), __LINE__)

void *ta_safe_malloc(unsigned long n, unsigned long line);

#define TA_SAFE_REALLOC(p, n) ta_safe_realloc((p), (n), __LINE__)

void *ta_safe_realloc(void *p, unsigned long n, unsigned long line);

#define TA_SAFE_FREE(curr) ta_safe_free((curr))

void ta_safe_free(void *curr);

#define out_of_memory(l, n) do {                    \
fprintf(stderr, "[%ld] Out of memory(%lu bytes).\n", l, (unsigned long)n);    \
} while (0)

void *ta_safe_malloc(unsigned long n, unsigned long line) {
    void *ptr = malloc(n);
    if (ptr == NULL) {
        out_of_memory(line, n);
    }
    return ptr;
}

void *ta_safe_realloc(void *p, unsigned long n, unsigned long line) {
    void *ptr = realloc(p, n);
    if (ptr == NULL) {
        out_of_memory(line, n);
    }
    return ptr;
}

void ta_safe_free(void *curr) {
    if (curr) {
        free(curr);
        curr = NULL;
    }
}

static HttpResponse *create_http_response() {
    HttpResponse *response = (HttpResponse *) TA_SAFE_MALLOC(sizeof(HttpResponse));


    if (response == NULL) {
        return NULL;
    }
    response->body = NULL;
    response->body_size = 0;
    return response;
}

void destroy_http_response(HttpResponse *response) {
    if (response == NULL) {
        return;
    }
    TA_SAFE_FREE(response->body);
    response->body = NULL;

    TA_SAFE_FREE(response);
}

static size_t write_call_back(void *data, size_t size, size_t nmemb,
                              void *user_data) {
    HttpResponse *response = (HttpResponse *) user_data;
    size_t total_size = size * nmemb;

    response->body = (char *) TA_SAFE_REALLOC(response->body, response->body_size + total_size + 1);
    if (response->body == NULL) {
        return (size_t) 0;
    }

    memcpy(response->body + response->body_size, data, total_size);
    response->body_size += total_size;
    response->body[response->body_size] = '\0';

    return total_size;
}

HttpResponse *ta_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds, const char *lib_name, const char *lib_version) {
    CURL *curl;
    CURLcode res;
    HttpResponse *response;
    struct curl_slist *header_list = NULL;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "curl_easy_init() failed\n");
        return NULL;
    }

    response = create_http_response();
    if (response == NULL) {
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data_length);

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

        snprintf(buffer, 50, "TA-Integration-Count: %d", data_size);
        header_list = curl_slist_append(header_list, buffer);

        header_list = curl_slist_append(header_list, "compress: none");

        snprintf(buffer, 50, "appid: %s", appid);
        header_list = curl_slist_append(header_list, buffer);

        snprintf(buffer, 50, "TA-Integration-Type: %s", lib_name);
        header_list = curl_slist_append(header_list, buffer);

        snprintf(buffer, 50, "TA-Integration-Version: %s", lib_version);
        header_list = curl_slist_append(header_list, buffer);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    if (timeout_seconds > 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_seconds);
        // dont want to get a sig alarm on timeout
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    res = curl_easy_perform(curl);
    curl_slist_free_all(header_list);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        destroy_http_response(response);
        return NULL;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->status);
    curl_easy_cleanup(curl);
    return response;
}

HttpResponse *ta_debug_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds, bool dryRun)
{

    CURL *curl;
    CURLcode res;
    HttpResponse *response;
    struct curl_slist *header_list = NULL;
    char *final_encodedata_;
    char *final_datas_;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "curl_easy_init() failed\n");
        return NULL;
    }

    final_encodedata_ = curl_easy_escape(curl, data, strlen(data));

    final_datas_ = (char *) malloc(strlen("appid=&source=server&dryRun=1&data=") + strlen(appid) + strlen(final_encodedata_) );
    strcpy(final_datas_, "appid=");
    strcat(final_datas_, appid);
    strcat(final_datas_, "&source=server&");
    strcat(final_datas_, "dryRun=");
    if (!dryRun) {
        strcat(final_datas_, "1");
    } else {
        strcat(final_datas_, "0");
    }
    strcat(final_datas_, "&data=");
    strcat(final_datas_, final_encodedata_);

    response = create_http_response();
    if (response == NULL) {
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, final_datas_);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(final_datas_));

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

    if (timeout_seconds > 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_seconds);
        // dont want to get a sig alarm on timeout
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    res = curl_easy_perform(curl);
    curl_slist_free_all(header_list);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        destroy_http_response(response);
        return NULL;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->status);
    curl_easy_cleanup(curl);

    return response;
}