//
// Created by 沈和平 on 2022/5/15.
//

#ifndef CPP_TANETWORKING_H
#define CPP_TANETWORKING_H

#include <iostream>
#include <map>
#include <vector>

#include <curl/curl.h>
#include <zlib.h>
#include <algorithm>
#include <functional>

typedef struct {
    long status;
    char *body;
    unsigned long body_size;

} HttpResponse;

HttpResponse *ta_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds, const char *lib_name, const char *lib_version);

HttpResponse *ta_debug_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds, bool dryRun);

void destroy_http_response(HttpResponse *response);


#endif //CPP_TANETWORKING_H
