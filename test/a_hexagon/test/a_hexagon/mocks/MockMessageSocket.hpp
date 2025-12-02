/**
 * @file MockMessageSocket.hpp
 * @brief Mock implementation of IMessageSocket for unit testing
 * @details Thread-safe in-memory message queue for testing adapters
 *          without real network dependencies. Enables %95+ test coverage.
 * 
 * Features:
 * - In-memory message queuing (no network)
 * - Thread-safe operations
 * - Call counting for verification
 * - Configurable behavior (success/failure)
 * - Deterministic timeout simulation
 * 
 * Usage:
 * @code
 * auto mockSocket = std::make_unique<MockMessageSocket>();
 * 
 * // Queue incoming message for receive()
 * mockSocket->queueIncomingMessage(testData);
 * 
 * // Inject into adapter
 * auto adapter = std::make_unique<IncomingAdapter>(mockPort, std::move(mockSocket));
 * adapter->start();
 * 
 * // Verify sent messages
 * auto sentMessages = mockSocket->getSentMessages();
 * EXPECT_EQ(sentMessages.size(), expectedCount);
 * @endcode
 * 
 * @author a_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#ifndef A_HEXAGON_TEST_MOCKS_MOCK_MESSAGE_SOCKET_HPP
#define A_HEXAGON_TEST_MOCKS_MOCK_MESSAGE_SOCKET_HPP

#include "adapters/common/messaging/IMessageSocket.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <atomic>

namespace a_hexagon {
namespace test {
namespace mocks {

/**
 * @class MockMessageSocket
 * @brief Mock socket for unit testing without network
 * @details Provides in-memory message queue for deterministic testing
 */
class MockMessageSocket : public adapters::common::messaging::IMessageSocket {
public:
    MockMessageSocket() = default;
    ~MockMessageSocket() override = default;

    // ==================== IMessageSocket Interface ====================

    [[nodiscard]] bool connect(const std::string& endpoint) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (failConnect_) {
            return false;
        }
        
        endpoint_ = endpoint;
        connected_ = true;
        connectCallCount_++;
        return true;
    }

    [[nodiscard]] bool send(const std::vector<uint8_t>& data) override {
        return send(data, "");
    }

    [[nodiscard]] bool send(const std::vector<uint8_t>& data, const std::string& group) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!connected_ || failSend_) {
            return false;
        }
        
        SentMessage msg{data, group, std::chrono::steady_clock::now()};
        sentMessages_.push(msg);
        sendCallCount_++;
        cv_.notify_one();  // Wake up any waiting receivers
        return true;
    }

    [[nodiscard]] std::optional<std::vector<uint8_t>> receive(int32_t timeoutMs) override {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (!connected_) {
            return std::nullopt;
        }
        
        receiveCallCount_++;
        
        // Check if message already available
        if (!incomingMessages_.empty()) {
            auto data = incomingMessages_.front();
            incomingMessages_.pop();
            return data;
        }
        
        // Non-blocking mode
        if (timeoutMs == 0) {
            return std::nullopt;
        }
        
        // Wait for message with timeout
        bool hasMessage = cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
            [this] { return !incomingMessages_.empty() || !connected_; });
        
        if (hasMessage && !incomingMessages_.empty()) {
            auto data = incomingMessages_.front();
            incomingMessages_.pop();
            return data;
        }
        
        return std::nullopt;  // Timeout
    }

    void close() noexcept override {
        std::lock_guard<std::mutex> lock(mutex_);
        connected_ = false;
        closeCallCount_++;
        cv_.notify_all();  // Wake up any waiting receivers
    }

    [[nodiscard]] bool isConnected() const noexcept override {
        std::lock_guard<std::mutex> lock(mutex_);
        return connected_;
    }

    [[nodiscard]] std::string getSocketType() const noexcept override {
        return "Mock";
    }

    // ==================== Test Helper Methods ====================

    /**
     * @brief Queue message for receive() to return
     * @param data Binary data to be received
     */
    void queueIncomingMessage(const std::vector<uint8_t>& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        incomingMessages_.push(data);
        cv_.notify_one();
    }

    /**
     * @brief Queue multiple messages for receive()
     * @param messages Vector of binary messages
     */
    void queueIncomingMessages(const std::vector<std::vector<uint8_t>>& messages) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& msg : messages) {
            incomingMessages_.push(msg);
        }
        cv_.notify_all();
    }

    /**
     * @brief Get number of queued incoming messages
     */
    [[nodiscard]] size_t getIncomingMessageCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return incomingMessages_.size();
    }

    /**
     * @brief Message sent via send()
     */
    struct SentMessage {
        std::vector<uint8_t> data;
        std::string group;
        std::chrono::steady_clock::time_point timestamp;
    };

    /**
     * @brief Get all sent messages
     */
    [[nodiscard]] std::queue<SentMessage> getSentMessages() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sentMessages_;
    }

    /**
     * @brief Get sent message count
     */
    [[nodiscard]] size_t getSentMessageCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sentMessages_.size();
    }

    /**
     * @brief Get last sent message
     */
    [[nodiscard]] std::optional<SentMessage> getLastSentMessage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (sentMessages_.empty()) {
            return std::nullopt;
        }
        
        auto temp = sentMessages_;
        SentMessage last;
        while (!temp.empty()) {
            last = temp.front();
            temp.pop();
        }
        return last;
    }

    /**
     * @brief Clear all message queues and reset counters
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!sentMessages_.empty()) sentMessages_.pop();
        while (!incomingMessages_.empty()) incomingMessages_.pop();
        sendCallCount_ = 0;
        receiveCallCount_ = 0;
        connectCallCount_ = 0;
        closeCallCount_ = 0;
        failConnect_ = false;
        failSend_ = false;
    }

    // ==================== Call Count Verification ====================

    [[nodiscard]] int getSendCallCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sendCallCount_;
    }

    [[nodiscard]] int getReceiveCallCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return receiveCallCount_;
    }

    [[nodiscard]] int getConnectCallCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return connectCallCount_;
    }

    [[nodiscard]] int getCloseCallCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closeCallCount_;
    }

    [[nodiscard]] std::string getEndpoint() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return endpoint_;
    }

    // ==================== Failure Injection ====================

    /**
     * @brief Set connect() to fail
     */
    void setFailConnect(bool fail) {
        std::lock_guard<std::mutex> lock(mutex_);
        failConnect_ = fail;
    }

    /**
     * @brief Set send() to fail
     */
    void setFailSend(bool fail) {
        std::lock_guard<std::mutex> lock(mutex_);
        failSend_ = fail;
    }

    /**
     * @brief Simulate disconnection
     */
    void simulateDisconnect() {
        close();
    }

    // Delete copy/move for thread safety
    MockMessageSocket(const MockMessageSocket&) = delete;
    MockMessageSocket& operator=(const MockMessageSocket&) = delete;
    MockMessageSocket(MockMessageSocket&&) = delete;
    MockMessageSocket& operator=(MockMessageSocket&&) = delete;

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    
    bool connected_{false};
    std::string endpoint_;
    
    std::queue<std::vector<uint8_t>> incomingMessages_;
    std::queue<SentMessage> sentMessages_;
    
    int sendCallCount_{0};
    int receiveCallCount_{0};
    int connectCallCount_{0};
    int closeCallCount_{0};
    
    bool failConnect_{false};
    bool failSend_{false};
};

} // namespace mocks
} // namespace test
} // namespace a_hexagon

#endif // A_HEXAGON_TEST_MOCKS_MOCK_MESSAGE_SOCKET_HPP
