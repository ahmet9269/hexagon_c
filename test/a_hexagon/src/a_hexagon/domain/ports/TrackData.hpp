/**
 * @file TrackData.hpp
 * @brief Header file for TrackData model class
 * @details Contains track state in ECEF coordinate system with timing information
 *          Auto-generated MISRA C++ 2023 compliant implementation
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * Direction: incoming
 */

#pragma once

// MISRA C++ 2023 compliant includes
#include <string>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <cstring>

namespace domain {
namespace model {

/**
 * @brief Input data model for track information
 * @details This class represents incoming track data containing position,
 *          velocity, and timing information in ECEF coordinate system.
 * 
 * @note MISRA C++ 2023 compliant implementation
 */
class TrackData final {
public:
    // Network configuration constants
    // Original UDP Multicast configuration (for production environment)
    // static constexpr const char* MULTICAST_ADDRESS = "239.1.1.1";
    // static constexpr int PORT = 9000;
    // static constexpr const char* ZMQ_PROTOCOL = "udp";
    
    // TCP localhost configuration (for development/container environment)
    static constexpr const char* MULTICAST_ADDRESS = "127.0.0.1";
    static constexpr int PORT = 15000;
    static constexpr const char* ZMQ_PROTOCOL = "tcp";
    
    // ZeroMQ DISH socket configuration (incoming)
    static constexpr const char* ZMQ_SOCKET_TYPE = "DISH";
    static constexpr bool IS_SUBSCRIBER = true;

    // MISRA C++ 2023 compliant constructors
    explicit TrackData() noexcept;
    
    // Copy constructor
    TrackData(const TrackData& other) = default;
    
    // Move constructor
    TrackData(TrackData&& other) noexcept = default;
    
    // Copy assignment operator
    TrackData& operator=(const TrackData& other) = default;
    
    // Move assignment operator
    TrackData& operator=(TrackData&& other) noexcept = default;
    
    // Destructor
    ~TrackData() = default;
    
    // Getters and Setters
    int32_t getTrackId() const noexcept;
    void setTrackId(const int32_t& value);

    double getXVelocityECEF() const noexcept;
    void setXVelocityECEF(const double& value);

    double getYVelocityECEF() const noexcept;
    void setYVelocityECEF(const double& value);

    double getZVelocityECEF() const noexcept;
    void setZVelocityECEF(const double& value);

    double getXPositionECEF() const noexcept;
    void setXPositionECEF(const double& value);

    double getYPositionECEF() const noexcept;
    void setYPositionECEF(const double& value);

    double getZPositionECEF() const noexcept;
    void setZPositionECEF(const double& value);

    int64_t getOriginalUpdateTime() const noexcept;
    void setOriginalUpdateTime(const int64_t& value);

    // Validation - MISRA compliant
    [[nodiscard]] bool isValid() const noexcept;

    // Binary Serialization - MISRA compliant
    [[nodiscard]] std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data) noexcept;
    [[nodiscard]] std::size_t getSerializedSize() const noexcept;

private:
    // Member variables
    /// Unique integer identifier for the track
    int32_t trackId_;
    /// X-axis velocity in ECEF coordinates (m/s)
    double xVelocityECEF_;
    /// Y-axis velocity in ECEF coordinates (m/s)
    double yVelocityECEF_;
    /// Z-axis velocity in ECEF coordinates (m/s)
    double zVelocityECEF_;
    /// X-axis position in ECEF coordinates (meters)
    double xPositionECEF_;
    /// Y-axis position in ECEF coordinates (meters)
    double yPositionECEF_;
    /// Z-axis position in ECEF coordinates (meters)
    double zPositionECEF_;
    /// Original update time (microseconds)
    int64_t originalUpdateTime_;

    // Validation functions - MISRA compliant
    void validateTrackId(int32_t value) const;
    void validatePosition(double value) const;
    void validateVelocity(double value) const;
    void validateTimestamp(int64_t value) const;
};

} // namespace model
} // namespace domain
