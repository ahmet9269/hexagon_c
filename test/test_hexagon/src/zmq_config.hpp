/**
 * @file zmq_config.hpp
 * @brief ZeroMQ configuration for Draft API support
 * @details Must be included BEFORE zmq.hpp to enable RADIO/DISH sockets
 */

#ifndef ZMQ_CONFIG_HPP
#define ZMQ_CONFIG_HPP

// Enable ZeroMQ Draft API (RADIO/DISH sockets)
#ifndef ZMQ_BUILD_DRAFT_API
#define ZMQ_BUILD_DRAFT_API 1
#endif

// ZMQ socket type constants (if not defined)
#ifndef ZMQ_RADIO
#define ZMQ_RADIO 14
#endif

#ifndef ZMQ_DISH
#define ZMQ_DISH 15
#endif

#endif // ZMQ_CONFIG_HPP
