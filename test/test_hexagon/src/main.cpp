/**
 * @file main.cpp
 * @brief Simple TrackData publisher for a_hexagon testing
 * @details Sends TrackData at 8Hz using ZeroMQ RADIO socket (UDP multicast)
 * 
 * Usage: ./test_hexagon [count]
 *   count: Number of messages to send (default: 100, 0=infinite)
 */

// MUST be included BEFORE zmq.hpp to enable Draft API
#include "zmq_config.hpp"
#include <zmq.hpp>

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdint>

// Global flag for graceful shutdown
static volatile bool g_running = true;

void signalHandler(int) {
    g_running = false;
}

/**
 * @brief TrackData structure matching a_hexagon's expected format
 * Total size: 60 bytes
 */
#pragma pack(push, 1)
struct TrackData {
    int32_t trackId;           // 4 bytes
    double xVelocityECEF;      // 8 bytes
    double yVelocityECEF;      // 8 bytes
    double zVelocityECEF;      // 8 bytes
    double xPositionECEF;      // 8 bytes
    double yPositionECEF;      // 8 bytes
    double zPositionECEF;      // 8 bytes
    int64_t originalUpdateTime; // 8 bytes
};
#pragma pack(pop)

static_assert(sizeof(TrackData) == 60, "TrackData must be 60 bytes");

/**
 * @brief Create simulated track data (aircraft in circular path)
 */
TrackData createTrackData(int32_t trackId, double timeOffset) {
    TrackData data{};
    
    // Circular motion parameters
    constexpr double angularVelocity = 0.1;  // rad/s
    constexpr double radius = 10000.0;        // meters
    constexpr double altitude = 5000.0;       // meters
    
    double angle = angularVelocity * timeOffset;
    double speed = angularVelocity * radius;
    
    data.trackId = trackId;
    
    // Position (circular path)
    data.xPositionECEF = radius * std::cos(angle);
    data.yPositionECEF = radius * std::sin(angle);
    data.zPositionECEF = altitude;
    
    // Velocity (tangential)
    data.xVelocityECEF = -speed * std::sin(angle);
    data.yVelocityECEF = speed * std::cos(angle);
    data.zVelocityECEF = 0.0;
    
    // Timestamp in milliseconds
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    data.originalUpdateTime = ms;
    
    return data;
}

int main(int argc, char* argv[]) {
    // Parse arguments
    int messageCount = 100;
    if (argc > 1) {
        messageCount = std::atoi(argv[1]);
    }
    
    // Configuration - UDP multicast to match a_hexagon's DISH socket
    const char* endpoint = "udp://239.1.1.1:9000";
    const char* group = "TrackData";
    constexpr int32_t trackId = 1001;
    constexpr int intervalMs = 125;  // 8Hz = 125ms
    
    std::cout << "=== test_hexagon - TrackData Publisher ===" << std::endl;
    std::cout << "Endpoint: " << endpoint << std::endl;
    std::cout << "Group: " << group << std::endl;
    std::cout << "Track ID: " << trackId << std::endl;
    std::cout << "Interval: " << intervalMs << "ms (8Hz)" << std::endl;
    std::cout << "Count: " << (messageCount > 0 ? std::to_string(messageCount) : "infinite") << std::endl;
    std::cout << "Message size: " << sizeof(TrackData) << " bytes" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    // Signal handler
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    try {
        // Create ZeroMQ context and RADIO socket (UDP multicast)
        zmq::context_t context(1);
        zmq::socket_t socket(context, ZMQ_RADIO);
        
        // Connect to multicast endpoint
        socket.connect(endpoint);
        std::cout << "RADIO socket connected to " << endpoint << std::endl;
        
        // Small delay for connection setup
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        auto startTime = std::chrono::steady_clock::now();
        int count = 0;
        
        std::cout << "\nSending TrackData..." << std::endl;
        
        while (g_running && (messageCount == 0 || count < messageCount)) {
            // Calculate elapsed time
            auto now = std::chrono::steady_clock::now();
            double elapsed = std::chrono::duration<double>(now - startTime).count();
            
            // Create track data
            TrackData data = createTrackData(trackId, elapsed);
            
            // Create ZeroMQ message with group
            zmq::message_t message(sizeof(TrackData));
            std::memcpy(message.data(), &data, sizeof(TrackData));
            message.set_group(group);
            
            // Send
            socket.send(message, zmq::send_flags::dontwait);
            count++;
            
            // Progress output (every second at 8Hz)
            if (count % 8 == 0 || count == 1) {
                std::cout << "[" << std::fixed << std::setprecision(1) << elapsed << "s] "
                          << "Sent #" << count 
                          << " | TrackID: " << data.trackId
                          << " | Pos: (" << static_cast<int>(data.xPositionECEF) 
                          << ", " << static_cast<int>(data.yPositionECEF) << ")"
                          << std::endl;
            }
            
            // Wait for next interval (8Hz)
            std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        }
        
        // Summary
        auto endTime = std::chrono::steady_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n=== Summary ===" << std::endl;
        std::cout << "Messages sent: " << count << std::endl;
        std::cout << "Duration: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "Rate: " << std::setprecision(1) << (count / totalTime) << " msg/s" << std::endl;
        
        socket.close();
        context.close();
        
    } catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQ error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
