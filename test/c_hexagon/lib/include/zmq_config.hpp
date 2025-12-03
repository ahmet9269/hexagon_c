/**
 * @file zmq_config.hpp
 * @brief ZeroMQ configuration for Draft API support
 * @details This header must be included before zmq.hpp to enable RADIO/DISH sockets
 * 
 * @note Include this file before any ZeroMQ headers
 */

#ifndef ZMQ_BUILD_DRAFT_API
#define ZMQ_BUILD_DRAFT_API 1
#endif
