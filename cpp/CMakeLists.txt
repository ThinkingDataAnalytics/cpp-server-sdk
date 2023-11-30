cmake_minimum_required(VERSION 3.12)
project(ThinkingData)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

message(STATUS "[${PROJECT_NAME}] current cmake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
message(STATUS "[${PROJECT_NAME}] CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM} ")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE release CACHE STRING "Build Type" FORCE)
endif()

message(STATUS "[${PROJECT_NAME}] Build type:${CMAKE_BUILD_TYPE}")

message(STATUS "[${PROJECT_NAME}] Debug configuration:${CMAKE_CXX_FLAGS_DEBUG}")

message(STATUS "[${PROJECT_NAME}] release configuration:${CMAKE_CXX_FLAGS_RELEASE}")

message(STATUS "[${PROJECT_NAME}] release configuration with debug info:${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

message(STATUS "[${PROJECT_NAME}] minimal release configuration:${CMAKE_CXX_FLAGS_MINSIZEREL}")

include_directories(include)

# #
# # log consumer. Recommended for production environments
# #
# set(TE_LIB_NAME thinkingdata)
# add_library(${TE_LIB_NAME} src/TDAnalytics.cpp src/TDUtils.cpp src/TDLoggerConsumer.cpp src/TDConsumer.cpp)
# if(UNIX)
#   find_package(Threads REQUIRED)
#   target_link_libraries(${TE_LIB_NAME} Threads::Threads)
# endif()


# #
# # debug consumer. Recommended for test
# #
# set(TE_LIB_NAME thinkingdata)
# add_library(${TE_LIB_NAME} src/TDAnalytics.cpp src/TDUtils.cpp src/TDDebugConsumer.cpp src/TDNetwork.cpp src/TDConsumer.cpp)
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
# add_library(${TE_LIB_NAME} src/TDAnalytics.cpp src/TDUtils.cpp src/TDBatchConsumer.cpp src/TDNetwork.cpp src/TDConsumer.cpp)
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
add_library(${TE_LIB_NAME} src/TDAnalytics.cpp src/TDUtils.cpp src/TDBatchConsumer.cpp src/TDDebugConsumer.cpp src/TDLoggerConsumer.cpp src/TDNetwork.cpp src/TDConsumer.cpp)
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
