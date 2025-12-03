/**
 * @file MockOutgoingPort.hpp
 * @brief GMock implementation of OutgoingPort for unit testing
 * @details Enables testing use cases without adapter dependencies.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 */

#ifndef B_HEXAGON_TEST_MOCK_OUTGOING_PORT_HPP
#define B_HEXAGON_TEST_MOCK_OUTGOING_PORT_HPP

#include <gmock/gmock.h>
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"
#include "domain/ports/DelayCalcTrackData.hpp"

namespace mocks {

/**
 * @class MockOutgoingPort
 * @brief GMock-based mock outgoing port for unit testing
 */
class MockOutgoingPort : public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
public:
    MockOutgoingPort() = default;
    ~MockOutgoingPort() override = default;

    // GMock method declaration
    MOCK_METHOD(void, sendDelayCalcTrackData, (const domain::ports::DelayCalcTrackData& data), (override));
    
    // Alias for compatibility with tests using 'send'
    bool send(const domain::ports::DelayCalcTrackData& data) {
        sendDelayCalcTrackData(data);
        return true;  // Mock always succeeds by default
    }
};

}  // namespace mocks

#endif  // B_HEXAGON_TEST_MOCK_OUTGOING_PORT_HPP
