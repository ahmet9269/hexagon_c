/**
 * @file ExtrapTrackDataZeroMQIncomingAdapter.hpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 */

#pragma once

//#include "domain/ports/incoming/ExtrapTrackDataIncomingPort.hpp"      // Inbound port interface
#include "domain/ports/incoming/IExtrapTrackDataIncomingPort.hpp"  // Inbound port interface
#include "domain/ports/ExtrapTrackData.hpp"                   // Domain data model
#include <zmq_config.hpp>
#include <zmq.hpp>                                       // ZeroMQ C++ bindings
#include <string>                                        // String utilities
#include <memory>                                        // Smart pointers
#include <stdexcept>                                     // Exception types

// Using declarations for convenience
using domain::ports::ExtrapTrackData;
//using domain::ports::incoming::IExtrapTrackDataIncomingPort;

/**
 * @class ExtrapTrackDataZeroMQIncomingAdapter
 * @brief ZeroMQ DISH adapter for receiving ExtrapTrackData via UDP multicast
 * 
 * Implements the RADIO/DISH UDP pattern:
 * - DISH socket binds to UDP multicast endpoint
 * - Joins specific group using zmq_join()
 * - Receives group-filtered messages from RADIO senders
 * - Deserializes binary messages to ExtrapTrackData objects
 * 
 * Uses configuration constants from ExtrapTrackData domain model.
 */
class ExtrapTrackDataZeroMQIncomingAdapter final {
public:
    // Default constructor - uses configuration from ExtrapTrackData domain model
    explicit ExtrapTrackDataZeroMQIncomingAdapter(domain::ports::incoming::IExtrapTrackDataIncomingPort* dataReceiver = nullptr);

    // Destructor - RAII cleanup
    ~ExtrapTrackDataZeroMQIncomingAdapter() noexcept = default;

    // Disable copy operations
    ExtrapTrackDataZeroMQIncomingAdapter(const ExtrapTrackDataZeroMQIncomingAdapter& other) = delete;
    ExtrapTrackDataZeroMQIncomingAdapter& operator=(const ExtrapTrackDataZeroMQIncomingAdapter& other) = delete;

    // Allow move operations
    ExtrapTrackDataZeroMQIncomingAdapter(ExtrapTrackDataZeroMQIncomingAdapter&& other) noexcept = default;
    ExtrapTrackDataZeroMQIncomingAdapter& operator=(ExtrapTrackDataZeroMQIncomingAdapter&& other) noexcept = default;

    // Start continuous message reception loop
    void startReceiving();

private:
// Network configuration constants
    // Original UDP Multicast configuration (for production environment)
    // static constexpr const char* MULTICAST_ADDRESS = "239.1.1.1";
    // static constexpr int PORT = 9001;
    // static constexpr const char* ZMQ_PROTOCOL = "udp";
    // static constexpr const char* ZMQ_MULTICAST_ADDRESS = "239.1.1.1";
    // static constexpr int ZMQ_PORT = 9001;
    
    // UDP Multicast configuration (RADIO/DISH pattern)
    static constexpr const char* MULTICAST_ADDRESS = "239.1.1.2";
    static constexpr int PORT = 9001;
    static constexpr const char* ZMQ_PROTOCOL = "udp";
    static constexpr const char* ZMQ_MULTICAST_ADDRESS = "239.1.1.2";
    static constexpr int ZMQ_PORT = 9001;
    
    // ZeroMQ RADIO socket configuration (outgoing)
    static constexpr const char* ZMQ_SOCKET_TYPE = "RADIO";
    static constexpr bool IS_PUBLISHER = true;

    

    // Deserialize binary data to ExtrapTrackData object
    static ExtrapTrackData deserializeBinary(const uint8_t* data, std::size_t size);

    zmq::context_t context_;           // ZeroMQ context
    zmq::socket_t socket_;             // DISH socket for UDP multicast
    const std::string group_;          // Group identifier for filtering
    domain::ports::incoming::IExtrapTrackDataIncomingPort* const dataReceiver_; // Domain notification interface
};
