/**
 * @file MessagePipeline.hpp
 * @brief Message Processing Pipeline for Thread-per-Type Architecture
 * @details Encapsulates a complete message processing flow with incoming and
 *          outgoing adapters. Each pipeline operates in isolation.
 * 
 * Pipeline Structure:
 * ┌─────────────────────────────────┐
 * │       MessagePipeline          │
 * ├─────────────────────────────────┤
 * │  IncomingAdapter (Thread T1)   │
 * │           ↓                    │
 * │     [Domain Service]           │
 * │           ↓                    │
 * │  OutgoingAdapter (Optional)    │
 * └─────────────────────────────────┘
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IAdapter.hpp
 * @see AdapterManager.hpp
 */

#pragma once

#include "IAdapter.hpp"
#include <memory>
#include <string>
#include <mutex>

namespace adapters {

/**
 * @class MessagePipeline
 * @brief Encapsulates a complete message processing pipeline
 * @details Groups related incoming and outgoing adapters for a specific
 *          message type. Each pipeline runs in isolation with its own thread(s).
 * 
 * Design Rationale:
 * - Class instead of struct: Has behavior (start/stop/isRunning)
 * - Encapsulation: Internal state management
 * - Single Responsibility: Only manages one message type's flow
 * 
 * @invariant name_ is non-empty after construction
 * @invariant At least one adapter (incoming or outgoing) should be set
 */
class MessagePipeline {
public:
    /**
     * @brief Default constructor
     */
    MessagePipeline() = default;

    /**
     * @brief Construct pipeline with name
     * @param name Pipeline identifier (e.g., "ExtrapTrackData", "SensorData")
     */
    explicit MessagePipeline(std::string name) 
        : name_(std::move(name)) {}

    /**
     * @brief Construct fully configured pipeline
     * @param name Pipeline identifier
     * @param incoming Incoming adapter (receives messages)
     * @param outgoing Outgoing adapter (sends messages, optional)
     */
    MessagePipeline(std::string name,
                    std::shared_ptr<IAdapter> incoming,
                    std::shared_ptr<IAdapter> outgoing = nullptr)
        : name_(std::move(name))
        , incomingAdapter_(std::move(incoming))
        , outgoingAdapter_(std::move(outgoing)) {}

    /**
     * @brief Default destructor
     */
    ~MessagePipeline() = default;

    // Move semantics - mutex requires custom implementation
    MessagePipeline(MessagePipeline&& other) noexcept
        : name_(std::move(other.name_))
        , incomingAdapter_(std::move(other.incomingAdapter_))
        , outgoingAdapter_(std::move(other.outgoingAdapter_))
        // mutex_ is default constructed (not moved)
    {}
    
    MessagePipeline& operator=(MessagePipeline&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(mutex_);
            std::lock_guard<std::mutex> otherLock(other.mutex_);
            name_ = std::move(other.name_);
            incomingAdapter_ = std::move(other.incomingAdapter_);
            outgoingAdapter_ = std::move(other.outgoingAdapter_);
        }
        return *this;
    }

    // Disable copy - mutex is not copyable
    MessagePipeline(const MessagePipeline&) = delete;
    MessagePipeline& operator=(const MessagePipeline&) = delete;

    /**
     * @brief Start the entire pipeline
     * @details Starts outgoing adapter first (if present), then incoming adapter
     * @return true if all components started successfully
     * @post isRunning() returns true if successful
     * @thread_safe Yes - protected by mutex
     */
    [[nodiscard]] bool start() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        bool success = true;
        
        // Start outgoing first (so it's ready to receive)
        if (outgoingAdapter_) {
            success = outgoingAdapter_->start();
        }
        
        // Then start incoming
        if (success && incomingAdapter_) {
            success = incomingAdapter_->start();
        }
        
        return success;
    }

    /**
     * @brief Stop the entire pipeline gracefully
     * @details Stops incoming adapter first, then outgoing adapter
     *          (reverse order of data flow)
     * @post isRunning() returns false
     * @thread_safe Yes - protected by mutex
     */
    void stop() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Stop incoming first (stop receiving new data)
        if (incomingAdapter_) {
            incomingAdapter_->stop();
        }
        
        // Then stop outgoing
        if (outgoingAdapter_) {
            outgoingAdapter_->stop();
        }
    }

    /**
     * @brief Check if pipeline is running
     * @return true if all configured components are running
     * @thread_safe Yes - protected by mutex
     */
    [[nodiscard]] bool isRunning() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        
        bool running = true;
        if (incomingAdapter_) {
            running = incomingAdapter_->isRunning();
        }
        if (running && outgoingAdapter_) {
            running = outgoingAdapter_->isRunning();
        }
        return running;
    }

    /**
     * @brief Get pipeline name
     * @return Pipeline identifier
     */
    [[nodiscard]] const std::string& getName() const noexcept {
        return name_;
    }

    /**
     * @brief Set pipeline name
     * @param name New pipeline identifier
     */
    void setName(std::string name) {
        name_ = std::move(name);
    }

    /**
     * @brief Get incoming adapter
     * @return Shared pointer to incoming adapter (may be null)
     */
    [[nodiscard]] std::shared_ptr<IAdapter> getIncomingAdapter() const noexcept {
        return incomingAdapter_;
    }

    /**
     * @brief Set incoming adapter
     * @param adapter Incoming adapter to set
     */
    void setIncomingAdapter(std::shared_ptr<IAdapter> adapter) {
        incomingAdapter_ = std::move(adapter);
    }

    /**
     * @brief Get outgoing adapter
     * @return Shared pointer to outgoing adapter (may be null)
     */
    [[nodiscard]] std::shared_ptr<IAdapter> getOutgoingAdapter() const noexcept {
        return outgoingAdapter_;
    }

    /**
     * @brief Set outgoing adapter
     * @param adapter Outgoing adapter to set
     */
    void setOutgoingAdapter(std::shared_ptr<IAdapter> adapter) {
        outgoingAdapter_ = std::move(adapter);
    }

    /**
     * @brief Check if pipeline has incoming adapter
     * @return true if incoming adapter is configured
     */
    [[nodiscard]] bool hasIncomingAdapter() const noexcept {
        return incomingAdapter_ != nullptr;
    }

    /**
     * @brief Check if pipeline has outgoing adapter
     * @return true if outgoing adapter is configured
     */
    [[nodiscard]] bool hasOutgoingAdapter() const noexcept {
        return outgoingAdapter_ != nullptr;
    }

private:
    std::string name_;                            ///< Pipeline identifier
    std::shared_ptr<IAdapter> incomingAdapter_;   ///< Incoming adapter (receives messages)
    std::shared_ptr<IAdapter> outgoingAdapter_;   ///< Outgoing adapter (sends messages, optional)
    mutable std::mutex mutex_;                    ///< Thread safety mutex for lifecycle operations
};

} // namespace adapters
