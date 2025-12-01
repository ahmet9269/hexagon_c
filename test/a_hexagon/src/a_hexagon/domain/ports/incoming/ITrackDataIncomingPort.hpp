#ifndef TRACK_DATA_INCOMING_PORT_H 
#define TRACK_DATA_INCOMING_PORT_H 
#include "domain/ports/TrackData.hpp"
namespace domain {
namespace ports {
namespace incoming {
class ITrackDataIncomingPort {
public: 
    virtual ~ITrackDataIncomingPort() = default;
    virtual void processAndForwardTrackData(const model::TrackData& trackData) = 0;
};
}
}
}
#endif
