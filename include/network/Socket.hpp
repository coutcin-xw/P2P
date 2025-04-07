#ifndef INCLUDE_NETWORK_SOCKET_HPP_
#define INCLUDE_NETWORK_SOCKET_HPP_

#include "network/IPAddress.hpp"
#include "network/Port.hpp"
#include <memory>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <BaseTsd.h>
using ssize_t = SSIZE_T;
#define SOCKET_HANDLE SOCKET
#define SOCKET_ERROR_VAL INVALID_SOCKET
#define CLOSE_SOCKET closesocket
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET_HANDLE int
#define SOCKET_ERROR_VAL (-1)
#define CLOSE_SOCKET close
#endif

namespace p2p
{

    enum class SocketType
    {
        UDP,
        TCP
    };

    class ISocket
    {
    public:
        virtual ~ISocket() = default;
        virtual bool bind(const IIPAddress &ipAddress, const Port &port) = 0;
    };
    class IUDPSocket : virtual public ISocket
    {
    public:
        virtual ~IUDPSocket() = default;
        virtual ssize_t sendTo(const void *buf, size_t len, const IIPAddress &ipAddress, const Port &port) = 0;
        virtual ssize_t recvFrom(void *buf, size_t len, std::unique_ptr<IIPAddress> &srcIpAddress, std::unique_ptr<Port> &srcPort) = 0;
        virtual SOCKET_HANDLE getNativeHandle() const noexcept = 0;
        virtual std::unique_ptr<IIPAddress> getLocalIPAddress() const noexcept = 0;
        virtual std::unique_ptr<Port> getLocalPort() const noexcept = 0;
    };

    class ITCPSocket : virtual public ISocket
    {
    public:
        virtual ~ITCPSocket() = default;
        virtual bool listen(int backlog = 5) = 0;
        virtual std::unique_ptr<ITCPSocket> accept(std::unique_ptr<IIPAddress> &clientIpAddress, std::unique_ptr<Port> &clientPort) = 0;
        virtual bool connect(const IIPAddress &ipAddress, const Port &port) = 0;
        virtual ssize_t send(const void *buf, size_t len) = 0;
        virtual ssize_t recv(void *buf, size_t len) = 0;
        virtual SOCKET_HANDLE getNativeHandle() const noexcept = 0;
        virtual std::unique_ptr<IIPAddress> getLocalIPAddress() const noexcept = 0;
        virtual std::unique_ptr<Port> getLocalPort() const noexcept = 0;
    };

    std::unique_ptr<IUDPSocket> CreateUDPSocket(bool isIPv6);
    std::unique_ptr<ITCPSocket> CreateTCPSocket(bool isIPv6);
    std::unique_ptr<ISocket> CreateSocket(SocketType type, bool isIPv6);

} // namespace p2p

#endif // INCLUDE_NETWORK_SOCKET_HPP_