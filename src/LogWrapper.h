#pragma once
#ifndef LOG_WRAPPER_H
#define LOG_WRAPPER_H
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <chrono>
#include <memory>
#include <mutex>
#include <unordered_map>

struct LogConfig
{
    std::string loggerName = "DefaultLogger";
    std::string filePath = "logs/rlog.log";
    spdlog::level::level_enum logLevel = spdlog::level::trace;
    size_t maxFileSize = 1024 * 1024 * 50; // 50MB
    size_t maxFiles = 5;
    bool useAsync = true;
};

class LogWrapper
{
public:
    static LogWrapper &instance();

    LogWrapper(const LogWrapper &) = delete;
    LogWrapper &operator=(const LogWrapper &) = delete;

    void init(const LogConfig &config = LogConfig());

    std::shared_ptr<spdlog::logger> get_logger() const;

    template <typename Duration, typename... Args>
    void log_throttled(const Duration &interval, spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args);

    template <typename... Args>
    void log_ex(const std::string &key, spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args);
    void shutdown()
    {
        if (m_is_initialized)
        {
            spdlog::shutdown();
            m_is_initialized = false;
        }
    }

private:
    LogWrapper() = default;
    ~LogWrapper() { spdlog::shutdown(); }

    std::shared_ptr<spdlog::logger> m_logger;
    bool m_is_initialized = false;

    std::mutex m_throttle_mutex;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_throttle_map;

    std::mutex m_ex_loggers_mutex;
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_ex_loggers;
};

inline spdlog::logger *get_console_only_logger()
{
    static auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    static auto logger = std::make_shared<spdlog::logger>("console_only", console_sink);
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::trace);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] [%t] [%s:%#] : %v");
    return logger.get();
}

#define LOG_CONSOLE_TRACE(...) SPDLOG_LOGGER_CALL(get_console_only_logger(), spdlog::level::trace, __VA_ARGS__)
#define LOG_CONSOLE_DEBUG(...) SPDLOG_LOGGER_CALL(get_console_only_logger(), spdlog::level::debug, __VA_ARGS__)
#define LOG_CONSOLE_INFO(...) SPDLOG_LOGGER_CALL(get_console_only_logger(), spdlog::level::info, __VA_ARGS__)
#define LOG_CONSOLE_WARN(...) SPDLOG_LOGGER_CALL(get_console_only_logger(), spdlog::level::warn, __VA_ARGS__)
#define LOG_CONSOLE_ERROR(...) SPDLOG_LOGGER_CALL(get_console_only_logger(), spdlog::level::err, __VA_ARGS__)
#define LOG_CONSOLE_CRITICAL(...) SPDLOG_LOGGER_CALL(get_console_only_logger(), spdlog::level::critical, __VA_ARGS__)

#define LOG_TRACE(...) SPDLOG_LOGGER_CALL(LogWrapper::instance().get_logger(), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_CALL(LogWrapper::instance().get_logger(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_CALL(LogWrapper::instance().get_logger(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_CALL(LogWrapper::instance().get_logger(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(LogWrapper::instance().get_logger(), spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(LogWrapper::instance().get_logger(), spdlog::level::critical, __VA_ARGS__)

#define LOG_TRACE_THROTTLED(interval, ...) LogWrapper::instance().log_throttled(interval, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG_THROTTLED(interval, ...) LogWrapper::instance().log_throttled(interval, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO_THROTTLED(interval, ...) LogWrapper::instance().log_throttled(interval, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN_THROTTLED(interval, ...) LogWrapper::instance().log_throttled(interval, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR_THROTTLED(interval, ...) LogWrapper::instance().log_throttled(interval, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL_THROTTLED(interval, ...) LogWrapper::instance().log_throttled(interval, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::critical, __VA_ARGS__)

#define LOG_EX_TRACE(key, ...) LogWrapper::instance().log_ex(key, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, __VA_ARGS__)
#define LOG_EX_DEBUG(key, ...) LogWrapper::instance().log_ex(key, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::debug, __VA_ARGS__)
#define LOG_EX_INFO(key, ...) LogWrapper::instance().log_ex(key, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, __VA_ARGS__)
#define LOG_EX_WARN(key, ...) LogWrapper::instance().log_ex(key, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, __VA_ARGS__)
#define LOG_EX_ERROR(key, ...) LogWrapper::instance().log_ex(key, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, __VA_ARGS__)
#define LOG_EX_CRITICAL(key, ...) LogWrapper::instance().log_ex(key, spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::critical, __VA_ARGS__)

template <typename Duration, typename... Args>
void LogWrapper::log_throttled(const Duration &interval, spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    const std::string key = std::string(loc.filename) + ":" + std::to_string(loc.line);
    const auto now = std::chrono::steady_clock::now();

    std::lock_guard<std::mutex> lock(m_throttle_mutex);
    const auto it = m_throttle_map.find(key);

    if (it == m_throttle_map.end() || (now - it->second) >= interval)
    {
        m_throttle_map[key] = now;
        if (m_logger)
        {
            m_logger->log(loc, lvl, fmt, std::forward<Args>(args)...);
        }
    }
}

template <typename... Args>
void LogWrapper::log_ex(const std::string &key, spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args)
{
    std::shared_ptr<spdlog::logger> ex_logger;

    {
        std::lock_guard<std::mutex> lock(m_ex_loggers_mutex);
        const auto it = m_ex_loggers.find(key);
        if (it != m_ex_loggers.end())
        {
            ex_logger = it->second;
        }
    }

    if (!ex_logger)
    {
        std::lock_guard<std::mutex> lock(m_ex_loggers_mutex);
        const auto it = m_ex_loggers.find(key);
        if (it != m_ex_loggers.end())
        {
            ex_logger = it->second;
        }
        else
        {
            try
            {
                auto now = std::chrono::system_clock::now();
                std::time_t time = std::chrono::system_clock::to_time_t(now);
                std::tm tm = *std::localtime(&time);
                const std::string file_date = spdlog::fmt_lib::format("_{:04d}-{:02d}-{:02d}_{:02d}_{:02d}_{:02d}",
                                                                      tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                std::string file_path = "logs/extra/" + key + file_date + ".log";
                auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path, true);

                ex_logger = std::make_shared<spdlog::logger>(key, file_sink);
                ex_logger->set_level(spdlog::level::trace);
                ex_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] [%t] [%s:%#] : %v");

                m_ex_loggers[key] = ex_logger;
            }
            catch (const spdlog::spdlog_ex &ex)
            {
                LOG_ERROR("Failed to create extra logger for key '{}': {}", key, ex.what());
                return;
            }
        }
    }

    if (ex_logger)
    {
        ex_logger->log(loc, lvl, fmt, std::forward<Args>(args)...);
    }
}

#endif // LOG_WRAPPER_H