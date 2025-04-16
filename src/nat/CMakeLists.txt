# 网络模块：Socket 和 Address
add_library(Nat STATIC
    # STUNClient.cpp
    Stun.cpp
)

target_include_directories(Nat PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
