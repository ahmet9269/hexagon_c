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
 * @author b_hexagon Team
 * @version 1.1 - shared_mutex for improved read performance
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 * @see IAdapter.hpp
 * @see MessagePipeline.hpp
 */

#pragma once

#include "MessagePipeline.hpp"
#include "../../utils/Logger.hpp"
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
 *          Each registered pipeline operates independently with zero contention.
 *          Supports 20+ simultaneous incoming/outgoing adapters.
 * 
 * Usage Example:
 * @code
 * AdapterManager manager;
 * 
 * // Register multiple pipelines
 * for (int i = 0; i < 20; i++) {
 *     auto inAdapter = std::make_shared<IncomingAdapter>(service);
 *     auto outAdapter = std::make_shared<OutgoingAdapter>();
 *     MessagePipeline pipeline("Type_" + std::to_string(i), inAdapter, outAdapter);
 *     manager.registerPipeline(std::move(pipeline));
 * }
 * 
 * if (manager.startAll()) {
 *     // All 20 pipelines running in isolated threads
 * }
 * 
 * manager.stopAll();  // Graceful shutdown of all pipelines
 * @endcode
 * 
 * @invariant pipelines_ contains only valid MessagePipeline objects
 * @invariant running_ reflects actual state of pipelines
 * @thread_safe All public methods are thread-safe
 */
class AdapterManager {
public:
    /**
     * @brief Default constructor
     * @post isRunning() returns false
     * @post getPipelineCount() returns 0
     */
    AdapterManager() : running_{false} {}

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
     * @thread_safe Yes (uses unique_lock for write)
     */
    void registerPipeline(MessagePipeline pipeline) {
        std::unique_lock<std::shared_mutex> lock(pipelines_mutex_);
        Logger::info("Registering pipeline: ", pipeline.getName());
        pipelines_.push_back(std::move(pipeline));
    }

    /**
     * @brief Start all registered pipelines
     * @return true if all started successfully, false if any failed
     * @post isRunning() returns true if successful
     * @note If any pipeline fails to start, previously started pipelines
     *       remain running. Call stopAll() for cleanup.
     * @thread_safe Yes (uses unique_lock for write)
     */
    [[nodiscard]] bool startAll() {
        std::unique_lock<std::shared_mutex> lock(pipelines_mutex_);
        
        bool success = true;
        size_t started = 0;
        
        Logger::info("Starting ", pipelines_.size(), " pipeline(s)...");
        
        for (auto& pipeline : pipelines_) {
            Logger::debug("Starting pipeline: ", pipeline.getName());
            if (pipeline.start()) {
                started++;
                Logger::info("Pipeline started: ", pipeline.getName());
            } else {
                Logger::error("Failed to start pipeline: ", pipeline.getName());
                success = false;
            }
        }
        
        running_.store(success);
        Logger::info("Started ", started, "/", pipelines_.size(), " pipelines");
        
        return success;
    }

    /**
     * @brief Stop all pipelines gracefully
     * @post isRunning() returns false
     * @post All pipeline.isRunning() return false
     * @thread_safe Yes (uses unique_lock for write)
     */
    void stopAll() noexcept {
        std::unique_lock<std::shared_mutex> lock(pipelines_mutex_);
        
        Logger::info("Stopping all ", pipelines_.size(), " pipeline(s)...");
        running_.store(false);
        
        for (auto& pipeline : pipelines_) {
            Logger::debug("Stopping pipeline: ", pipeline.getName());
            pipeline.stop();
        }
        
        Logger::info("All pipelines stopped");
    }

    /**
     * @brief Get number of registered pipelines
     * @return Number of pipelines
     * @thread_safe Yes (uses shared_lock for read - concurrent reads allowed)
     */
    [[nodiscard]] std::size_t getPipelineCount() const noexcept {
        std::shared_lock<std::shared_mutex> lock(pipelines_mutex_);
        return pipelines_.size();
    }

    /**
     * @brief Check if manager is running
     * @return true if running
     * @thread_safe Yes (atomic)
     */
    [[nodiscard]] bool isRunning() const noexcept {
        return running_.load();
    }

    /**
     * @brief Get pipeline by name
     * @param name Pipeline name to search for
     * @return Pointer to pipeline if found, nullptr otherwise
     * @thread_safe Yes (uses shared_lock for read - concurrent reads allowed)
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
     * @thread_safe Yes (uses shared_lock for read - concurrent reads allowed)
     */
    [[nodiscard]] const MessagePipeline* getPipeline(const std::string& name) const noexcept {
        std::shared_lock<std::shared_mutex> lock(pipelines_mutex_);
        auto it = std::find_if(pipelines_.begin(), pipelines_.end(),
            [&name](const MessagePipeline& p) { return p.getName() == name; });
        return (it != pipelines_.end()) ? &(*it) : nullptr;
    }

    /**
     * @brief Get all pipeline names
     * @return Vector of pipeline names
     * @thread_safe Yes (uses shared_lock for read - concurrent reads allowed)
     */
    [[nodiscard]] std::vector<std::string> getPipelineNames() const {
        std::shared_lock<std::shared_mutex> lock(pipelines_mutex_);
        std::vector<std::string> names;
        names.reserve(pipelines_.size());
        for (const auto& pipeline : pipelines_) {
            names.push_back(pipeline.getName());
        }
        return names;
    }

private:
    std::vector<MessagePipeline> pipelines_;        ///< Registered pipelines
    std::atomic<bool> running_;                     ///< Manager running state
    mutable std::shared_mutex pipelines_mutex_;     ///< Reader-writer mutex for thread-safe access
};

} // namespace adapters
