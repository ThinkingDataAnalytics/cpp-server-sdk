cmake_minimum_required(VERSION 3.12)
project(cpp-server-sdk)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

# log consumer
add_library(thinkingdata src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TALoggerConsumer.cpp)
add_executable(program example/program.cpp)
target_link_libraries(program thinkingdata)

#
## batch consumer
#add_library(thinkingdata_batch src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TABatchConsumer.cpp src/TANetwork.cpp)
#if(WIN32)
#    include_directories(thirdParty/curl/include)
#    link_directories(thirdParty/curl/lib)
#    target_link_libraries(thinkingdata_batch libcurl)
#else ()
#    target_link_libraries(thinkingdata_batch curl)
#endif()
#add_executable(batchConsmerDemo example/batchConsmerDemo.cpp)
#target_link_libraries(batchConsmerDemo thinkingdata_batch)

## debug consumer
#add_library(thinkingdata_debug src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TADebugConsumer.cpp src/TANetwork.cpp)
#if(WIN32)
#    include_directories(thirdParty/curl/include)
#    link_directories(thirdParty/curl/lib)
#    target_link_libraries(thinkingdata_debug libcurl)
#else ()
#    target_link_libraries(thinkingdata_debug curl)
#endif()
#add_executable(debugConsmerDemo example/debugConsumerDemo.cpp)
#target_link_libraries(debugConsmerDemo thinkingdata_debug)
