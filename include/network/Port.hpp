#ifndef INCLUDE_NETWORK_PORT_HPP_
#define INCLUDE_NETWORK_PORT_HPP_


#include <cstdint>

namespace p2p
{
    /**
     * @brief 端口类
     */
    class Port
    {
    public:
        explicit Port(uint16_t port, bool isNetworkOrder = false);
        uint16_t getPort() const noexcept;         // 获取主机字节序的端口
        uint16_t getNetworkPort() const noexcept;  // 获取网络字节序的端口

    private:
        uint16_t port_;
    };
} // namespace p2p


#endif  // INCLUDE_P2P_PORT_HPP_
