/**
 * @file CustomFinalTrackDataAdapter.hpp
 * @brief Custom adapter for final track data transmission
 */

#ifndef CUSTOM_FINAL_TRACK_DATA_ADAPTER_HPP
#define CUSTOM_FINAL_TRACK_DATA_ADAPTER_HPP

#include "../../domain/ports/outgoing/FinalTrackDataOutgoingPort.hpp"
#include "../../domain/model/FinalCalcTrackData.hpp"

namespace adapters {
namespace outgoing {

/**
 * @brief Custom implementation of FinalTrackDataOutgoingPort
 * 
 * This adapter implements the outgoing port using custom technology.
 * Implementation details are intentionally left for custom development.
 */
class CustomFinalTrackDataAdapter : public domain::ports::outgoing::FinalTrackDataOutgoingPort {
private:
    // TODO: Add your custom connection/client members here
    // Example: CustomClient* client_;
    // Example: std::string endpoint_;
    // Example: ConnectionConfig config_;
    
public:
    /**
     * @brief Constructor - Initialize your custom adapter
     */
    CustomFinalTrackDataAdapter();
    
    /**
     * @brief Destructor - Cleanup your custom resources
     */
    ~CustomFinalTrackDataAdapter() override;
    
    /**
     * @brief Send final track data using your custom technology
     * 
     * @param data Final calculated track data to be transmitted
     * @throws std::runtime_error if transmission fails
     */
    void sendFinalTrackData(const domain::model::FinalCalcTrackData& data) override;
    
    /**
     * @brief Check if your custom connection is ready
     * 
     * @return true if ready, false otherwise
     */
    bool isReady() const override;
    
private:
    /**
     * @brief Initialize your custom connection
     * TODO: Implement your connection setup logic
     */
    void initializeConnection();
    
    /**
     * @brief Cleanup your custom connection
     * TODO: Implement your cleanup logic
     */
    void cleanup();
};

} // namespace outgoing
} // namespace adapters

#endif // CUSTOM_FINAL_TRACK_DATA_ADAPTER_HPP