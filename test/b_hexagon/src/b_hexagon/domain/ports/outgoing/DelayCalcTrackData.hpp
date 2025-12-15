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
 * @brief Track state with multi-hop delay calculation in ECEF coordinates
 * @details Extends ExtrapTrackData with additional timing metrics for delay analysis.
 *          Auto-generated from DelayCalcTrackData.json schema.
 *          
 * Extended Fields (beyond ExtrapTrackData):
 * - firstHopDelayTime: Latency from data generation to first reception (microseconds)
 * - secondHopSentTime: Timestamp when forwarding processed data (microseconds)
 * 
 * Multi-Hop Delay Tracking:
 * 1. Original data generated (originalUpdateTime)
 * 2. First hop transmission (firstHopSentTime)
 * 3. First hop reception and processing (current time)
 * 4. First hop delay calculated (firstHopDelayTime = current - firstHopSentTime)
 * 5. Second hop transmission begins (secondHopSentTime)
 * 
 * Use Case:
 * Measure end-to-end latency across distributed tracking systems
 * Identify bottlenecks in multi-hop data pipelines
 * 
 * Binary Format: 96 bytes total (76 from ExtrapTrackData + 16 new)
 * - Little-endian packed struct (no padding)
 * 
 * MISRA C++ 2023 compliant implementation
 * Direction: incoming (from domain processing)
 */
class DelayCalcTrackData final {
public:
    // MISRA C++ 2023 compliant constructors
    explicit DelayCalcTrackData() noexcept;
    
    // Copy constructor
    DelayCalcTrackData(const DelayCalcTrackData& other) = default;
    
    // Move constructor
    DelayCalcTrackData(DelayCalcTrackData&& other) noexcept = default;
    
    // Copy assignment operator
    DelayCalcTrackData& operator=(const DelayCalcTrackData& other) = default;
    
    // Move assignment operator
    DelayCalcTrackData& operator=(DelayCalcTrackData&& other) noexcept = default;
    
    // Destructor
    ~DelayCalcTrackData() = default;
    
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
    /// İlk atlama için hesaplanan gecikme (mikrosaniye)
    int64_t firstHopDelayTime_;
    /// İkinci atlamanın gönderildiği zaman (mikrosaniye)
    int64_t secondHopSentTime_;

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
};

} // namespace ports
} // namespace domain
