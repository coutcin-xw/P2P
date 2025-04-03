// 测试用例示例
TEST(SocketTest, LocalAddressBinding) {
    auto socket = CreateSocket(SocketType::UDP, false);
    auto addr = CreateAddress("0.0.0.0", 0);
    socket->bind(*addr);
    
    auto local = socket->getLocalAddress();
    ASSERT_NE(local, nullptr);
    std::cout << "Bound to: " << local->toString() << std::endl;
}
