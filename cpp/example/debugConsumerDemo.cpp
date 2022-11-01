//
// Created by wwango on 2022/10/25.
//
#include "../include/ThinkingAnalyticsAPI.h"
#include "../include/TADebugConsumer.h"

using namespace TaSDK;

int main(int argc, char *argv[]) {

    string accountId = "shp";
    string distincId = "shp666";

    TADebugConsumer debugConsumer = TADebugConsumer("35a15b58ae934f3994c1abf77910e390","http://receiver.ta.thinkingdata.cn");
    ThinkingDataAnalytics tga = ThinkingDataAnalytics(debugConsumer, true);

    // 设置公共属性
    TaSDK::PropertiesNode superProperties;
    superProperties.SetString("kkk", "vvv");
    tga.setSupperProperties(superProperties);
    for(int i = 0; i<21; i++) {
        tga.track(accountId, distincId, "event", superProperties);
    }

    return 0;
}