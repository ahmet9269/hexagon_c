/**
 * @file NetworkConfig.hpp
 * @brief Centralized network configuration for all adapters
 * @details Contains all ZeroMQ endpoints, ports, and connection settings.
 *          Separated from domain models to maintain clean architecture.
 * 
 * @author c_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#pragma once

#include <string>
#include <cstdint>

namespace config {

/**
 * @brief Network configuration namespace
 * @details Contains all network-related constants and configurations.
 *          Use these constants instead of hardcoding values in adapters.
 */
namespace network {

// ═══════════════════════════════════════════════════════════════════
// Environment Configuration
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Environment type enumeration
 */
enum class Environment {
    Development,   ///< Local development (localhost)
    Container,     ///< Container/Docker environment
    Production     ///< Production environment (multicast)
};

// Current active environment
#ifdef PRODUCTION_BUILD
    static constexpr Environment CURRENT_ENV = Environment::Production;
#else
    static constexpr Environment CURRENT_ENV = Environment::Development;
#endif

// ═══════════════════════════════════════════════════════════════════
// Development/Container Environment (localhost TCP)
// ═══════════════════════════════════════════════════════════════════
namespace dev {
    static constexpr const char* HOST = "127.0.0.1";
    static constexpr const char* PROTOCOL = "tcp";
    
    // DelayCalcTrackData (incoming from B_hexagon)
    static constexpr int DELAY_CALC_PORT = 15002;
    static constexpr const char* DELAY_CALC_ENDPOINT = "tcp://127.0.0.1:15002";
    static constexpr const char* DELAY_CALC_GROUP = "DelayCalcTrackData";
    
    // FinalCalcTrackData (outgoing to external systems)
    static constexpr int FINAL_CALC_PORT = 15003;
    static constexpr const char* FINAL_CALC_ENDPOINT = "tcp://127.0.0.1:15003";
    static constexpr const char* FINAL_CALC_GROUP = "FinalCalcTrackData";
}

// ═══════════════════════════════════════════════════════════════════
// Production Environment (UDP Multicast)
// ═══════════════════════════════════════════════════════════════════
namespace prod {
    static constexpr const char* PROTOCOL = "udp";
    static constexpr const char* UDN_PREFIX = "udn;";
    
    // DelayCalcTrackData (incoming from B_hexagon)
    static constexpr const char* DELAY_CALC_MULTICAST = "239.1.1.5";
    static constexpr int DELAY_CALC_PORT = 9595;
    static constexpr const char* DELAY_CALC_ENDPOINT = "udp://udn;239.1.1.5:9595";
    static constexpr const char* DELAY_CALC_GROUP = "DelayCalcTrackData";
    
    // FinalCalcTrackData (outgoing to external systems)
    static constexpr const char* FINAL_CALC_MULTICAST = "239.1.1.5";
    static constexpr int FINAL_CALC_PORT = 9597;
    static constexpr const char* FINAL_CALC_ENDPOINT = "udp://udn;239.1.1.5:9597";
    static constexpr const char* FINAL_CALC_GROUP = "FinalCalcTrackData";
}

// ═══════════════════════════════════════════════════════════════════
// Helper Functions
// ═══════════════════════════════════════════════════════════════════

/**
 * @brief Get DelayCalcTrackData endpoint based on current environment
 * @return Endpoint string for ZeroMQ socket
 */
inline const char* getDelayCalcEndpoint() noexcept {
    return (CURRENT_ENV == Environment::Production) 
        ? prod::DELAY_CALC_ENDPOINT 
        : dev::DELAY_CALC_ENDPOINT;
}

/**
 * @brief Get FinalCalcTrackData endpoint based on current environment
 * @return Endpoint string for ZeroMQ socket
 */
inline const char* getFinalCalcEndpoint() noexcept {
    return (CURRENT_ENV == Environment::Production) 
        ? prod::FINAL_CALC_ENDPOINT 
        : dev::FINAL_CALC_ENDPOINT;
}

/**
 * @brief Get DelayCalcTrackData group name
 * @return Group name for RADIO/DISH pattern
 */
inline const char* getDelayCalcGroup() noexcept {
    return (CURRENT_ENV == Environment::Production) 
        ? prod::DELAY_CALC_GROUP 
        : dev::DELAY_CALC_GROUP;
}

/**
 * @brief Get FinalCalcTrackData group name
 * @return Group name for RADIO/DISH pattern
 */
inline const char* getFinalCalcGroup() noexcept {
    return (CURRENT_ENV == Environment::Production) 
        ? prod::FINAL_CALC_GROUP 
        : dev::FINAL_CALC_GROUP;
}

// ═══════════════════════════════════════════════════════════════════
// ZeroMQ Socket Configuration
// ═══════════════════════════════════════════════════════════════════
namespace zmq {
    // Socket types documentation
    static constexpr const char* INCOMING_SOCKET_TYPE = "DISH";  // For subscribers
    static constexpr const char* OUTGOING_SOCKET_TYPE = "RADIO"; // For publishers
    
    // Socket options
    static constexpr int DEFAULT_RECV_TIMEOUT_MS = 100;
    static constexpr int DEFAULT_SEND_TIMEOUT_MS = 100;
    static constexpr int DEFAULT_LINGER_MS = 0;
    static constexpr int HIGH_WATER_MARK = 0;  // 0 = unlimited
    
    // Thread configuration
    static constexpr int IO_THREADS = 1;
    static constexpr int REALTIME_THREAD_PRIORITY = 95;
    static constexpr int DEDICATED_CPU_CORE = 1;
}

} // namespace network
} // namespace config
