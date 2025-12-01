/**
 * @file TrackData.cpp
 * @brief Implementation of TrackData model class
 * @details MISRA C++ 2023 compliant implementation with binary serialization
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 */

#include "domain/ports/TrackData.hpp"
#include <limits>
#include <cstring>

namespace domain {
namespace model {

// Default constructor
TrackData::TrackData() noexcept
    : trackId_(0)
    , xVelocityECEF_(0.0)
    , yVelocityECEF_(0.0)
    , zVelocityECEF_(0.0)
    , xPositionECEF_(0.0)
    , yPositionECEF_(0.0)
    , zPositionECEF_(0.0)
    , originalUpdateTime_(0) {
}

// Getters
int32_t TrackData::getTrackId() const noexcept {
    return trackId_;
}

double TrackData::getXVelocityECEF() const noexcept {
    return xVelocityECEF_;
}

double TrackData::getYVelocityECEF() const noexcept {
    return yVelocityECEF_;
}

double TrackData::getZVelocityECEF() const noexcept {
    return zVelocityECEF_;
}

double TrackData::getXPositionECEF() const noexcept {
    return xPositionECEF_;
}

double TrackData::getYPositionECEF() const noexcept {
    return yPositionECEF_;
}

double TrackData::getZPositionECEF() const noexcept {
    return zPositionECEF_;
}

int64_t TrackData::getOriginalUpdateTime() const noexcept {
    return originalUpdateTime_;
}

// Setters with validation
void TrackData::setTrackId(const int32_t& value) {
    validateTrackId(value);
    trackId_ = value;
}

void TrackData::setXVelocityECEF(const double& value) {
    validateVelocity(value);
    xVelocityECEF_ = value;
}

void TrackData::setYVelocityECEF(const double& value) {
    validateVelocity(value);
    yVelocityECEF_ = value;
}

void TrackData::setZVelocityECEF(const double& value) {
    validateVelocity(value);
    zVelocityECEF_ = value;
}

void TrackData::setXPositionECEF(const double& value) {
    validatePosition(value);
    xPositionECEF_ = value;
}

void TrackData::setYPositionECEF(const double& value) {
    validatePosition(value);
    yPositionECEF_ = value;
}

void TrackData::setZPositionECEF(const double& value) {
    validatePosition(value);
    zPositionECEF_ = value;
}

void TrackData::setOriginalUpdateTime(const int64_t& value) {
    validateTimestamp(value);
    originalUpdateTime_ = value;
}

// Validation
bool TrackData::isValid() const noexcept {
    // Check for NaN/Inf in double values
    if (std::isnan(xVelocityECEF_) || std::isinf(xVelocityECEF_) ||
        std::isnan(yVelocityECEF_) || std::isinf(yVelocityECEF_) ||
        std::isnan(zVelocityECEF_) || std::isinf(zVelocityECEF_) ||
        std::isnan(xPositionECEF_) || std::isinf(xPositionECEF_) ||
        std::isnan(yPositionECEF_) || std::isinf(yPositionECEF_) ||
        std::isnan(zPositionECEF_) || std::isinf(zPositionECEF_)) {
        return false;
    }
    
    // Basic range checks
    if (trackId_ < 0) {
        return false;
    }
    
    if (originalUpdateTime_ < 0) {
        return false;
    }
    
    return true;
}

// Binary Serialization
std::size_t TrackData::getSerializedSize() const noexcept {
    return sizeof(int32_t) +      // trackId
           (6 * sizeof(double)) + // velocities and positions
           sizeof(int64_t);       // originalUpdateTime
}

std::vector<uint8_t> TrackData::serialize() const {
    std::vector<uint8_t> buffer(getSerializedSize());
    std::size_t offset = 0;
    
    // TrackId
    std::memcpy(buffer.data() + offset, &trackId_, sizeof(int32_t));
    offset += sizeof(int32_t);
    
    // Velocities
    std::memcpy(buffer.data() + offset, &xVelocityECEF_, sizeof(double));
    offset += sizeof(double);
    std::memcpy(buffer.data() + offset, &yVelocityECEF_, sizeof(double));
    offset += sizeof(double);
    std::memcpy(buffer.data() + offset, &zVelocityECEF_, sizeof(double));
    offset += sizeof(double);
    
    // Positions
    std::memcpy(buffer.data() + offset, &xPositionECEF_, sizeof(double));
    offset += sizeof(double);
    std::memcpy(buffer.data() + offset, &yPositionECEF_, sizeof(double));
    offset += sizeof(double);
    std::memcpy(buffer.data() + offset, &zPositionECEF_, sizeof(double));
    offset += sizeof(double);
    
    // Timestamp
    std::memcpy(buffer.data() + offset, &originalUpdateTime_, sizeof(int64_t));
    
    return buffer;
}

bool TrackData::deserialize(const std::vector<uint8_t>& data) noexcept {
    if (data.size() < getSerializedSize()) {
        return false;
    }
    
    try {
        std::size_t offset = 0;
        
        // TrackId
        std::memcpy(&trackId_, data.data() + offset, sizeof(int32_t));
        offset += sizeof(int32_t);
        
        // Velocities
        std::memcpy(&xVelocityECEF_, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&yVelocityECEF_, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&zVelocityECEF_, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        
        // Positions
        std::memcpy(&xPositionECEF_, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&yPositionECEF_, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        std::memcpy(&zPositionECEF_, data.data() + offset, sizeof(double));
        offset += sizeof(double);
        
        // Timestamp
        std::memcpy(&originalUpdateTime_, data.data() + offset, sizeof(int64_t));
        
        return isValid();
        
    } catch (...) {
        return false;
    }
}

// Private validation functions
void TrackData::validateTrackId(int32_t value) const {
    if (value < 0) {
        throw std::out_of_range("TrackId must be non-negative");
    }
}

void TrackData::validatePosition(double value) const {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument("Position value must be a valid number");
    }
}

void TrackData::validateVelocity(double value) const {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument("Velocity value must be a valid number");
    }
}

void TrackData::validateTimestamp(int64_t value) const {
    if (value < 0) {
        throw std::out_of_range("Timestamp must be non-negative");
    }
}

} // namespace model
} // namespace domain
