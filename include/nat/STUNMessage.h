#ifndef INCLUDE_NAT_STUNMESSAGE_H_
#define INCLUDE_NAT_STUNMESSAGE_H_
/*
*STUN client and server based on RFC5389 standard
*/
#include <iostream>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>
#include <unordered_map>
#include "network/Address.hpp"
/*

STUN消息结构
+---------------+---------------+---------------+---------------+
|0              |1              |2              |3              |
+---------------+---------------+---------------+---------------+
|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Message Type           |         Message Length        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          Magic Cookie                         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
|                      Transaction ID (96 bits)                 |
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Attributes                        |
|                             ...                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Message Type
+-----------------------+-----------------------+
|           1           |           2           |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|0 |1 |2 |3 |4 |5 |6 |7 |0 |1 |2 |3 |4 |5 |6 |7 |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|- |- |M |M |M |M |M |C |M |M |M |C |M |M |M |M |
|  |  |11|10|9 |8 |7 |1 |6 |5 |4 |0 |3 |2 |1 |0 |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+


每个属性采用TLV（Type-Length-Value）结构：
+---------------+---------------+---------------+---------------+
|0              |1              |2              |3              |
+---------------+---------------+---------------+---------------+
|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|             Type              |            Length             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             Value                             |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
namespace p2p {
    namespace nat {
        struct StunHeader {
            uint16_t type; 
            uint16_t length;
            uint32_t magic_cookie;
            uint8_t transaction_id[12];
        };
        const uint32_t STUNMAGIC = 0x2112A442;
        struct StunAttr {
            uint16_t type;
            uint16_t length;
        };
        enum StunMessageClass {
            REQUEST         = 0b00, //请求
            INDICATION      = 0b01,
            SUCCESS_RESPONSE= 0b10,
            ERROR_RESPONSE  = 0b11
        };
        enum StunMessageMethod {
            BINDING         = 0x001,
        };
        enum StunBindingType {
            BINDING_REQUEST          = (StunMessageMethod::BINDING & 0x0FFF) | (((StunMessageClass::REQUEST & 0x002) << 7)|((StunMessageClass::REQUEST & 0x001) << 4)),          // 0x0001
            BINDING_INDICATION       = (StunMessageMethod::BINDING & 0x0FFF) | (((StunMessageClass::INDICATION & 0x002) << 7)|((StunMessageClass::INDICATION & 0x001) << 4)),      // 0x0011
            BINDING_SUCCESS_RESPONSE = (StunMessageMethod::BINDING & 0x0FFF) | (((StunMessageClass::SUCCESS_RESPONSE & 0x002) << 7)|((StunMessageClass::SUCCESS_RESPONSE & 0x001) << 4)),// 0x0101
            BINDING_ERROR_RESPONSE   = (StunMessageMethod::BINDING & 0x0FFF) | (((StunMessageClass::ERROR_RESPONSE & 0x002) << 7)|((StunMessageClass::ERROR_RESPONSE & 0x001) << 4)),    // 0x0111
        };
        
        enum StunAttributesType {
            MAPPED_ADDRESS      = 0x0001,  //客户端公网地址，因未加密被弃用 
            USERNAME            = 0x0006,  //认证用户名
            MESSAGE_INTEGRITY   = 0x0008,  //MAC-SHA1消息完整性校验
            ERROR_CODE          = 0x0009,  //错误代码及原因描述（格式：3位码+文本） 
            UNKNOWN_ATTRIBUTES  = 0x000A,  //标记客户端不支持的属性列表
            REALM               = 0x0014,  //服务器认证域（类似HTTP Basic Auth的Realm）
            NONCE               = 0x0015,  //服务器生成的随机数，用于认证防重放 
            XOR_MAPPED_ADDRESS  = 0x0020,  //异或加密的公网地址（取代MAPPED-ADDRESS）
            SOFTWARE            = 0x0022,  //客户端/服务端软件标识（如"ExampleStunServer v1.0"） 
            FINGERPRINT         = 0x8028,  //消息尾部的CRC32校验码（用于兼容性检测）
        }
        class Stun {
        public:
            virtual std::unordered_map<std::string, Address>
        
        private:
            int sockfd_;
        };
    }
    
}

#endif  // INCLUDE_NAT_STUNMESSAGE_H_
