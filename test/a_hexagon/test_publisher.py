#!/usr/bin/env python3
"""
Test Publisher for a_hexagon
Sends simulated TrackData to a_hexagon's incoming adapter (SUB socket)

Usage:
    python3 test_publisher.py [--count N] [--interval MS]

Options:
    --count N       Number of messages to send (default: 10, 0=infinite)
    --interval MS   Interval between messages in milliseconds (default: 125 = 8Hz)
"""

import zmq
import struct
import time
import argparse
import math

# TrackData binary format (matches C++ serialization):
# - trackId: int32_t (4 bytes)
# - xVelocityECEF: double (8 bytes)
# - yVelocityECEF: double (8 bytes)
# - zVelocityECEF: double (8 bytes)
# - xPositionECEF: double (8 bytes)
# - yPositionECEF: double (8 bytes)
# - zPositionECEF: double (8 bytes)
# - originalUpdateTime: int64_t (8 bytes)
# Total: 60 bytes

TRACK_DATA_FORMAT = '<i d d d d d d q'  # little-endian
TRACK_DATA_SIZE = struct.calcsize(TRACK_DATA_FORMAT)

def create_track_data(track_id: int, time_offset: float) -> bytes:
    """
    Create a simulated TrackData binary message.
    
    Simulates an aircraft flying in a circular path.
    """
    # Simulate circular motion
    angular_velocity = 0.1  # rad/s
    radius = 10000.0  # meters
    altitude = 5000.0  # meters
    
    angle = angular_velocity * time_offset
    
    # Position (ECEF-like coordinates)
    x_pos = radius * math.cos(angle)
    y_pos = radius * math.sin(angle)
    z_pos = altitude
    
    # Velocity (tangential to circle)
    speed = angular_velocity * radius  # m/s
    x_vel = -speed * math.sin(angle)
    y_vel = speed * math.cos(angle)
    z_vel = 0.0
    
    # Timestamp (milliseconds since epoch - TrackDataExtrapolator expects ms)
    timestamp = int(time.time() * 1_000)
    
    return struct.pack(
        TRACK_DATA_FORMAT,
        track_id,
        x_vel, y_vel, z_vel,
        x_pos, y_pos, z_pos,
        timestamp
    )

def main():
    parser = argparse.ArgumentParser(description='Test Publisher for a_hexagon')
    parser.add_argument('--count', type=int, default=10, 
                        help='Number of messages (0=infinite)')
    parser.add_argument('--interval', type=int, default=125, 
                        help='Interval in ms (125ms = 8Hz)')
    parser.add_argument('--endpoint', type=str, default='tcp://127.0.0.1:15000',
                        help='ZeroMQ endpoint')
    parser.add_argument('--track-id', type=int, default=1001,
                        help='Track ID to use')
    args = parser.parse_args()
    
    print(f"=== a_hexagon Test Publisher ===")
    print(f"Endpoint: {args.endpoint}")
    print(f"Track ID: {args.track_id}")
    print(f"Interval: {args.interval}ms ({1000/args.interval:.1f}Hz)")
    print(f"Count: {args.count if args.count > 0 else 'infinite'}")
    print(f"Message size: {TRACK_DATA_SIZE} bytes")
    print("=" * 35)
    
    # Create ZeroMQ PUB socket (for testing - production uses RADIO/DISH)
    context = zmq.Context()
    socket = context.socket(zmq.PUB)
    
    # Bind to endpoint (a_hexagon will connect)
    socket.bind(args.endpoint)
    print(f"PUB socket bound to {args.endpoint}")
    
    # Wait for subscribers to connect
    print("Waiting 2 seconds for subscribers to connect...")
    time.sleep(2)
    
    start_time = time.time()
    count = 0
    
    try:
        while args.count == 0 or count < args.count:
            elapsed = time.time() - start_time
            
            # Create TrackData message
            data = create_track_data(args.track_id, elapsed)
            
            # Send message (PUB/SUB for testing)
            socket.send(data)
            count += 1
            
            # Print progress
            if count % 8 == 0 or count == 1:  # Print every second (at 8Hz)
                print(f"[{elapsed:.1f}s] Sent message #{count} for track {args.track_id}")
            
            # Wait for next interval
            time.sleep(args.interval / 1000.0)
            
    except KeyboardInterrupt:
        print(f"\nInterrupted after {count} messages")
    
    print(f"\n=== Summary ===")
    print(f"Total messages sent: {count}")
    print(f"Duration: {time.time() - start_time:.2f}s")
    
    socket.close()
    context.term()

if __name__ == '__main__':
    main()
