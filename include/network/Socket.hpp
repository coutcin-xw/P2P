#ifndef INCLUDE_NETWORK_SOCKET_HPP_
#define INCLUDE_NETWORK_SOCKET_HPP_

#include "network/IPAddress.hpp"
#include "network/Port.hpp"
#include <memory>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <BaseTsd.h>  // For SSIZE_T
using ssize_t = SSIZE_T;
#else
#include <sys/socket.h> // socket API
#include <arpa/inet.h>
#include <unistd.h>     // close()
#endif
namespace p2p
{

    /// Socket 类型枚举
    enum class SocketType
    {
        UDP,
        TCP
    };
    class ISocket
    {
    public:
        /// 绑定到指定地址
        virtual bool bind(const IIPAddress &ipAddress, const Port &port) = 0;
    };
    /**
     * @brief UDP Socket 抽象接口
     */
    class IUDPSocket : virtual public ISocket
    {
    public:
        virtual ~IUDPSocket() = default;

        /// 发送数据到指定地址
        virtual ssize_t sendTo(const void *buf, size_t len, const IIPAddress &ipAddress, const Port &port) = 0;

        /// 接收数据并获取来源地址
        virtual ssize_t recvFrom(void *buf, size_t len, std::unique_ptr<IIPAddress> &srcIpAddress, std::unique_ptr<Port> &srcPort) = 0;

        /// 获取底层 socket 描述符
        virtual int getNativeHandle() const noexcept = 0;
 
        /// 获取本地绑定的 IP 地址和端口
        virtual std::unique_ptr<IIPAddress> getLocalIPAddress() const noexcept = 0;
        virtual std::unique_ptr<Port> getLocalPort() const noexcept = 0;
    };

    /**
     * @brief TCP Socket 抽象接口
     */
    class ITCPSocket : virtual public ISocket
    {
    public:
        virtual ~ITCPSocket() = default;

        /// 监听连接
        virtual bool listen(int backlog = 5) = 0;

        /// 接受连接
        virtual std::unique_ptr<ITCPSocket> accept(std::unique_ptr<IIPAddress> &clientIpAddress, std::unique_ptr<Port> &clientPort) = 0;

        /// 连接到指定地址
        virtual bool connect(const IIPAddress &ipAddress, const Port &port) = 0;

        /// 发送数据
        virtual ssize_t send(const void *buf, size_t len) = 0;

        /// 接收数据
        virtual ssize_t recv(void *buf, size_t len) = 0;

        /// 获取底层 socket 描述符
        virtual int getNativeHandle() const noexcept = 0;

        /// 获取本地绑定的 IP 地址和端口
        virtual std::unique_ptr<IIPAddress> getLocalIPAddress() const noexcept = 0;
        virtual std::unique_ptr<Port> getLocalPort() const noexcept = 0;
    };

    /**
     * @brief 创建 Socket 的工厂函数
     * @param type 协议类型（UDP 或 TCP）
     * @param isIPv6 是否创建 IPv6 Socket（双栈支持）
     * @return 创建的 Socket 对象
     * @throws std::runtime_error 不支持的协议类型
     */
    std::unique_ptr<IUDPSocket> CreateUDPSocket(bool isIPv6);
    std::unique_ptr<ITCPSocket> CreateTCPSocket(bool isIPv6);
    std::unique_ptr<ISocket> CreateSocket(SocketType type, bool isIPv6);
} // namespace p2p

#endif // INCLUDE_NETWORK_SOCKET_HPP_