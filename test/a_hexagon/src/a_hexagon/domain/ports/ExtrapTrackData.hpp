/**
 * @file ExtrapTrackData.hpp
 * @brief Header file for ExtrapTrackData model class
 * @details Contains extrapolated track state in ECEF coordinate system
 *          with detailed timing information for latency analysis.
 *          Auto-generated MISRA C++ 2023 compliant implementation.
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * Direction: outgoing
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
 * @class ExtrapTrackData
 * @brief Extrapolated track data model for output
 * @details Contains extrapolated kinematic state in ECEF coordinate system
 *          with detailed timing information. Used for measuring hop latencies
 *          across the hexagon pipeline (a_hexagon → b_hexagon → c_hexagon).
 * 
 * @invariant trackId_ is unique per track
 * @invariant All timestamps are in microseconds
 * @invariant Position values are in meters (ECEF)
 * @invariant Velocity values are in m/s (ECEF)
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @note Direction: outgoing
 */
class ExtrapTrackData final {
public:
    // MISRA C++ 2023 compliant constructors
    explicit ExtrapTrackData() noexcept;
    
    // Copy constructor
    ExtrapTrackData(const ExtrapTrackData& other) = default;
    
    // Move constructor
    ExtrapTrackData(ExtrapTrackData&& other) noexcept = default;
    
    // Copy assignment operator
    ExtrapTrackData& operator=(const ExtrapTrackData& other) = default;
    
    // Move assignment operator
    ExtrapTrackData& operator=(ExtrapTrackData&& other) noexcept = default;
    
    // Destructor
    ~ExtrapTrackData() = default;
    
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

    // Validation - MISRA compliant
    [[nodiscard]] bool isValid() const noexcept;

    // Binary Serialization - MISRA compliant
    [[nodiscard]] std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data) noexcept;
    [[nodiscard]] std::size_t getSerializedSize() const noexcept;

private:
    // Member variables
    /// İz için benzersiz tam sayı kimliği
    int32_t trackId_;
    /// ECEF koordinatlarındaki X ekseni hızı (m/s)
    double xVelocityECEF_;
    /// ECEF koordinatlarındaki Y ekseni hızı (m/s)
    double yVelocityECEF_;
    /// ECEF koordinatlarındaki Z ekseni hızı (m/s)
    double zVelocityECEF_;
    /// ECEF koordinatlarındaki X ekseni konumu (metre)
    double xPositionECEF_;
    /// ECEF koordinatlarındaki Y ekseni konumu (metre)
    double yPositionECEF_;
    /// ECEF koordinatlarındaki Z ekseni konumu (metre)
    double zPositionECEF_;
    /// Original güncelleme zamanı (mikrosaniye)
    int64_t originalUpdateTime_;
    /// Son güncelleme zamanı (mikrosaniye)
    int64_t updateTime_;
    /// İlk atlamanın gönderildiği zaman (mikrosaniye)
    int64_t firstHopSentTime_;

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
};

}  // namespace model
}  // namespace domain
