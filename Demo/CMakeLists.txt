cmake_minimum_required(VERSION 3.22)
project(Demo)

set(CMAKE_CXX_STANDARD 98)

link_directories(${PROJECT_SOURCE_DIR}/thirdPark/curl/lib ${PROJECT_SOURCE_DIR}/thirdPark/tacpp/lib)
include_directories(${PROJECT_SOURCE_DIR}/thirdPark/curl/include ${PROJECT_SOURCE_DIR}/thirdPark/tacpp/include)

add_executable(Demo main.cpp)
target_link_libraries(Demo curl libz.a)
target_link_libraries(Demo libtacpp.a)


