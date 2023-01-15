#pragma once
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#define SPD_LOG_LEVEL(log_level, ...)                                          \
  spdlog::log_level(fmt::format("({}:{}.{}) ", __FILE__, __LINE__, __func__) + \
                    fmt::format(__VA_ARGS__))
#define LAND_INFO(...) SPD_LOG_LEVEL(info, __VA_ARGS__)
#define LAND_WARN(...) SPD_LOG_LEVEL(warn, __VA_ARGS__)
#define LAND_DEBUG(...) SPD_LOG_LEVEL(debug, __VA_ARGS__)
#define LAND_TRACE(...) SPD_LOG_LEVEL(trace, __VA_ARGS__)

#define LAND_ERROR(...)                                 \
  {                                                     \
    SPD_LOG_LEVEL(error, __VA_ARGS__);                  \
    throw std::runtime_error(fmt::format(__VA_ARGS__)); \
  }

namespace grassland::util {
void assert_handler(bool result, const char *code);
}

#define LAND_ASSERT(x) ::grassland::util::assert_handler(x, #x)
