#include "domain/logic/TrackDataExtrapolator.hpp"
#include <chrono>
#include <thread>
namespace domain {
namespace logic {
using std::vector;
using namespace domain::model;
TrackDataExtrapolator::TrackDataExtrapolator(domain::ports::outgoing::ExtrapTrackDataOutgoingPort* outgoingPort) 
    : outgoingPort_(outgoingPort) {
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
	        if (outgoingPort_) {
	            outgoingPort_->sendExtrapTrackData(extrap); // Artık tek veri gönderiyoruz
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
