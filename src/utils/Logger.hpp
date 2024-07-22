#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <spdlog/spdlog.h>

#define VZ_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define VZ_LOG_WARNING(...) spdlog::warn(__VA_ARGS__)
#define VZ_LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__);std::exit(-1)
#define VZ_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define VZ_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)

#endif
