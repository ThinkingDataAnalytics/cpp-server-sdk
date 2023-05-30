cmake_minimum_required(VERSION 3.12)
project(ThinkingEngine)
set(CMAKE_CXX_STANDARD 11)


message(STATUS "[ThinkingEngine] current cmake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "[ThinkingEngine] CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM} ")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif()

message(STATUS "[ThinkingEngine] Build type:${CMAKE_BUILD_TYPE}")

message(STATUS "[ThinkingEngine] Debug configuration:${CMAKE_CXX_FLAGS_DEBUG}")

message(STATUS "[ThinkingEngine] release configuration:${CMAKE_CXX_FLAGS_RELEASE}")

message(STATUS "[ThinkingEngine] release configuration with debug info:${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

message(STATUS "[ThinkingEngine] minimal release configuration:${CMAKE_CXX_FLAGS_MINSIZEREL}")

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
