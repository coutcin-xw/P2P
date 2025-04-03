# 网络模块：Socket 和 Address
add_library(Network STATIC
    Address.cpp
    Socket.cpp
    IPAddress.cpp
    Port.cpp
)

target_include_directories(Network PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
if(WIN32)
    target_link_libraries(Network PRIVATE ws2_32)  # Link Winsock2 on Windows
endif()