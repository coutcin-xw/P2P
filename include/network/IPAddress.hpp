#ifndef INCLUDE_NETWORK_IPADDRESS_HPP_
#define INCLUDE_NETWORK_IPADDRESS_HPP_


#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace p2p
{
    /**
     * @brief IP地址抽象基类
     */
    class IIPAddress
    {
    public:
        virtual ~IIPAddress() = default;
        virtual std::vector<uint8_t> getIP() const noexcept = 0; // 获取二进制格式的IP地址
        virtual std::string getIPString() const noexcept = 0;    // 获取字符串格式的IP地址
        virtual bool isIPv6() const noexcept = 0;                // 判断是否为IPv6
    };

    /**
     * @brief IPv4地址具体实现类
     */
    class IPv4Address final : public IIPAddress
    {
    public:
        explicit IPv4Address(const std::string &ip);
        explicit IPv4Address(const in_addr &addr);
        std::vector<uint8_t> getIP() const noexcept override;
        std::string getIPString() const noexcept override;
        bool isIPv6() const noexcept override { return false; }

    private:
        in_addr addr_;
    };

    /**
     * @brief IPv6地址具体实现类
     */
    class IPv6Address final : public IIPAddress
    {
    public:
        explicit IPv6Address(const std::string &ip);
        explicit IPv6Address(const in6_addr &addr);
        std::vector<uint8_t> getIP() const noexcept override;
        std::string getIPString() const noexcept override;
        bool isIPv6() const noexcept override { return true; }

    private:
        in6_addr addr_;
    };

    /**
     * @brief IP地址工厂函数
     * @param ip 字符串格式的IP地址
     * @return 根据输入自动创建的IPv4/IPv6地址对象
     */
    std::unique_ptr<IIPAddress> CreateIPAddress(const std::string &ip);
} // namespace p2p

#endif  // INCLUDE_P2P_IPADDRESS_HPP_
