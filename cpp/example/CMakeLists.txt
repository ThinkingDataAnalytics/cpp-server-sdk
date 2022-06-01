# 此处指定可执行文件的名称，这个可执行程序program就是用来测试library的用例
add_executable(program program.cpp)

if (WIN32)
    # 引用自己开发的library，第三方库，共享库，静态库等
    add_compile_definitions(BUILDING_LIBCURL)
    include_directories(.. ../thirdParty/curl/include)
    link_directories(.. ../thirdParty/curl/lib)
    target_link_libraries(program pcre_x86 libcurl)
    #target_link_libraries(program pcre_x86 libcurl ws2_32 winmm wldap32)
    target_link_libraries(program tacpp) # target_link_libraries：给目标指定链接的第三方库的路径位置
else()
    # 引用自己开发的library，第三方库，共享库，静态库等
    target_include_directories(program PRIVATE .. ../)  # target_include_directories：给目标指定头文件包含路径
    target_link_libraries(program curl libz.a)
    target_link_libraries(program tacpp) # target_link_libraries：给目标指定链接的第三方库的路径位置
endif ()
