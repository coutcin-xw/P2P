
#include "nat/Stun.h"
#include "network/Address.hpp"
#include "network/Port.hpp"
#include <cstddef>

void p2p::nat::Stun::generate_transaction_id(uint8_t *tid) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);
  for (int i = 0; i < 12; ++i) {
    tid[i] = dis(gen);
  }
}

std::unique_ptr<p2p::IAddress>
p2p::nat::Stun::get_nat_mapping(std::unique_ptr<p2p::IAddress> stun_server) {
  uint8_t buffer[128];
  StunHeader *header = reinterpret_cast<StunHeader *>(buffer);
  header->type = htons(p2p::nat::StunMessageMethod::BINDING); // Binding Request
  header->length = 0;
  header->magic_cookie = htonl(0x2112A442);
  generate_transaction_id(header->transaction_id);
  // 发送请求
  if (this->sockfd_->sendTo(buffer, sizeof(StunHeader),
                            stun_server->getIPAddress(),
                            stun_server->getPort()) < 0) {
    return nullptr;
  }
  // 接收响应
  std::unique_ptr<IIPAddress> srcIpAddress;
  std::unique_ptr<Port> srcPort;
  ssize_t recv_len =
      this->sockfd_->recvFrom(buffer, sizeof(buffer), srcIpAddress, srcPort);
  // 解析响应
  if (recv_len > 0) {
    return parse_response(buffer, recv_len);
  } else {
    return nullptr;
  }
}

std::unique_ptr<p2p::IAddress> p2p::nat::Stun::parse_response(uint8_t *buffer, size_t len) {
  p2p::nat::StunHeader *header =
      reinterpret_cast<p2p::nat::StunHeader *>(buffer);
  if (ntohs(header->type) != p2p::nat::StunBindingType::BINDING_SUCCESS_RESPONSE)
    return nullptr; // 检查是否为Binding Response
  size_t pos = sizeof(p2p::nat::StunHeader);
  while (pos < len) {
    p2p::nat::StunAttr *attr =
        reinterpret_cast<p2p::nat::StunAttr *>(buffer + pos);
    uint16_t attr_type = ntohs(attr->type);
    uint16_t attr_len = ntohs(attr->length);
    if (attr_type ==
        p2p::nat::StunAttributesType::XOR_MAPPED_ADDRESS) { // XOR-MAPPED-ADDRESS
      uint8_t *value = buffer + pos + 4;
      // 验证属性长度（IPv4应为8字节）
      if (attr_len < 8)
        return nullptr;
      // 解析地址族和端口
      uint8_t address_family = value[1];
      if (address_family != 0x01)
        continue; // 0x01=IPv4
      // 获取网络字节序的加密值
      uint16_t xport = *reinterpret_cast<uint16_t *>(value + 2);
      uint32_t xip = *reinterpret_cast<uint32_t *>(value + 4);
      // 正确解码流程（先异或后转换字节序）
      uint16_t port = (xport ^ htons(0x2112)); // 异或后转主机序
      port = ntohs(port);
      uint32_t ip = (xip ^ htonl(0x2112A442)); // 异或后转主机序
      ip = ntohl(ip);
      // 输出结果
      struct in_addr addr;
      addr.s_addr = htonl(ip);
      std::cout << "Public IP: " << inet_ntoa(addr) << "\nPort: " << port
                << std::endl;

      return p2p::CreateAddress(std::string(inet_ntoa(addr)), port);
    }
    pos += 4 + attr_len + (attr_len % 4 ? 4 - attr_len % 4 : 0);
  }
  return nullptr;
}
