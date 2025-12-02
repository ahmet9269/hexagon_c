# test_hexagon

Simple C++ TrackData publisher for testing a_hexagon application.

## Features

- Sends TrackData at 8Hz (125ms interval)
- Uses ZeroMQ RADIO socket with UDP multicast
- Simulates aircraft in circular flight path
- Binary serialization matching a_hexagon format (60 bytes)

## Build

```bash
make
```

## Run

```bash
# Send 100 messages (default)
./run.sh

# Send 500 messages
./run.sh 500

# Or directly
./bin/test_hexagon 500

# Infinite mode (Ctrl+C to stop)
./bin/test_hexagon 0
```

## Configuration

| Parameter | Value |
|-----------|-------|
| Endpoint | `udp://239.1.1.1:9000` |
| Group | `TrackData` |
| Track ID | 1001 |
| Frequency | 8Hz (125ms) |
| Message Size | 60 bytes |

## Message Format (TrackData)

```cpp
struct TrackData {
    int32_t trackId;           // 4 bytes
    double xVelocityECEF;      // 8 bytes
    double yVelocityECEF;      // 8 bytes
    double zVelocityECEF;      // 8 bytes
    double xPositionECEF;      // 8 bytes
    double yPositionECEF;      // 8 bytes
    double zPositionECEF;      // 8 bytes
    int64_t originalUpdateTime; // 8 bytes (milliseconds)
};  // Total: 60 bytes
```
