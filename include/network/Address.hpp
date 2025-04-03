#ifndef INCLUDE_NETWORK_ADDRESS_HPP_
#define INCLUDE_NETWORK_ADDRESS_HPP_

#include "network/IPAddress.hpp"
#include "Port.hpp"
#include <memory>
#include <sys/socket.h>

namespace p2p
{
    /**
     * @brief 网络地址抽象基类
     */
    class IAddress
    {
    public:
        virtual ~IAddress() = default;
        virtual const IIPAddress &getIPAddress() const noexcept = 0; // 获取IP地址
        virtual const Port &getPort() const noexcept = 0;            // 获取端口
        virtual std::string toString() const = 0;                   // 转换为可读字符串
    };

    /**
     * @brief 地址具体实现类
     */
    class Address final : public IAddress
    {
    public:
        Address(std::unique_ptr<IIPAddress> ipAddress, std::unique_ptr<Port> port);
        const IIPAddress &getIPAddress() const noexcept override;
        const Port &getPort() const noexcept override;
        std::string toString() const override;

    private:
        std::unique_ptr<IIPAddress> ipAddress_;
        std::unique_ptr<Port> port_;
    };

    /**
     * @brief 地址工厂函数
     * @param ip 字符串格式的IP地址
     * @param port 端口号（主机字节序）
     * @return 根据输入自动创建的地址对象
     */
    std::unique_ptr<IAddress> CreateAddress(const std::string &ip, uint16_t port);
} // namespace p2p


#endif // INCLUDE_ADDRESS_HPP_
