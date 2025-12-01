/**
 * @file DelayCalcTrackDataZeroMQOutgoingAdapter.hpp
 * @brief ZeroMQ RADIO adapter for outbound data transmission using UDP multicast
 */

#pragma once

//#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types
#include "domain/ports/outgoing/IDelayCalcTrackDataOutgoingPort.hpp"        // Outbound port interface
#include "domain/ports/DelayCalcTrackData.hpp"    // Domain data model
#include <zmq_config.hpp>
#include <zmq.hpp>                                       // ZeroMQ C++ bindings
#include <string>                                        // String utilities

// Using declarations for convenience
using domain::ports::DelayCalcTrackData;

/**
 * @class DelayCalcTrackDataZeroMQOutgoingAdapter
 * @brief ZeroMQ RADIO adapter for broadcasting DelayCalcTrackData via UDP multicast
 * 
 * Implements the RADIO/DISH UDP pattern:
 * - RADIO socket connects to UDP multicast endpoint
 * - Messages are serialized to binary format for efficiency
 * - Messages are tagged with group identifiers
 * - DISH receivers filter by group using zmq_join()
 * 
 * Uses configuration constants from DelayCalcTrackData domain model.
 */
class DelayCalcTrackDataZeroMQOutgoingAdapter final : public domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort {
public:
    // Default constructor - uses configuration from DelayCalcTrackData domain model
    explicit DelayCalcTrackDataZeroMQOutgoingAdapter();

    // Destructor - RAII cleanup
    ~DelayCalcTrackDataZeroMQOutgoingAdapter() noexcept = default;

    // Disable copy operations
    DelayCalcTrackDataZeroMQOutgoingAdapter(const DelayCalcTrackDataZeroMQOutgoingAdapter& other) = delete;
    DelayCalcTrackDataZeroMQOutgoingAdapter& operator=(const DelayCalcTrackDataZeroMQOutgoingAdapter& other) = delete;

    // Allow move operations
    DelayCalcTrackDataZeroMQOutgoingAdapter(DelayCalcTrackDataZeroMQOutgoingAdapter&& other) noexcept = default;
    DelayCalcTrackDataZeroMQOutgoingAdapter& operator=(DelayCalcTrackDataZeroMQOutgoingAdapter&& other) noexcept = default;

    // Send data via RADIO socket (implements DelayCalcTrackDataOutgoingPort)
    void sendDelayCalcTrackData(const DelayCalcTrackData& data) override;

private:
    // Network configuration constants
    // Original UDP Multicast configuration (for production environment)
    // static constexpr const char* MULTICAST_ADDRESS = "239.1.1.1";
    // static constexpr int PORT = 9002;
    // static constexpr const char* ZMQ_PROTOCOL = "udp";
    // static constexpr const char* ZMQ_MULTICAST_ADDRESS = "239.1.1.1";
    // static constexpr int ZMQ_PORT = 9002;
    
    // TCP localhost configuration (for development/container environment)
    static constexpr const char* MULTICAST_ADDRESS = "127.0.0.1";
    static constexpr int PORT = 15002;
    static constexpr const char* ZMQ_PROTOCOL = "tcp";
    static constexpr const char* ZMQ_MULTICAST_ADDRESS = "127.0.0.1";
    static constexpr int ZMQ_PORT = 15002;
    
    // ZeroMQ DISH socket configuration (incoming)
    static constexpr const char* ZMQ_SOCKET_TYPE = "DISH";
    static constexpr bool IS_SUBSCRIBER = true;

    zmq::context_t context_;      // ZeroMQ context
    zmq::socket_t socket_;        // RADIO socket for UDP multicast
    const std::string group_;     // Group identifier for DISH filtering
};
