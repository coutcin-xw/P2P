#include "network/IPAddress.hpp"
#include <stdexcept>
#include <arpa/inet.h> // inet_pton, inet_ntop

namespace p2p
{
    /* IPv4Address 实现 */

    /**
     * @brief 从字符串构造 IPv4 地址
     * @param ip 点分十进制格式的 IPv4 地址
     * @throws std::invalid_argument 如果 IP 地址无效
     */
    IPv4Address::IPv4Address(const std::string &ip)
    {
        if (inet_pton(AF_INET, ip.c_str(), &addr_) != 1)
        {
            throw std::invalid_argument("Invalid IPv4 address: " + ip);
        }
    }

    /**
     * @brief 从系统结构体构造 IPv4 地址
     * @param addr in_addr 结构体
     */
    IPv4Address::IPv4Address(const in_addr &addr) : addr_(addr) {}

    /**
     * @brief 获取二进制格式的 IPv4 地址
     * @return 4 字节的二进制 IP 地址（网络字节序）
     */
    std::vector<uint8_t> IPv4Address::getIP() const noexcept
    {
        const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&addr_.s_addr);
        return {bytes, bytes + 4};
    }

    /**
     * @brief 获取字符串格式的 IPv4 地址
     * @return 点分十进制格式的 IPv4 地址
     */
    std::string IPv4Address::getIPString() const noexcept
    {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
        return std::string(buf);
    }

    /* IPv6Address 实现 */

    /**
     * @brief 从字符串构造 IPv6 地址
     * @param ip 标准格式的 IPv6 地址
     * @throws std::invalid_argument 如果 IP 地址无效
     */
    IPv6Address::IPv6Address(const std::string &ip)
    {
        if (inet_pton(AF_INET6, ip.c_str(), &addr_) != 1)
        {
            throw std::invalid_argument("Invalid IPv6 address: " + ip);
        }
    }

    /**
     * @brief 从系统结构体构造 IPv6 地址
     * @param addr in6_addr 结构体
     */
    IPv6Address::IPv6Address(const in6_addr &addr) : addr_(addr) {}

    /**
     * @brief 获取二进制格式的 IPv6 地址
     * @return 16 字节的二进制 IP 地址（网络字节序）
     */
    std::vector<uint8_t> IPv6Address::getIP() const noexcept
    {
        const uint8_t *bytes = addr_.s6_addr;
        return {bytes, bytes + 16};
    }

    /**
     * @brief 获取字符串格式的 IPv6 地址
     * @return 标准格式的 IPv6 地址
     */
    std::string IPv6Address::getIPString() const noexcept
    {
        char buf[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr_, buf, sizeof(buf));
        return std::string(buf);
    }

    /* IP 地址工厂函数 */

    /**
     * @brief 创建 IP 地址对象
     * @param ip 字符串格式的 IP 地址
     * @return 根据输入自动创建的 IPv4/IPv6 地址对象
     * @throws std::invalid_argument 如果 IP 地址无效
     */
    std::unique_ptr<IIPAddress> CreateIPAddress(const std::string &ip)
    {
        if (ip.find(':') != std::string::npos)
        {
            return std::make_unique<IPv6Address>(ip);
        }
        else
        {
            return std::make_unique<IPv4Address>(ip);
        }
    }
} // namespace p2p