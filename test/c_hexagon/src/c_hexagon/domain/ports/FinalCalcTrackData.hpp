#pragma once

// MISRA C++ 2023 compliant includes
#include <string>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <cstring>

namespace domain {
namespace ports {

/**
 * @brief Contains final track state in ECEF coordinate system and complete breakdown of multi-hop delay calculations.
 * Auto-generated from FinalCalcTrackData.json
 * MISRA C++ 2023 compliant implementation
 * Direction: outgoing
 */
class FinalCalcTrackData final {
public:
    // Network configuration constants
    // Original UDP Multicast configuration (for production environment)
    // static constexpr const char* MULTICAST_ADDRESS = "239.1.1.5";
    // static constexpr int PORT = 9597;
    
    // TCP localhost configuration (for development/container environment)
    static constexpr const char* MULTICAST_ADDRESS = "127.0.0.1";
    static constexpr int PORT = 15003;
    
    // ZeroMQ RADIO socket configuration (outgoing)
    static constexpr const char* ZMQ_SOCKET_TYPE = "RADIO";
    static constexpr bool IS_PUBLISHER = true;

    // MISRA C++ 2023 compliant constructors
    explicit FinalCalcTrackData() noexcept;
    
    // Copy constructor
    FinalCalcTrackData(const FinalCalcTrackData& other) = default;
    
    // Move constructor
    FinalCalcTrackData(FinalCalcTrackData&& other) noexcept = default;
    
    // Copy assignment operator
    FinalCalcTrackData& operator=(const FinalCalcTrackData& other) = default;
    
    // Move assignment operator
    FinalCalcTrackData& operator=(FinalCalcTrackData&& other) noexcept = default;
    
    // Destructor
    ~FinalCalcTrackData() = default;
    
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

    int64_t getUpdateTime() const noexcept;
    void setUpdateTime(const int64_t& value);

    int64_t getFirstHopSentTime() const noexcept;
    void setFirstHopSentTime(const int64_t& value);

    int64_t getFirstHopDelayTime() const noexcept;
    void setFirstHopDelayTime(const int64_t& value);

    int64_t getSecondHopSentTime() const noexcept;
    void setSecondHopSentTime(const int64_t& value);

    int64_t getSecondHopDelayTime() const noexcept;
    void setSecondHopDelayTime(const int64_t& value);

    int64_t getTotalDelayTime() const noexcept;
    void setTotalDelayTime(const int64_t& value);

    int64_t getThirdHopSentTime() const noexcept;
    void setThirdHopSentTime(const int64_t& value);

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
    /// Last update time (microseconds)
    int64_t updateTime_;
    /// First hop sent timestamp (microseconds)
    int64_t firstHopSentTime_;
    /// Calculated delay for first hop (microseconds)
    int64_t firstHopDelayTime_;
    /// Second hop sent timestamp (microseconds)
    int64_t secondHopSentTime_;
    /// Calculated delay for second hop (microseconds)
    int64_t secondHopDelayTime_;
    /// Total calculated delay (microseconds)
    int64_t totalDelayTime_;
    /// Third hop sent timestamp (microseconds)
    int64_t thirdHopSentTime_;

    // Validation functions - MISRA compliant
    void validateTrackId(int32_t value) const;
    void validateXVelocityECEF(double value) const;
    void validateYVelocityECEF(double value) const;
    void validateZVelocityECEF(double value) const;
    void validateXPositionECEF(double value) const;
    void validateYPositionECEF(double value) const;
    void validateZPositionECEF(double value) const;
    void validateOriginalUpdateTime(int64_t value) const;
    void validateUpdateTime(int64_t value) const;
    void validateFirstHopSentTime(int64_t value) const;
    void validateFirstHopDelayTime(int64_t value) const;
    void validateSecondHopSentTime(int64_t value) const;
    void validateSecondHopDelayTime(int64_t value) const;
    void validateTotalDelayTime(int64_t value) const;
    void validateThirdHopSentTime(int64_t value) const;
};

} // namespace ports
} // namespace domain
