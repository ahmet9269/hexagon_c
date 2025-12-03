/**
 * @file AdapterManager.hpp
 * @brief Thread-per-Type Adapter Manager for Hexagonal Architecture
 * @details Manages lifecycle of incoming/outgoing adapters with isolated threads
 *          Each message type has its own adapter and service instance
 * 
 * Architecture:
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │                    THREAD-PER-TYPE ARCHITECTURE                     │
 * ├─────────────────────────────────────────────────────────────────────┤
 * │                                                                     │
 * │  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐ │
 * │  │ MessageType A   │    │ MessageType B   │    │ MessageType C   │ │
 * │  ├─────────────────┤    ├─────────────────┤    ├─────────────────┤ │
 * │  │ InAdapter (T1)  │    │ InAdapter (T2)  │    │ InAdapter (T3)  │ │
 * │  │       ↓         │    │       ↓         │    │       ↓         │ │
 * │  │ DomainService   │    │ DomainService   │    │ DomainService   │ │
 * │  │       ↓         │    │       ↓         │    │       ↓         │ │
 * │  │ OutAdapter (T1) │    │ OutAdapter (T2) │    │ OutAdapter (T3) │ │
 * │  └─────────────────┘    └─────────────────┘    └─────────────────┘ │
 * │                                                                     │
 * │  Each vertical stack is COMPLETELY ISOLATED - zero contention!     │
 * └─────────────────────────────────────────────────────────────────────┘
 * 
 * @author c_hexagon Team
 * @version 1.1
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IAdapter.hpp
 * @see MessagePipeline.hpp
 */

#pragma once

#include "MessagePipeline.hpp"
#include <vector>
#include <atomic>
#include <algorithm>
#include <mutex>
#include <shared_mutex>

namespace adapters {

/**
 * @class AdapterManager
 * @brief Manages all message pipelines with thread-per-type isolation
 * @details Provides centralized lifecycle management for all adapters.
 *          Each registered pipeline operates independently.
 * 
 * Usage Example:
 * @code
 * AdapterManager manager;
 * 
 * MessagePipeline trackPipeline("TrackData", trackInAdapter, trackOutAdapter);
 * manager.registerPipeline(std::move(trackPipeline));
 * 
 * if (manager.startAll()) {
 *     // All pipelines running
 * }
 * 
 * manager.stopAll();
 * @endcode
 * 
 * @invariant pipelines_ contains only valid MessagePipeline objects
 * @invariant running_ reflects actual state of pipelines
 */
class AdapterManager {
public:
    /**
     * @brief Default constructor
     * @post isRunning() returns false
     * @post getPipelineCount() returns 0
     */
    AdapterManager() : running_(false) {}

    /**
     * @brief Destructor - stops all pipelines
     * @details Ensures graceful shutdown of all registered pipelines
     */
    ~AdapterManager() {
        stopAll();
    }

    // Delete copy/move to prevent accidental duplication
    AdapterManager(const AdapterManager&) = delete;
    AdapterManager& operator=(const AdapterManager&) = delete;
    AdapterManager(AdapterManager&&) = delete;
    AdapterManager& operator=(AdapterManager&&) = delete;

    /**
     * @brief Register a new message pipeline
     * @param pipeline Pipeline to register (moved)
     * @pre Manager should not be running when registering
     * @post getPipelineCount() increases by 1
     * @note Thread-safe via mutex protection
     */
    void registerPipeline(MessagePipeline pipeline) {
        std::unique_lock<std::shared_mutex> lock(pipelines_mutex_);
        pipelines_.push_back(std::move(pipeline));
    }

    /**
     * @brief Start all registered pipelines
     * @return true if all started successfully, false if any failed
     * @post isRunning() returns true if successful
     * @note If any pipeline fails to start, previously started pipelines
     *       remain running. Call stopAll() for cleanup.
     * @note Thread-safe via mutex protection
     */
    [[nodiscard]] bool startAll() {
        std::unique_lock<std::shared_mutex> lock(pipelines_mutex_);
        bool success = true;
        for (auto& pipeline : pipelines_) {
            if (!pipeline.start()) {
                success = false;
                // Continue starting others for partial availability
            }
        }
        running_ = success;
        return success;
    }

    /**
     * @brief Stop all pipelines gracefully
     * @post isRunning() returns false
     * @post All pipeline.isRunning() return false
     * @note Thread-safe via mutex protection
     */
    void stopAll() noexcept {
        std::unique_lock<std::shared_mutex> lock(pipelines_mutex_);
        running_ = false;
        for (auto& pipeline : pipelines_) {
            pipeline.stop();
        }
    }

    /**
     * @brief Get number of registered pipelines
     * @return Number of pipelines
     * @note Thread-safe via shared_mutex (read lock)
     */
    [[nodiscard]] std::size_t getPipelineCount() const noexcept {
        std::shared_lock<std::shared_mutex> lock(pipelines_mutex_);
        return pipelines_.size();
    }

    /**
     * @brief Check if manager is running
     * @return true if running
     */
    [[nodiscard]] bool isRunning() const noexcept {
        return running_.load();
    }

    /**
     * @brief Get pipeline by name
     * @param name Pipeline name to search for
     * @return Pointer to pipeline if found, nullptr otherwise
     * @note Thread-safe via shared_mutex (read lock)
     */
    [[nodiscard]] MessagePipeline* getPipeline(const std::string& name) noexcept {
        std::shared_lock<std::shared_mutex> lock(pipelines_mutex_);
        auto it = std::find_if(pipelines_.begin(), pipelines_.end(),
            [&name](const MessagePipeline& p) { return p.getName() == name; });
        return (it != pipelines_.end()) ? &(*it) : nullptr;
    }

    /**
     * @brief Get pipeline by name (const version)
     * @param name Pipeline name to search for
     * @return Const pointer to pipeline if found, nullptr otherwise
     * @note Thread-safe via shared_mutex (read lock)
     */
    [[nodiscard]] const MessagePipeline* getPipeline(const std::string& name) const noexcept {
        std::shared_lock<std::shared_mutex> lock(pipelines_mutex_);
        auto it = std::find_if(pipelines_.begin(), pipelines_.end(),
            [&name](const MessagePipeline& p) { return p.getName() == name; });
        return (it != pipelines_.end()) ? &(*it) : nullptr;
    }

private:
    std::vector<MessagePipeline> pipelines_;    ///< Registered pipelines
    std::atomic<bool> running_;                 ///< Manager running state
    mutable std::shared_mutex pipelines_mutex_; ///< Mutex for thread-safe access to pipelines
};

} // namespace adapters
