

#include <iostream>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>
#pragma pack(push, 1)
struct StunHeader {
    uint16_t type;
    uint16_t length;
    uint32_t magic_cookie;
    uint8_t transaction_id[12];
};
struct StunAttr {
    uint16_t type;
    uint16_t length;
};
#pragma pack(pop)
class StunMessage {
public:
    StunClient() : sockfd(-1) {}
    
    bool get_nat_mapping(const char* server, uint16_t port) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) return false;
        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        inet_pton(AF_INET, server, &serv_addr.sin_addr);
        // 构建STUN请求
        uint8_t buffer[128];
        StunHeader* header = reinterpret_cast<StunHeader*>(buffer);
        header->type = htons(0x0001); // Binding Request
        header->length = 0;
        header->magic_cookie = htonl(0x2112A442);
        generate_transaction_id(header->transaction_id);
        // 发送请求
        if(sendto(sockfd, buffer, sizeof(StunHeader), 0, 
                (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            close(sockfd);
            return false;
        }
        // 接收响应
        sockaddr_in from_addr{};
        socklen_t addr_len = sizeof(from_addr);
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                   (sockaddr*)&from_addr, &addr_len);
        
        // 解析响应
        if(recv_len > 0) {
            return parse_response(buffer, recv_len);
        }
        close(sockfd);
        return false;
    }
private:
    int sockfd;
    void generate_transaction_id(uint8_t* tid) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for(int i=0; i<12; ++i) {
            tid[i] = dis(gen);
        }
    }
    bool parse_response(uint8_t* buffer, size_t len) {
        StunHeader* header = reinterpret_cast<StunHeader*>(buffer);
        if(ntohs(header->type) != 0x0101) return false; // 检查是否为Binding Response
        size_t pos = sizeof(StunHeader);
        while(pos < len) {
            StunAttr* attr = reinterpret_cast<StunAttr*>(buffer + pos);
            uint16_t attr_type = ntohs(attr->type);
            uint16_t attr_len = ntohs(attr->length);
            if(attr_type == 0x0020) { // XOR-MAPPED-ADDRESS
                uint8_t* value = buffer + pos + 4;
                
                // 验证属性长度（IPv4应为8字节）
                if(attr_len < 8) return false; 
                // 解析地址族和端口
                uint8_t address_family = value[1]; 
                if(address_family != 0x01) continue; // 0x01=IPv4
                // 获取网络字节序的加密值
                uint16_t xport = *reinterpret_cast<uint16_t*>(value + 2);
                uint32_t xip = *reinterpret_cast<uint32_t*>(value + 4);
                // 正确解码流程（先异或后转换字节序）
                uint16_t port = (xport ^ htons(0x2112)); // 异或后转主机序
                port = ntohs(port);
                uint32_t ip = (xip ^ htonl(0x2112A442)); // 异或后转主机序
                ip = ntohl(ip);
                // 输出结果
                struct in_addr addr;
                addr.s_addr = htonl(ip);
                std::cout << "Public IP: " << inet_ntoa(addr)
                          << "\nPort: " << port << std::endl;
                return true;
            }
            pos += 4 + attr_len + (attr_len%4 ? 4-attr_len%4 : 0);
        }
        return false;
    }
};
// 使用示例l
int main() {
    StunClient client;
    if(client.get_nat_mapping("111.206.174.2", 3478)) {
        std::cout << "NAT mapping成功获取" << std::endl;
    } else {
        std::cerr << "查询失败" << std::endl;
    }
    return 0;
}


