// #include "network/Socket.hpp"
// #include "network/IPAddress.hpp"
// #include "network/Port.hpp"
// #include <iostream>
#include "nat/Stun.h"
#include <cstring>

int main()
{
    p2p::nat::Stun stun;
    stun.get_nat_mapping(p2p::CreateAddress("111.206.174.2", 3478));
    // try
    // {
    //     // 创建 UDP Socket
    //     auto socket = p2p::CreateUDPSocket(true); // 创建 IPv4 UDP Socket

    //     // 创建 IP 地址和端口
    //     auto ipAddress = p2p::CreateIPAddress("::1"); // 监听所有网络接口
    //     p2p::Port port(1234); // 监听端口 1234

    //     // 绑定到指定地址
    //     if (!socket->bind(*ipAddress, port))
    //     {
    //         std::cerr << "Failed to bind to address!" << std::endl;
    //         return 1;
    //     }

    //     std::cout << "Listening on UDP port 1234..." << std::endl;

    //     // 接收数据
    //     char buffer[1024];
    //     while (true)
    //     {
    //         std::unique_ptr<p2p::IIPAddress> srcIpAddress;
    //         std::unique_ptr<p2p::Port> srcPort;

    //         // 接收数据
    //         ssize_t len = socket->recvFrom(buffer, sizeof(buffer), srcIpAddress, srcPort);

    //         if (len > 0)
    //         {
    //             // 打印接收到的数据和来源地址
    //             std::cout << "Received " << len << " bytes from "
    //                       << srcIpAddress->getIPString() << ":" << srcPort->getPort() << ": "
    //                       << std::string(buffer, len) << std::endl;
    //         }
    //         else
    //         {
    //             std::cerr << "Failed to receive data!" << std::endl;
    //             break;
    //         }
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     return 1;
    // }
    // try
    // {
    //     // 创建 TCP Socket
    //     auto socket = p2p::CreateTCPSocket(false); // 创建 IPv4 TCP Socket

    //     // 创建服务器 IP 地址和端口
    //     auto serverIpAddress = p2p::CreateIPAddress("127.0.0.1"); // 服务器地址
    //     p2p::Port serverPort(1234); // 服务器端口

    //     // 连接到服务器
    //     if (!socket->connect(*serverIpAddress, serverPort))
    //     {
    //         std::cerr << "Failed to connect to server!" << std::endl;
    //         return 1;
    //     }

    //     std::cout << "Connected to server at " << serverIpAddress->getIPString() 
    //               << ":" << serverPort.getPort() << std::endl;

    //     // 发送数据到服务器
    //     const char* message = "Hello, Server!";
    //     ssize_t sentLen = socket->send(message, strlen(message));

    //     if (sentLen < 0)
    //     {
    //         std::cerr << "Failed to send data to server!" << std::endl;
    //         return 1;
    //     }

    //     std::cout << "Sent " << sentLen << " bytes to server: " << message << std::endl;

    //     // 接收服务器的响应
    //     char buffer[1024];
    //     ssize_t recvLen = socket->recv(buffer, sizeof(buffer));

    //     if (recvLen < 0)
    //     {
    //         std::cerr << "Failed to receive data from server!" << std::endl;
    //         return 1;
    //     }

    //     buffer[recvLen] = '\0'; // 确保字符串以 null 结尾
    //     std::cout << "Received " << recvLen << " bytes from server: " << buffer << std::endl;
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     return 1;
    // }
    // try
    // {
    //     // 创建 TCP Socket
    //     auto socket = p2p::CreateTCPSocket(false); // 创建 IPv4 TCP Socket

    //     // 创建服务器 IP 地址和端口
    //     auto serverIpAddress = p2p::CreateIPAddress("0.0.0.0"); // 监听所有网络接口
    //     p2p::Port serverPort(1234); // 监听端口 1234

    //     // 绑定到指定地址
    //     if (!socket->bind(*serverIpAddress, serverPort))
    //     {
    //         std::cerr << "Failed to bind to address!" << std::endl;
    //         return 1;
    //     }

    //     // 开始监听连接
    //     if (!socket->listen(5)) // 设置 backlog 为 5
    //     {
    //         std::cerr << "Failed to listen on socket!" << std::endl;
    //         return 1;
    //     }

    //     std::cout << "Listening on TCP port 1234..." << std::endl;

    //     // 接受客户端连接
    //     while (true)
    //     {
    //         std::unique_ptr<p2p::IIPAddress> clientIpAddress;
    //         std::unique_ptr<p2p::Port> clientPort;

    //         // 接受连接
    //         auto clientSocket = socket->accept(clientIpAddress, clientPort);

    //         if (clientSocket)
    //         {
    //             std::cout << "Accepted connection from "
    //                       << clientIpAddress->getIPString() << ":" << clientPort->getPort() << std::endl;

    //             // 接收客户端发送的数据
    //             char buffer[1024];
    //             ssize_t recvLen = clientSocket->recv(buffer, sizeof(buffer));

    //             if (recvLen > 0)
    //             {
    //                 buffer[recvLen] = '\0'; // 确保字符串以 null 结尾
    //                 std::cout << "Received " << recvLen << " bytes from client: " << buffer << std::endl;

    //                 // 发送响应给客户端
    //                 const char* response = "Hello, Client!";
    //                 ssize_t sentLen = clientSocket->send(response, strlen(response));

    //                 if (sentLen < 0)
    //                 {
    //                     std::cerr << "Failed to send data to client!" << std::endl;
    //                 }
    //                 else
    //                 {
    //                     std::cout << "Sent " << sentLen << " bytes to client: " << response << std::endl;
    //                 }
    //             }
    //             else
    //             {
    //                 std::cerr << "Failed to receive data from client!" << std::endl;
    //             }
    //         }
    //         else
    //         {
    //             std::cerr << "Failed to accept client connection!" << std::endl;
    //         }
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     return 1;
    // }
    return 0;
}