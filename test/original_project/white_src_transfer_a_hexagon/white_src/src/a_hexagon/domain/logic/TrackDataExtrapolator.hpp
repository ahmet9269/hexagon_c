
#ifndef TRACK_DATA_EXTRAPOLATOR_H 
#define TRACK_DATA_EXTRAPOLATOR_H 

#include <domain/ports/outgoing/ExtrapTrackDataOutgoingPort.hpp>
#include "domain/ports/incoming/TrackDataIncomingPort.h"
#include <vector>
#include "domain/model/TrackData.hpp"
#include "domain/model/ExtrapTrackData.hpp"

namespace domain {
namespace logic {

using domain::model::TrackData;     
using domain::model::ExtrapTrackData;
using std::vector; 

class TrackDataExtrapolator :public domain::ports::incoming::TrackDataIncomingPort{
private:
    domain::ports::outgoing::ExtrapTrackDataOutgoingPort* outgoingPort_;

public: 
    TrackDataExtrapolator(domain::ports::outgoing::ExtrapTrackDataOutgoingPort* outgoingPort);
    void processAndForwardTrackData(const domain::model::TrackData& trackData);
    void extrapolateTrackData(const TrackData& trackData, double inputFrequency,double outputFrequency);

};
}
}
#endif
