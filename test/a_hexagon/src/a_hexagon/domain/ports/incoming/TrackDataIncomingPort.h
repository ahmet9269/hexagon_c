/**
 * @file TrackDataIncomingPort.h
 * @brief Legacy header - redirects to ITrackDataIncomingPort.hpp
 * @deprecated Use ITrackDataIncomingPort.hpp instead
 */
#pragma once
#include "ITrackDataIncomingPort.hpp"

namespace domain {
namespace ports {
namespace incoming {
    // Type alias for backward compatibility
    using TrackDataIncomingPort = ITrackDataIncomingPort;
}
}
}
