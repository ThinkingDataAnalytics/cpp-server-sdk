cmake_minimum_required(VERSION 3.23)

project(cpp-server-sdk)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

# log consumer
add_library(tacpp src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TALoggerConsumer.cpp)
add_executable(program example/program.cpp)
target_link_libraries(program tacpp)

