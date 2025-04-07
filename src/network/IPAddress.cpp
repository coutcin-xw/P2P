#include "network/IPAddress.hpp"
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h> // inet_pton, inet_ntop
#endif

namespace p2p
{
    /* IPv4Address 实现 */

    IPv4Address::IPv4Address(const std::string &ip)
    {
#ifdef _WIN32
        if (InetPtonA(AF_INET, ip.c_str(), &addr_) != 1)
#else
        if (inet_pton(AF_INET, ip.c_str(), &addr_) != 1)
#endif
        {
            throw std::invalid_argument("Invalid IPv4 address: " + ip);
        }
    }

    IPv4Address::IPv4Address(const in_addr &addr) : addr_(addr) {}

    std::vector<uint8_t> IPv4Address::getIP() const noexcept
    {
        const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&addr_.s_addr);
        return {bytes, bytes + 4};
    }

    std::string IPv4Address::getIPString() const noexcept
    {
        char buf[INET_ADDRSTRLEN];
#ifdef _WIN32
        InetNtopA(AF_INET, const_cast<in_addr*>(&addr_), buf, sizeof(buf));
#else
        inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
#endif
        return std::string(buf);
    }

    /* IPv6Address 实现 */

    IPv6Address::IPv6Address(const std::string &ip)
    {
#ifdef _WIN32
        if (InetPtonA(AF_INET6, ip.c_str(), &addr_) != 1)
#else
        if (inet_pton(AF_INET6, ip.c_str(), &addr_) != 1)
#endif
        {
            throw std::invalid_argument("Invalid IPv6 address: " + ip);
        }
    }

    IPv6Address::IPv6Address(const in6_addr &addr) : addr_(addr) {}

    std::vector<uint8_t> IPv6Address::getIP() const noexcept
    {
        const uint8_t *bytes = addr_.s6_addr;
        return {bytes, bytes + 16};
    }

    std::string IPv6Address::getIPString() const noexcept
    {
        char buf[INET6_ADDRSTRLEN];
#ifdef _WIN32
        InetNtopA(AF_INET6, const_cast<in6_addr*>(&addr_), buf, sizeof(buf));
#else
        inet_ntop(AF_INET6, &addr_, buf, sizeof(buf));
#endif
        return std::string(buf);
    }

    /* IP 地址工厂函数 */

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