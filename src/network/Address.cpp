#include "network/Address.hpp"
#include "network/IPAddress.hpp"
#include "network/Port.hpp"
#include <stdexcept>
#include <memory>

namespace p2p
{
    /* Address 实现 */
    Address::Address(std::unique_ptr<IIPAddress> ipAddress, std::unique_ptr<Port> port)
        : ipAddress_(std::move(ipAddress)), port_(std::move(port)) {}

    const IIPAddress &Address::getIPAddress() const noexcept
    {
        return *ipAddress_;
    }

    const Port &Address::getPort() const noexcept
    {
        return *port_;
    }

    std::string Address::toString() const
    {
        return ipAddress_->getIPString() + ":" + std::to_string(port_->getPort());
    }

    /* 工厂函数实现 */
    std::unique_ptr<IAddress> CreateAddress(const std::string &ip, uint16_t port)
    {
        auto ipAddress = CreateIPAddress(ip);
        auto portObj = std::make_unique<Port>(port);
        return std::make_unique<Address>(std::move(ipAddress), std::move(portObj));
    }
} // namespace p2p