/**
 * @file IAdapter.hpp
 * @brief Base interface for all adapters in the hexagonal architecture
 * @details Defines the common contract for incoming and outgoing adapters
 *          enabling lifecycle management and polymorphic behavior.
 * 
 * @author b_hexagon Team
 * @version 1.0
 * @date 2025
 * 
 * @note MISRA C++ 2023 compliant implementation
 */

#pragma once

#include <string>

namespace adapters {

/**
 * @brief Base interface for all adapters (incoming and outgoing)
 * @details All adapters must implement this interface to be managed
 *          by the AdapterManager. Provides lifecycle control and identification.
 */
class IAdapter {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~IAdapter() = default;
    
    /**
     * @brief Start the adapter
     * @return true if started successfully, false otherwise
     */
    [[nodiscard]] virtual bool start() = 0;
    
    /**
     * @brief Stop the adapter gracefully
     * @details Should release all resources and stop any background threads
     */
    virtual void stop() = 0;
    
    /**
     * @brief Check if adapter is currently running
     * @return true if running, false otherwise
     */
    [[nodiscard]] virtual bool isRunning() const = 0;
    
    /**
     * @brief Get adapter name for logging and identification
     * @return Unique adapter identifier string
     */
    [[nodiscard]] virtual std::string getName() const noexcept = 0;
    
protected:
    /**
     * @brief Protected default constructor
     * @details Prevents direct instantiation of interface
     */
    IAdapter() = default;
    
    /**
     * @brief Protected copy constructor
     */
    IAdapter(const IAdapter&) = default;
    
    /**
     * @brief Protected move constructor
     */
    IAdapter(IAdapter&&) = default;
    
    /**
     * @brief Protected copy assignment
     */
    IAdapter& operator=(const IAdapter&) = default;
    
    /**
     * @brief Protected move assignment
     */
    IAdapter& operator=(IAdapter&&) = default;
};

} // namespace adapters
