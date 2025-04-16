cmake_minimum_required(VERSION 3.10)
project(P2P)
message(STATUS "Start----------")

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义项目根目录
set(HOME ${CMAKE_CURRENT_SOURCE_DIR})

# 设置输出目录（可修改为target目录下的子目录）
set(EXECUTABLE_OUTPUT_PATH ${HOME}/target/bin)
set(LIBRARY_OUTPUT_PATH ${HOME}/target/lib)


# 静态编译示例（需要时取消注释）
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# 包含目录设置
include_directories(
    ${HOME}/include
    # 添加其他包含目录
)

# 库目录设置
link_directories(
    ${HOME}/build/lib
    ${HOME}/deps
    # 添加其他库目录
)
if(WIN32)
    list(APPEND EXTRA_LIBS Ws2_32)
    include_directories("$ENV{WindowsSdkDir}Include/$ENV{WindowsSDKVersion}shared")
    include_directories("$ENV{WindowsSdkDir}Include/$ENV{WindowsSDKVersion}um")
endif()

# 包含子目录
add_subdirectory(${HOME}/src/network)
add_subdirectory(${HOME}/src/nat)
list(APPEND EXTRA_LIBS Network)
list(APPEND EXTRA_LIBS Nat)
# add_subdirectory(${HOME}/src/nat)
# list(APPEND EXTRA_LIBS Nat)
# add_subdirectory(${HOME}/src/p2p)
# add_subdirectory(${HOME}/src/signaling)
# 第三方库示例（以OpenSSL为例）
# find_package(OpenSSL REQUIRED)
# if(OpenSSL_FOUND)
#     include_directories(${OPENSSL_INCLUDE_DIR})
#     list(APPEND EXTRA_LIBS ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
#     message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
# else()
#     message(FATAL_ERROR "OpenSSL not found!")
# endif()

# 收集源文件（注意：GLOB不是CMake推荐做法，适合示例使用）
file(GLOB MAIN_SRC 
    ${HOME}/src/*.cpp
)

message(STATUS "Source files: ${MAIN_SRC}")

# 创建可执行文件
add_executable(${PROJECT_NAME} ${MAIN_SRC})

# 链接库配置
target_link_libraries(${PROJECT_NAME}
    ${EXTRA_LIBS}
    # 添加其他依赖库
)

# 安装配置示例（需要时取消注释）
# install(TARGETS ${PROJECT_NAME}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

# 单元测试支持示例（需要时取消注释）
# enable_testing()
# add_subdirectory(unittest)

# 如果存在单元测试目录，可以在这里添加测试
# if(EXISTS ${CMAKE_SOURCE_DIR}/unittest)
#     enable_testing()
#     add_subdirectory(test)
# endif()
message(STATUS "Configuration completed----------")
