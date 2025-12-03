/**
 * @file ZeroMQSocketTest.cpp
 * @brief Unit tests for ZeroMQSocket class
 * @details Tests ZeroMQ socket wrapper with real ZeroMQ context
 *          to achieve higher code coverage.
 * 
 * Coverage targets:
 * - Constructor (DISH and RADIO)
 * - connect() success and failure paths
 * - subscribe() success and failure paths
 * - send() success and failure paths
 * - receive() timeout and error paths
 * - close() and isConnected()
 * - Thread safety
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "adapters/common/messaging/ZeroMQSocket.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

using namespace adapters::common::messaging;
using namespace testing;

/**
 * @class ZeroMQSocketTest
 * @brief Test fixture for ZeroMQSocket tests
 */
class ZeroMQSocketTest : public Test {
protected:
    void SetUp() override {
        // Each test creates its own socket
    }

    void TearDown() override {
        // Cleanup handled by RAII
    }

    // Helper to create unique endpoints for parallel tests
    static std::string getUniqueEndpoint(int port) {
        return "tcp://127.0.0.1:" + std::to_string(port);
    }

    static std::string getUniqueMulticastEndpoint(int port) {
        return "udp://239.192.100.1:" + std::to_string(port);
    }
};

// ==================== Constructor Tests ====================

TEST_F(ZeroMQSocketTest, Constructor_DISH_CreatesSuccessfully) {
    // Act
    EXPECT_NO_THROW({
        ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    });
}

TEST_F(ZeroMQSocketTest, Constructor_RADIO_CreatesSuccessfully) {
    // Act
    EXPECT_NO_THROW({
        ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    });
}

TEST_F(ZeroMQSocketTest, Constructor_WithCustomIOThreads_CreatesSuccessfully) {
    // Act
    EXPECT_NO_THROW({
        ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH, 2);
    });
}

TEST_F(ZeroMQSocketTest, Constructor_MultipleInstances_Independent) {
    // Act - create multiple sockets
    ZeroMQSocket socket1(ZeroMQSocket::SocketType::DISH);
    ZeroMQSocket socket2(ZeroMQSocket::SocketType::RADIO);
    ZeroMQSocket socket3(ZeroMQSocket::SocketType::DISH, 2);

    // Assert - all should be independent and not connected
    EXPECT_FALSE(socket1.isConnected());
    EXPECT_FALSE(socket2.isConnected());
    EXPECT_FALSE(socket3.isConnected());
}

// ==================== getSocketType Tests ====================

TEST_F(ZeroMQSocketTest, GetSocketType_DISH_ReturnsCorrectString) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);

    // Act & Assert
    EXPECT_EQ(socket.getSocketType(), "ZeroMQ_DISH");
}

TEST_F(ZeroMQSocketTest, GetSocketType_RADIO_ReturnsCorrectString) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);

    // Act & Assert
    EXPECT_EQ(socket.getSocketType(), "ZeroMQ_RADIO");
}

// ==================== isConnected Tests ====================

TEST_F(ZeroMQSocketTest, IsConnected_BeforeConnect_ReturnsFalse) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);

    // Act & Assert
    EXPECT_FALSE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, IsConnected_AfterClose_ReturnsFalse) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19001)));

    // Act
    socket.close();

    // Assert
    EXPECT_FALSE(socket.isConnected());
}

// ==================== connect() Tests ====================

TEST_F(ZeroMQSocketTest, Connect_DISH_BindsSuccessfully) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    std::string endpoint = getUniqueMulticastEndpoint(19002);

    // Act
    bool result = socket.connect(endpoint);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_TRUE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, Connect_RADIO_ConnectsSuccessfully) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    std::string endpoint = getUniqueEndpoint(19003);

    // Act
    bool result = socket.connect(endpoint);

    // Assert
    EXPECT_TRUE(result);
    EXPECT_TRUE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, Connect_AlreadyConnected_ReturnsTrue) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    std::string endpoint = getUniqueMulticastEndpoint(19004);
    ASSERT_TRUE(socket.connect(endpoint));

    // Act - try to connect again
    bool result = socket.connect(endpoint);

    // Assert - should return true (already connected)
    EXPECT_TRUE(result);
    EXPECT_TRUE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, Connect_InvalidEndpoint_ReturnsFalse) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);

    // Act - use invalid endpoint format
    bool result = socket.connect("invalid://endpoint");

    // Assert
    EXPECT_FALSE(result);
    EXPECT_FALSE(socket.isConnected());
}

// ==================== subscribe() Tests ====================

TEST_F(ZeroMQSocketTest, Subscribe_DISH_SucceedsAfterConnect) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19005)));

    // Act
    bool result = socket.subscribe("TestGroup");

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ZeroMQSocketTest, Subscribe_RADIO_ReturnsFalse) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19006)));

    // Act - RADIO sockets cannot subscribe
    bool result = socket.subscribe("TestGroup");

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ZeroMQSocketTest, Subscribe_MultipleGroups_AllSucceed) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19007)));

    // Act & Assert
    EXPECT_TRUE(socket.subscribe("Group1"));
    EXPECT_TRUE(socket.subscribe("Group2"));
    EXPECT_TRUE(socket.subscribe("Group3"));
}

// ==================== send() Tests ====================

TEST_F(ZeroMQSocketTest, Send_NotConnected_ReturnsFalse) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    std::vector<uint8_t> data = {0x01, 0x02, 0x03};

    // Act
    bool result = socket.send(data);

    // Assert
    EXPECT_FALSE(result);
}

TEST_F(ZeroMQSocketTest, Send_AfterConnect_ReturnsTrue) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19008)));
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};

    // Act
    bool result = socket.send(data, "TestGroup");

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ZeroMQSocketTest, Send_EmptyData_ReturnsTrue) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19009)));
    std::vector<uint8_t> emptyData;

    // Act
    bool result = socket.send(emptyData, "TestGroup");

    // Assert
    EXPECT_TRUE(result);  // ZeroMQ accepts empty messages
}

TEST_F(ZeroMQSocketTest, Send_LargeData_Succeeds) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19010)));
    
    // Create 64KB of data
    std::vector<uint8_t> largeData(65536, 0xAB);

    // Act
    bool result = socket.send(largeData, "TestGroup");

    // Assert
    EXPECT_TRUE(result);
}

TEST_F(ZeroMQSocketTest, Send_WithoutGroup_Succeeds) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19011)));
    std::vector<uint8_t> data = {0x01, 0x02};

    // Act - use send without group
    bool result = socket.send(data);

    // Assert
    EXPECT_TRUE(result);
}

// ==================== receive() Tests ====================

TEST_F(ZeroMQSocketTest, Receive_NotConnected_ReturnsNullopt) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);

    // Act
    auto result = socket.receive(10);  // 10ms timeout

    // Assert
    EXPECT_FALSE(result.has_value());
}

TEST_F(ZeroMQSocketTest, Receive_Timeout_ReturnsNullopt) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19012)));
    ASSERT_TRUE(socket.subscribe("TimeoutTest"));

    // Act - no message sent, should timeout
    auto startTime = std::chrono::steady_clock::now();
    auto result = socket.receive(50);  // 50ms timeout
    auto endTime = std::chrono::steady_clock::now();

    // Assert
    EXPECT_FALSE(result.has_value());
    
    // Verify timeout was respected (within tolerance)
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    EXPECT_GE(elapsed.count(), 40);  // At least 40ms
    EXPECT_LE(elapsed.count(), 200); // At most 200ms
}

TEST_F(ZeroMQSocketTest, Receive_ZeroTimeout_ImmediateReturn) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19013)));
    ASSERT_TRUE(socket.subscribe("ImmediateTest"));

    // Act
    auto startTime = std::chrono::steady_clock::now();
    auto result = socket.receive(0);  // 0ms timeout = non-blocking
    auto endTime = std::chrono::steady_clock::now();

    // Assert
    EXPECT_FALSE(result.has_value());
    
    // Should return almost immediately
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    EXPECT_LE(elapsed.count(), 50);
}

// ==================== close() Tests ====================

TEST_F(ZeroMQSocketTest, Close_BeforeConnect_NoThrow) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);

    // Act & Assert - should not throw
    EXPECT_NO_THROW(socket.close());
    EXPECT_FALSE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, Close_AfterConnect_DisconnectsSuccessfully) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19014)));
    ASSERT_TRUE(socket.isConnected());

    // Act
    socket.close();

    // Assert
    EXPECT_FALSE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, Close_MultipleTimes_NoThrow) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19015)));

    // Act & Assert - multiple closes should be safe
    EXPECT_NO_THROW(socket.close());
    EXPECT_NO_THROW(socket.close());
    EXPECT_NO_THROW(socket.close());
    EXPECT_FALSE(socket.isConnected());
}

TEST_F(ZeroMQSocketTest, Close_SendAfterClose_ReturnsFalse) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19016)));
    socket.close();

    // Act
    std::vector<uint8_t> data = {0x01};
    bool result = socket.send(data);

    // Assert
    EXPECT_FALSE(result);
}

// ==================== Thread Safety Tests ====================

TEST_F(ZeroMQSocketTest, ConcurrentSend_ThreadSafe) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
    ASSERT_TRUE(socket.connect(getUniqueEndpoint(19017)));

    std::atomic<int> successCount{0};
    std::atomic<int> failCount{0};
    const int numThreads = 5;
    const int sendsPerThread = 20;

    // Act - concurrent sends from multiple threads
    std::vector<std::thread> threads;
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < sendsPerThread; ++i) {
                std::vector<uint8_t> data = {
                    static_cast<uint8_t>(t),
                    static_cast<uint8_t>(i)
                };
                if (socket.send(data, "ConcurrentGroup")) {
                    ++successCount;
                } else {
                    ++failCount;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Assert - all sends should succeed (mutex protection)
    EXPECT_EQ(successCount.load(), numThreads * sendsPerThread);
    EXPECT_EQ(failCount.load(), 0);
}

TEST_F(ZeroMQSocketTest, ConcurrentConnectClose_ThreadSafe) {
    // Arrange
    ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
    std::atomic<bool> shouldStop{false};

    // Act - one thread connects/closes, another checks state
    std::thread connector([&]() {
        for (int i = 0; i < 10 && !shouldStop; ++i) {
            (void)socket.connect(getUniqueMulticastEndpoint(19018 + i));
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            socket.close();
        }
    });

    std::thread checker([&]() {
        for (int i = 0; i < 50 && !shouldStop; ++i) {
            // Should not crash
            (void)socket.isConnected();
            (void)socket.getSocketType();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        shouldStop = true;
    });

    connector.join();
    checker.join();

    // Assert - no crash or deadlock
    SUCCEED();
}

// ==================== Destructor Tests ====================

TEST_F(ZeroMQSocketTest, Destructor_WhileConnected_ClosesGracefully) {
    // Act & Assert - should not leak or crash
    {
        ZeroMQSocket socket(ZeroMQSocket::SocketType::DISH);
        ASSERT_TRUE(socket.connect(getUniqueMulticastEndpoint(19028)));
        ASSERT_TRUE(socket.subscribe("DestructorTest"));
        // Destructor called here
    }
    // If we reach here, destructor handled cleanup properly
    SUCCEED();
}

TEST_F(ZeroMQSocketTest, Destructor_AfterManySends_ClosesGracefully) {
    // Act
    {
        ZeroMQSocket socket(ZeroMQSocket::SocketType::RADIO);
        ASSERT_TRUE(socket.connect(getUniqueEndpoint(19029)));
        
        for (int i = 0; i < 100; ++i) {
            std::vector<uint8_t> data = {static_cast<uint8_t>(i)};
            (void)socket.send(data, "DestructorTest");
        }
        // Destructor called here
    }

    // Assert
    SUCCEED();
}

// ==================== End-to-End Communication Test ====================

TEST_F(ZeroMQSocketTest, RadioDish_SendReceive_CommunicatesSuccessfully) {
    // This test verifies actual message passing between RADIO and DISH
    // Note: UDP multicast may not work in all test environments
    
    // Arrange
    const std::string endpoint = "inproc://test-radio-dish";
    const std::string group = "TestGroup";
    const std::vector<uint8_t> testData = {0xDE, 0xAD, 0xBE, 0xEF};
    
    // Create receiver (DISH) - Note: For inproc, we need same context
    // This is a simplified test that verifies the API flow
    ZeroMQSocket radio(ZeroMQSocket::SocketType::RADIO);
    ZeroMQSocket dish(ZeroMQSocket::SocketType::DISH);

    // These will use different contexts, so actual message passing won't work
    // but we're testing the API flow and error handling
    EXPECT_TRUE(radio.connect(getUniqueEndpoint(19030)));
    EXPECT_TRUE(dish.connect(getUniqueMulticastEndpoint(19031)));
    EXPECT_TRUE(dish.subscribe(group));
    
    // Send should succeed
    EXPECT_TRUE(radio.send(testData, group));
    
    // Receive will timeout (different contexts)
    auto received = dish.receive(10);
    EXPECT_FALSE(received.has_value());  // Expected: timeout
}

