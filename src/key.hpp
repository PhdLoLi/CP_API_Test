/** 
 * Author: Lijing Wang
 * Date: 5/7/2015
 */
#pragma once
#include <ndn-cxx/contexts/producer-context.hpp>
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <ndn-cxx/security/validator.hpp>
#include <ndn-cxx/util/time.hpp>
#define IDENTITY_NAME "/Lijing/TEST"
#define CONTENT_LENGTH 1715*30
#define PREFIX_NAME "/Lijing/Time/Space"
#if LOG_LEVEL >= 6 
#define LOG_TRACE(...) printf(__VA_ARGS__); printf("\n")
#else
#define LOG_TRACE(...)
#endif

#if LOG_LEVEL >= 5 
#define LOG_DEBUG(...) printf(__VA_ARGS__);  printf("\n")
#else
#define LOG_DEBUG(...)
#endif

#if LOG_LEVEL >= 4 
#define LOG_INFO(...)  printf(__VA_ARGS__);  printf("\n")
#else
#define LOG_INFO(...)
#endif

