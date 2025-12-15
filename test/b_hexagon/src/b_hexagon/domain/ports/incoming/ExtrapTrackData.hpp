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
 * @brief Track kinematic state in ECEF coordinate system with timing information
 * @details Represents extrapolated track data with position, velocity, and timing.
 *          Auto-generated from ExtrapTrackData.json schema.
 *          
 * ECEF Coordinate System:
 * - Origin: Earth's center of mass
 * - X-axis: Intersection of equator and prime meridian
 * - Y-axis: Intersection of equator and 90°E meridian  
 * - Z-axis: North pole
 * - Units: meters for position, m/s for velocity
 * 
 * Data Flow:
 * External Source → ExtrapTrackData (this) → CalculatorService → DelayCalcTrackData
 * 
 * Binary Format: 76 bytes total
 * - Little-endian packed struct (no padding)
 * - Compatible with C/C++/Python serialization
 * 
 * MISRA C++ 2023 compliant implementation
 * Direction: incoming (from external sources)
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
    // All setters validate input ranges (throws std::out_of_range on invalid values)
    // All getters are noexcept (safe to call, never throws)
    int32_t getTrackId() const noexcept;
    void setTrackId(const int32_t& value);  // Validates: 1 to 4,294,967,295

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
    // Validates all fields are within acceptable ranges
    // Returns true if all validations pass, false otherwise
    // noexcept - catches exceptions internally, never throws
    [[nodiscard]] bool isValid() const noexcept;

    // Binary Serialization - MISRA compliant
    // serialize(): Convert object to binary format (76 bytes)
    // deserialize(): Parse binary data into object fields
    // getSerializedSize(): Returns fixed size (76 bytes)
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

} // namespace ports
} // namespace domain