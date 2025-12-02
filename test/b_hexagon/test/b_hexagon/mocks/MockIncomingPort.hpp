/**
 * @file MockIncomingPort.hpp
 * @brief GMock implementation of IncomingPort for unit testing
 * @details Enables testing incoming adapters without domain dependencies.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef B_HEXAGON_TEST_MOCK_INCOMING_PORT_HPP
#define B_HEXAGON_TEST_MOCK_INCOMING_PORT_HPP

#include <gmock/gmock.h>
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"
#include "domain/ports/ExtrapTrackData.hpp"

namespace mocks {

/**
 * @class MockIncomingPort
 * @brief GMock-based mock incoming port for unit testing
 */
class MockIncomingPort : public domain::ports::incoming::IExtrapTrackDataIncomingPort {
public:
    MockIncomingPort() = default;
    ~MockIncomingPort() override = default;

    // GMock method declaration
    MOCK_METHOD(void, onDataReceived, (const domain::ports::ExtrapTrackData& data), (override));
};

}  // namespace mocks

#endif  // B_HEXAGON_TEST_MOCK_INCOMING_PORT_HPP
