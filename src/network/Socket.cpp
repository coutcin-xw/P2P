#include "network/Socket.hpp"
#include "network/IPAddress.hpp"
#include "network/Port.hpp"
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif
namespace p2p
{

    /* UDP Socket 实现 */
    class UDPSocket : public IUDPSocket
    {
    public:
        explicit UDPSocket(bool isIPv6)
        {
            int family = isIPv6 ? AF_INET6 : AF_INET;
            sockfd_ = socket(family, SOCK_DGRAM, IPPROTO_UDP);

            if (sockfd_ == -1)
            {
                throw std::system_error(errno, std::generic_category(), "socket() failed");
            }

            if (isIPv6)
            {
                int opt = 0;
                if (setsockopt(sockfd_, IPPROTO_IPV6, IPV6_V6ONLY,
                               (char *)&opt, sizeof(opt)) != 0)
                {
                    // 非致命错误，记录日志但继续运行
                }
            }
        }

        ~UDPSocket() override
        {
            if (sockfd_ != -1)
            {
                close(sockfd_);
            }
        }

        bool bind(const IIPAddress &ipAddress, const Port &port) override
        {
            sockaddr_storage addr;
            memset(&addr, 0, sizeof(addr));

            if (ipAddress.isIPv6())
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                addr6.sin6_family = AF_INET6;
                addr6.sin6_port = port.getNetworkPort();
                memcpy(&addr6.sin6_addr, ipAddress.getIP().data(), 16);
            }
            else
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                addr4.sin_family = AF_INET;
                addr4.sin_port = port.getNetworkPort();
                memcpy(&addr4.sin_addr, ipAddress.getIP().data(), 4);
            }

            return ::bind(sockfd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
        }

        ssize_t sendTo(const void *buf, size_t len, const IIPAddress &ipAddress, const Port &port) override
        {
            sockaddr_storage addr;
            memset(&addr, 0, sizeof(addr));

            if (ipAddress.isIPv6())
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                addr6.sin6_family = AF_INET6;
                addr6.sin6_port = port.getNetworkPort();
                memcpy(&addr6.sin6_addr, ipAddress.getIP().data(), 16);
            }
            else
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                addr4.sin_family = AF_INET;
                addr4.sin_port = port.getNetworkPort();
                memcpy(&addr4.sin_addr, ipAddress.getIP().data(), 4);
            }

            return ::sendto(sockfd_, reinterpret_cast<const char *>(buf), len, 0,
                            reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
        }

        ssize_t recvFrom(void *buf, size_t len, std::unique_ptr<IIPAddress> &srcIpAddress, std::unique_ptr<Port> &srcPort) override
        {
            sockaddr_storage addr;
            socklen_t addrlen = sizeof(addr);

            ssize_t ret = ::recvfrom(sockfd_, reinterpret_cast<char *>(buf), len, 0,
                                     reinterpret_cast<sockaddr *>(&addr), &addrlen);

            if (ret > 0)
            {
                if (addr.ss_family == AF_INET)
                {
                    auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                    srcIpAddress = std::make_unique<IPv4Address>(addr4.sin_addr);
                    srcPort = std::make_unique<Port>(ntohs(addr4.sin_port));
                }
                else
                {
                    auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                    srcIpAddress = std::make_unique<IPv6Address>(addr6.sin6_addr);
                    srcPort = std::make_unique<Port>(ntohs(addr6.sin6_port));
                }
            }

            return ret;
        }

        int getNativeHandle() const noexcept override
        {
            return sockfd_;
        }

        std::unique_ptr<IIPAddress> getLocalIPAddress() const noexcept override
        {
            sockaddr_storage addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen) != 0)
            {
                return nullptr;
            }

            if (addr.ss_family == AF_INET)
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                return std::make_unique<IPv4Address>(addr4.sin_addr);
            }
            else if (addr.ss_family == AF_INET6)
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                return std::make_unique<IPv6Address>(addr6.sin6_addr);
            }

            return nullptr;
        }

        std::unique_ptr<Port> getLocalPort() const noexcept override
        {
            sockaddr_storage addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen) != 0)
            {
                return nullptr;
            }

            if (addr.ss_family == AF_INET)
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                return std::make_unique<Port>(ntohs(addr4.sin_port));
            }
            else if (addr.ss_family == AF_INET6)
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                return std::make_unique<Port>(ntohs(addr6.sin6_port));
            }

            return nullptr;
        }
        static std::unique_ptr<UDPSocket> createFromSocket(int sockfd)
        {
            return std::unique_ptr<UDPSocket>(new UDPSocket(sockfd));
        }

    private:
        explicit UDPSocket(int sockfd) : sockfd_(sockfd) {}
        int sockfd_ = -1; // socket 文件描述符
    };

    /* TCP Socket 实现 */
    class TCPSocket : public ITCPSocket
    {
    public:
// Modify socket creation to handle Windows
explicit TCPSocket(bool isIPv6)
{
    InitWinsock(); // Ensure Winsock is initialized (Windows only)
    int family = isIPv6 ? AF_INET6 : AF_INET;
    sockfd_ = socket(family, SOCK_STREAM, IPPROTO_TCP);

#ifdef _WIN32
    if (sockfd_ == INVALID_SOCKET)
    {
        throw std::system_error(WSAGetLastError(), std::system_category(), "socket() failed");
    }
#else 
    if (sockfd_ == -1)
    {
        throw std::system_error(errno, std::generic_category(), "socket() failed");
    }
#endif

    if (isIPv6)
    {
        int opt = 0;
        if (setsockopt(sockfd_, IPPROTO_IPV6, IPV6_V6ONLY,
                       reinterpret_cast<char *>(&opt), sizeof(opt)) != 0)
        {
            // Log non-fatal error
        }
    }
}

        ~TCPSocket() override
        {
            if (sockfd_ != -1)
            {
#ifdef _WIN32
                closesocket(sockfd_);
#else
                close(sockfd_);
#endif
            }
        }
        // Initialize Winsock on Windows (call this once at program startup)
        static void InitWinsock()
        {
#ifdef _WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                throw std::runtime_error("WSAStartup failed");
            }
#endif
        }

        
        bool bind(const IIPAddress &ipAddress, const Port &port) override
        {
            sockaddr_storage addr;
            memset(&addr, 0, sizeof(addr));

            if (ipAddress.isIPv6())
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                addr6.sin6_family = AF_INET6;
                addr6.sin6_port = port.getNetworkPort();
                memcpy(&addr6.sin6_addr, ipAddress.getIP().data(), 16);
            }
            else
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                addr4.sin_family = AF_INET;
                addr4.sin_port = port.getNetworkPort();
                memcpy(&addr4.sin_addr, ipAddress.getIP().data(), 4);
            }

            return ::bind(sockfd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
        }

        bool listen(int backlog) override
        {
            return ::listen(sockfd_, backlog) == 0;
        }

        std::unique_ptr<ITCPSocket> accept(std::unique_ptr<IIPAddress> &clientIpAddress, std::unique_ptr<Port> &clientPort) override
        {
            sockaddr_storage addr;
            socklen_t addrlen = sizeof(addr);

            int clientSockfd = ::accept(sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen);

            if (clientSockfd == -1)
            {
                throw std::system_error(errno, std::generic_category(), "accept() failed");
            }

            if (addr.ss_family == AF_INET)
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                clientIpAddress = std::make_unique<IPv4Address>(addr4.sin_addr);
                clientPort = std::make_unique<Port>(ntohs(addr4.sin_port));
            }
            else
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                clientIpAddress = std::make_unique<IPv6Address>(addr6.sin6_addr);
                clientPort = std::make_unique<Port>(ntohs(addr6.sin6_port));
            }

            return TCPSocket::createFromSocket(clientSockfd);
        }

        bool connect(const IIPAddress &ipAddress, const Port &port) override
        {
            sockaddr_storage addr;
            memset(&addr, 0, sizeof(addr));

            if (ipAddress.isIPv6())
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                addr6.sin6_family = AF_INET6;
                addr6.sin6_port = port.getNetworkPort();
                memcpy(&addr6.sin6_addr, ipAddress.getIP().data(), 16);
            }
            else
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                addr4.sin_family = AF_INET;
                addr4.sin_port = port.getNetworkPort();
                memcpy(&addr4.sin_addr, ipAddress.getIP().data(), 4);
            }

            return ::connect(sockfd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == 0;
        }

        ssize_t send(const void *buf, size_t len) override
        {
            return ::send(sockfd_, reinterpret_cast<const char *>(buf), len, 0);
        }

        ssize_t recv(void *buf, size_t len) override
        {
            return ::recv(sockfd_, reinterpret_cast<char *>(buf), len, 0);
        }

        int getNativeHandle() const noexcept override
        {
            return sockfd_;
        }

        std::unique_ptr<IIPAddress> getLocalIPAddress() const noexcept override
        {
            sockaddr_storage addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen) != 0)
            {
                return nullptr;
            }

            if (addr.ss_family == AF_INET)
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                return std::make_unique<IPv4Address>(addr4.sin_addr);
            }
            else if (addr.ss_family == AF_INET6)
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                return std::make_unique<IPv6Address>(addr6.sin6_addr);
            }

            return nullptr;
        }
        static std::unique_ptr<TCPSocket> createFromSocket(int sockfd)
        {
            return std::unique_ptr<TCPSocket>(new TCPSocket(sockfd));
        }

        std::unique_ptr<Port> getLocalPort() const noexcept override
        {
            sockaddr_storage addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(sockfd_, reinterpret_cast<sockaddr *>(&addr), &addrlen) != 0)
            {
                return nullptr;
            }

            if (addr.ss_family == AF_INET)
            {
                auto &addr4 = reinterpret_cast<sockaddr_in &>(addr);
                return std::make_unique<Port>(ntohs(addr4.sin_port));
            }
            else if (addr.ss_family == AF_INET6)
            {
                auto &addr6 = reinterpret_cast<sockaddr_in6 &>(addr);
                return std::make_unique<Port>(ntohs(addr6.sin6_port));
            }

            return nullptr;
        }

    private:
        explicit TCPSocket(int sockfd) : sockfd_(sockfd) {}
        int sockfd_ = -1; // socket 文件描述符
    };

    /* Socket 工厂函数实现 */
    std::unique_ptr<IUDPSocket> CreateUDPSocket(bool isIPv6)
    {
        return std::make_unique<UDPSocket>(isIPv6);
    }

    std::unique_ptr<ITCPSocket> CreateTCPSocket(bool isIPv6)
    {
        return std::make_unique<TCPSocket>(isIPv6);
    }

    std::unique_ptr<ISocket> CreateSocket(SocketType type, bool isIPv6)
    {
        if (type == SocketType::UDP)
        {
            return std::make_unique<UDPSocket>(isIPv6);
        }
        else if (type == SocketType::TCP)
        {
            return std::make_unique<TCPSocket>(isIPv6);
        }
        else
        {
            throw std::runtime_error("Unsupported socket type");
        }
    }

} // namespace p2p