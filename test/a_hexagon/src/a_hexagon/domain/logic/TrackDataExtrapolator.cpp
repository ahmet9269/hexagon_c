/**
 * @file TrackDataExtrapolator.cpp
 * @brief Implementation of TrackDataExtrapolator domain service
 * @details Performs linear extrapolation of track data from input frequency
 *          (8Hz) to output frequency (100Hz). Uses simple kinematic equations
 *          for position extrapolation based on constant velocity assumption.
 * 
 * @author A Hexagon Team
 * @version 2.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#include "domain/logic/TrackDataExtrapolator.hpp"
#include <chrono>
#include <thread>

namespace domain {
namespace logic {

using std::vector;
using namespace domain::model;

// Primary constructor with smart pointer (MISRA compliant)
TrackDataExtrapolator::TrackDataExtrapolator(
    std::unique_ptr<domain::ports::outgoing::IExtrapTrackDataOutgoingPort> outgoingPort) 
    : outgoingPort_(std::move(outgoingPort)) {
}

// Legacy constructor for backward compatibility
TrackDataExtrapolator::TrackDataExtrapolator(
    domain::ports::outgoing::IExtrapTrackDataOutgoingPort* outgoingPort) 
    : rawOutgoingPort_(outgoingPort) {
    // Note: This constructor does NOT take ownership
    // For legacy code compatibility only - use unique_ptr constructor for new code
}
void TrackDataExtrapolator::extrapolateTrackData(const TrackData& trackData, double inputFrequency,double outputFrequency) {
	double inputInterval = 1.0 / inputFrequency;
	double outputInterval = 1.0 / outputFrequency;

	auto startTime = std::chrono::high_resolution_clock::now();

	    for (double t = 0; t < inputInterval; t += outputInterval) {
	        ExtrapTrackData extrap;


	        // Track data kopyala
	        extrap.setTrackId(trackData.getTrackId());
	        extrap.setXVelocityECEF(trackData.getXVelocityECEF());
	        extrap.setYVelocityECEF(trackData.getYVelocityECEF());
	        extrap.setZVelocityECEF(trackData.getZVelocityECEF());

	        // Position'ı velocity ile extrapole et
	        extrap.setXPositionECEF(trackData.getXPositionECEF() + trackData.getXVelocityECEF() * t);
	        extrap.setYPositionECEF(trackData.getYPositionECEF() + trackData.getYVelocityECEF() * t);
	        extrap.setZPositionECEF(trackData.getZPositionECEF() + trackData.getZVelocityECEF() * t);

	        // Timestamp'leri ayarla
	        extrap.setUpdateTime(trackData.getOriginalUpdateTime() * 1000 + static_cast<long>(t * 1000000)); // ms to μs + offset
	        extrap.setOriginalUpdateTime(trackData.getOriginalUpdateTime()); // milisaniye olarak kalsın

	        auto now = std::chrono::high_resolution_clock::now();
	        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	        extrap.setFirstHopSentTime(micros); // mikrosaniye cinsinden

        // Her veriyi hemen gönder (tek tek)
        if (outgoingPort_ != nullptr) {
            outgoingPort_->sendExtrapTrackData(extrap);
        } else if (rawOutgoingPort_ != nullptr) {
            rawOutgoingPort_->sendExtrapTrackData(extrap);
        }
        // 200Hz için 5ms bekle (gerçek zamanlı extrapolation)
	        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}
void TrackDataExtrapolator::processAndForwardTrackData(const TrackData& trackData) {

	// 8Hz TrackData'yı alıp 100Hz ExtrapTrackData olarak gönder
	extrapolateTrackData(trackData,8.0,100.0);
}
}
}
