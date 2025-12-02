//#include "CalculatorService.hpp"
#include "domain/logic/ProcessTrackUseCase.hpp"
#include "utils/Logger.hpp"
#include <stdexcept>


namespace domain {
namespace logic {

// ==================== Constructor (unique_ptr - legacy) ====================
ProcessTrackUseCase::ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::unique_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender
    ): calculator_(std::move(calculator)),
    dataSender_(std::move(dataSender)){
        if (!calculator_) {
            throw std::invalid_argument("ICalculatorService cannot be null");
        }
        if (!dataSender_) {
            throw std::invalid_argument("IDelayCalcTrackDataOutgoingPort cannot be null");
        }
        Logger::info("ProcessTrackUseCase initialized (unique_ptr ownership)");
    }

// ==================== Constructor (shared_ptr - Thread-per-Type) ====================
ProcessTrackUseCase::ProcessTrackUseCase(
    std::unique_ptr<ICalculatorService> calculator,
    std::shared_ptr<ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender
    ): calculator_(std::move(calculator)),
    dataSender_(std::move(dataSender)){
        if (!calculator_) {
            throw std::invalid_argument("ICalculatorService cannot be null");
        }
        if (!dataSender_) {
            throw std::invalid_argument("IDelayCalcTrackDataOutgoingPort cannot be null");
        }
        Logger::info("ProcessTrackUseCase initialized (shared_ptr ownership - Thread-per-Type)");
    }

void ProcessTrackUseCase::onDataReceived(const ports::ExtrapTrackData& data){
        Logger::info("=== RECEIVED DATA FROM A_HEXAGON ===");
        Logger::info("Track ID: ", data.getTrackId());
        Logger::info("Position ECEF: (", data.getXPositionECEF(), ", ", data.getYPositionECEF(), ", ", data.getZPositionECEF(), ")");
        Logger::info("Velocity ECEF: (", data.getXVelocityECEF(), ", ", data.getYVelocityECEF(), ", ", data.getZVelocityECEF(), ")");
        Logger::info("Update Time: ", data.getUpdateTime());
        Logger::info("=====================================");
        
        if (!data.isValid()) {
            Logger::warn("Invalid track data received: ID=", data.getTrackId());
            return;
        }
        
        try {
            // Process the track data through domain logic (via ICalculatorService abstraction)
            ports::DelayCalcTrackData processedData = calculator_->calculateDelay(data);
            
            Logger::info("Processed track ", data.getTrackId(), 
                        " -> Delay: ", processedData.getFirstHopDelayTime(), "μs");
            
            // Send processed data via outgoing adapter (synchronous - thread-safe)
            sendDelayCalcTrackData(processedData);
            
            Logger::debug("Successfully sent processed track data for ID=", data.getTrackId());
            
        } catch (const std::exception& e) {
            Logger::error("Error processing track ", data.getTrackId(), ": ", e.what());
        }
} 

void ProcessTrackUseCase::sendDelayCalcTrackData(const ports::DelayCalcTrackData& processedData) {
    if (dataSender_) {
        // Direct synchronous call - thread-safe since each message is processed sequentially
        // in the incoming adapter's thread. No async needed for this use case.
        dataSender_->sendDelayCalcTrackData(processedData);
    } else {
        Logger::error("Error while sending data: dataSender is null");
    }
}

}
}
