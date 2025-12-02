/**
 * @file MockMessageSocket.hpp
 * @brief GMock implementation of IMessageSocket for unit testing
 * @details Enables testing adapters without network dependencies.
 *          Part of the Dependency Inversion architecture.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef B_HEXAGON_TEST_MOCK_MESSAGE_SOCKET_HPP
#define B_HEXAGON_TEST_MOCK_MESSAGE_SOCKET_HPP

#include <gmock/gmock.h>
#include "adapters/common/messaging/IMessageSocket.hpp"
#include <vector>
#include <string>
#include <optional>

namespace mocks {

/**
 * @class MockMessageSocket
 * @brief GMock-based mock socket for unit testing
 */
class MockMessageSocket : public adapters::common::messaging::IMessageSocket {
public:
    MockMessageSocket() = default;
    ~MockMessageSocket() override = default;

    // GMock method declarations
    MOCK_METHOD(bool, connect, (const std::string& endpoint), (override));
    MOCK_METHOD(bool, send, (const std::vector<uint8_t>& data), (override));
    MOCK_METHOD(bool, send, (const std::vector<uint8_t>& data, const std::string& group), (override));
    MOCK_METHOD((std::optional<std::vector<uint8_t>>), receive, (int32_t timeoutMs), (override));
    MOCK_METHOD(bool, subscribe, (const std::string& group), (override));
    MOCK_METHOD(void, close, (), (noexcept, override));
    MOCK_METHOD(bool, isConnected, (), (const, noexcept, override));
    MOCK_METHOD(std::string, getSocketType, (), (const, noexcept, override));
};

}  // namespace mocks

#endif  // B_HEXAGON_TEST_MOCK_MESSAGE_SOCKET_HPP
