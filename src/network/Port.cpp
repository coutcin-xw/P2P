#include "network/Port.hpp"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h> // inet_pton, inet_ntop
#endif

namespace p2p
{
    /**
     * @brief 构造函数
     * @param port 端口号
     * @param isNetworkOrder 是否为网络字节序
     */
    Port::Port(uint16_t port, bool isNetworkOrder)
    {
        if (isNetworkOrder)
        {
            port_ = ntohs(port); // 网络字节序转为主机字节序
        }
        else
        {
            port_ = port; // 直接使用主机字节序
        }
    }

    /**
     * @brief 获取主机字节序的端口
     * @return 主机字节序的端口号
     */
    uint16_t Port::getPort() const noexcept
    {
        return port_;
    }

    /**
     * @brief 获取网络字节序的端口
     * @return 网络字节序的端口号
     */
    uint16_t Port::getNetworkPort() const noexcept
    {
        return htons(port_);
    }
} // namespace p2p