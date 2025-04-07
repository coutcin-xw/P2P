#include "network/IPAddress.hpp"
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#endif

namespace p2p
{
    
    /* IPv4Address 实现 */

    IPv4Address::IPv4Address(const std::string &ip)
    {
#ifdef _WIN32
        addr_.s_addr = inet_addr(ip.c_str());
        if (addr_.s_addr == INADDR_NONE) {
            throw std::invalid_argument("Invalid IPv4 address: " + ip);
        }
#else
        int result = inet_pton(AF_INET, ip.c_str(), &addr_);
        if (result != 1)
        {
            throw std::invalid_argument("Invalid IPv4 address: " + ip);
        }
#endif
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
        const char* result = inet_ntoa(addr_);
        return result ? std::string(result) : std::string();
#else
        const char* result = inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
        return result ? std::string(result) : std::string();
#endif
    }

    /* IPv6Address 实现 */

    IPv6Address::IPv6Address(const std::string &ip)
    {
#ifdef _WIN32
        int addrLen = sizeof(addr_);
        int result = WSAStringToAddressA(
            const_cast<char*>(ip.c_str()),
            AF_INET6,
            NULL,
            (LPSOCKADDR)&addr_,
            &addrLen
        );
        if (result != 0) {
            throw std::invalid_argument("Invalid IPv6 address: " + ip);
        }
#else
        int result = inet_pton(AF_INET6, ip.c_str(), &addr_);
        if (result != 1)
        {
            throw std::invalid_argument("Invalid IPv6 address: " + ip);
        }
#endif
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
        DWORD length = sizeof(buf);
        int result = WSAAddressToStringA(
            (LPSOCKADDR)&addr_,
            sizeof(addr_),
            NULL,
            buf,
            &length
        );
        return (result == 0) ? std::string(buf) : std::string();
#else
        const char* result = inet_ntop(AF_INET6, &addr_, buf, sizeof(buf));
        return result ? std::string(result) : std::string();
#endif
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
