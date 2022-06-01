cmake_minimum_required(VERSION 3.22)
project(cpp)

set(CMAKE_CXX_STANDARD 98)

add_library(tacpp src/ThinkingAnalyticsAPI.cpp include/tacpp/ThinkingAnalyticsAPI.h src/TAUtils.cpp include/tacpp/TAUtils.h src/TANetworking.cpp include/tacpp/TANetworking.h include/tacpp/TAConsumer.h)

add_subdirectory(example)