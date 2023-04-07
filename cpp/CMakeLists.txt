cmake_minimum_required(VERSION 3.12)
project(ThinkingEngine)

message("[ThinkingEngine] current cmake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message("[ThinkingEngine] CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM} ")

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

# #
# # log consumer. Recommended for production environments
# #
# set(TE_LIB_NAME thinkingdata)
# add_library(${TE_LIB_NAME} src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TALoggerConsumer.cpp src/TAConsumer.cpp)
# if(UNIX)
#   find_package(Threads REQUIRED)
#   target_link_libraries(${TE_LIB_NAME} Threads::Threads)
# endif()


# #
# # debug consumer. Recommended for test
# #
# set(TE_LIB_NAME thinkingdata)
# add_library(${TE_LIB_NAME} src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TADebugConsumer.cpp src/TANetwork.cpp src/TAConsumer.cpp)
# if(WIN32)
#     include_directories(thirdParty/curl/include)
#     link_directories(thirdParty/curl/lib)
#     target_link_libraries(${TE_LIB_NAME} libcurl)
# else ()
#     target_link_libraries(${TE_LIB_NAME} curl)
#     find_package(Threads REQUIRED)
#     target_link_libraries(${TE_LIB_NAME} Threads::Threads)
# endif()


# #
# # batch consumer. can be used in production environments
# #
# set(TE_LIB_NAME thinkingdata)
# add_library(${TE_LIB_NAME} src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TABatchConsumer.cpp src/TANetwork.cpp src/TAConsumer.cpp)
# if(WIN32)
#     include_directories(thirdParty/curl/include)
#     link_directories(thirdParty/curl/lib)
#     target_link_libraries(${TE_LIB_NAME} libcurl)
# else ()
#     target_link_libraries(${TE_LIB_NAME} curl)
#     find_package(Threads REQUIRED)
#     target_link_libraries(${TE_LIB_NAME} Threads::Threads)
# endif()

#
# Only be used to test.
#
# The test project includes all the consumers(loggerConsumer, debugConsumer, batchConsumer). Do not use it in production
#
set(TE_LIB_NAME thinkingdata)
add_library(${TE_LIB_NAME} src/ThinkingAnalyticsAPI.cpp src/TAUtils.cpp src/TABatchConsumer.cpp src/TADebugConsumer.cpp src/TALoggerConsumer.cpp src/TANetwork.cpp src/TAConsumer.cpp)
if(WIN32)
    include_directories(thirdParty/curl/include)
    link_directories(thirdParty/curl/lib)
    target_link_libraries(${TE_LIB_NAME} libcurl)
else ()
    target_link_libraries(${TE_LIB_NAME} curl)
    find_package(Threads REQUIRED)
    target_link_libraries(${TE_LIB_NAME} Threads::Threads)
endif()
add_executable(program example/program.cpp)
target_link_libraries(program ${TE_LIB_NAME})
