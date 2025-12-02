/**
 * @file ExtrapTrackDataZeroMQIncomingAdapter.cpp
 * @brief ZeroMQ DISH adapter for inbound data reception using UDP multicast
 */

#define ZMQ_BUILD_DRAFT_API  // Enable RADIO/DISH socket types

//#include "adapters/incoming/ExtrapTrackDataZeroMQIncomingAdapter.hpp"  // Own header
#include "ExtrapTrackDataZeroMQIncomingAdapter.hpp" // Own header
#include "../../../utils/Logger.hpp"                        // Logging
#include <stdexcept>      // Exception types
#include <cstring>        // Memory operations
#include <sstream>        // String stream for endpoint formatting

// Using declarations for convenience
using domain::ports::ExtrapTrackData;


// Default constructor 
ExtrapTrackDataZeroMQIncomingAdapter::ExtrapTrackDataZeroMQIncomingAdapter(domain::ports::incoming::IExtrapTrackDataIncomingPort* dataReceiver)
    : context_(1),
      socket_(context_, ZMQ_DISH),  // Use DISH for UDP multicast (RADIO/DISH pattern)
      group_("ExtrapTrackData"),  // Group name matches message type
      dataReceiver_(dataReceiver) {
    
    try {
        // Build endpoint from ExtrapTrackData configuration constants
        std::ostringstream oss;
        // UDP multicast endpoint (RADIO/DISH pattern)
        oss << ZMQ_PROTOCOL << "://"
            << ZMQ_MULTICAST_ADDRESS << ":"
            << ZMQ_PORT;
        std::string endpoint = oss.str();
        
        // DISH binds (listens for RADIO connections)
        socket_.bind(endpoint);
        socket_.join(group_.c_str());  // Join group for RADIO/DISH pattern
        
        Logger::info("ExtrapTrackDataZeroMQIncomingAdapter configured - endpoint: ", endpoint, ", group: ", group_);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("ExtrapTrackDataZeroMQIncomingAdapter config error: " + std::string(e.what()));
    }
}

// Start continuous message reception loop
void ExtrapTrackDataZeroMQIncomingAdapter::startReceiving() {
    while (true) {
        
        // Use C++ API for RADIO/DISH group message handling
        zmq::message_t message;
        
        try {
            // Blocking receive operation using C++ API
            auto result = socket_.recv(message, zmq::recv_flags::none);
            if (!result) {
                continue; // No message received
            }
            
            // Extract binary payload
            const uint8_t* binaryData = static_cast<const uint8_t*>(message.data());
            std::size_t dataSize = message.size();
            
            // Deserialize binary data to domain object
            ExtrapTrackData data = deserializeBinary(binaryData, dataSize);
            
            Logger::info("[b_hexagon] ExtrapTrackData received - TrackID: ", data.getTrackId(), ", Size: ", message.size(), " bytes");
            
            // Notify domain layer
            if (dataReceiver_ != nullptr) {
                dataReceiver_->onDataReceived(data);
            }
            
        } catch (const zmq::error_t& e) {
            throw std::runtime_error("ZeroMQ receive failed: " + std::string(e.what()));
        } catch (const std::exception& ex) {
            // Log error but continue processing (don't terminate reception loop)
            Logger::error("Message processing error: ", ex.what());
        }
    }
}

// Deserialize binary data to ExtrapTrackData object
ExtrapTrackData ExtrapTrackDataZeroMQIncomingAdapter::deserializeBinary(const uint8_t* data, std::size_t size) {
    ExtrapTrackData extrapData; // default constructed
    std::vector<uint8_t> binary_data(data, data + size);
    
    if (extrapData.deserialize(binary_data)) {
        return extrapData;
    } else {
        throw std::runtime_error("ExtrapTrackDataZeroMQIncomingAdapter: Binary deserialization failed");
    }
}
