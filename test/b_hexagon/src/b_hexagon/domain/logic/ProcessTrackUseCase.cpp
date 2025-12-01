//#include "CalculatorService.hpp"
#include "domain/logic/ProcessTrackUseCase.hpp"
#include "utils/Logger.hpp"
#include <stdexcept>
#include <future>


namespace domain{
namespace logic {
    ProcessTrackUseCase::ProcessTrackUseCase(
    std::unique_ptr<CalculatorService> calculator,
    std::unique_ptr<domain::ports::outgoing::IDelayCalcTrackDataOutgoingPort> dataSender
    ): calculator_(std::move(calculator)),
    dataSender_(std::move(dataSender)){
        if (!calculator_) {
            throw std::invalid_argument("CalculatorService cannot be null");
        }
        if (!dataSender_) {
            throw std::invalid_argument("DelayCalcTrackDataOutgoingPort cannot be null");
        }
            Logger::info("ProcessTrackUseCase initialized with CalculatorService and DelayCalcTrackDataOutgoingPort");
    }

void ProcessTrackUseCase::onDataReceived(const domain::ports::ExtrapTrackData& data){
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
            // Process the track data through domain logic
            domain::ports::DelayCalcTrackData processedData = calculator_->calculateDelay(data);
            
            Logger::info("Processed track ", data.getTrackId(), 
                        " -> Delay: ", processedData.getFirstHopDelayTime(), "μs");
                        //"SecondHop: ", processedData.getSecondHopSentTime(), "μs");
            // Send processed data via outgoing adapter
            sendDelayCalcTrackData(processedData);
            
            Logger::debug("Successfully sent processed track data for ID=", data.getTrackId());
            
        } catch (const std::exception& e) {
            Logger::error("Error processing track ", data.getTrackId(), ": ", e.what());
        }
} 

void ProcessTrackUseCase::sendDelayCalcTrackData (const domain::ports::DelayCalcTrackData& processedData) {
    if(dataSender_){
        std::async(std::launch::async, [this, processedData](){
             dataSender_->sendDelayCalcTrackData(processedData);
        } );
    }else{
        Logger::error("Error while sending data.");
    }
}

}
}
