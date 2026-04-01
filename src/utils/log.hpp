#pragma once
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Log.hpp>

// lazy
#define LOG_ENABLE_CHECK geode::Mod::get()->getSettingValue<bool>("enable-log")

namespace ninja::log {

template <typename... Args>
inline void debug(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (LOG_ENABLE_CHECK) geode::log::debug(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (LOG_ENABLE_CHECK) geode::log::info(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (LOG_ENABLE_CHECK) geode::log::warn(str, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(geode::format::FmtStr<Args...> str, Args&&... args) {
    if (LOG_ENABLE_CHECK) geode::log::error(str, std::forward<Args>(args)...);
}

}
