#include "LogWrapper.h"
#include <vector>

LogWrapper &LogWrapper::instance()
{
    static LogWrapper instance;
    return instance;
}

void LogWrapper::init(const LogConfig &config)
{
    if (m_is_initialized)
    {
        return;
    }

    try
    {
        if (config.useAsync)
        {
            spdlog::init_thread_pool(8192, 1);
        }

        const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(config.logLevel);

        const auto rotating_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            config.filePath, config.maxFileSize, config.maxFiles);
        rotating_file_sink->set_level(config.logLevel);

        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(console_sink);
        sinks.push_back(rotating_file_sink);

        if (config.useAsync)
        {
            m_logger = std::make_shared<spdlog::async_logger>(
                config.loggerName, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        }
        else
        {
            m_logger = std::make_shared<spdlog::logger>(config.loggerName, sinks.begin(), sinks.end());
        }

        m_logger->set_level(config.logLevel);
        m_logger->flush_on(config.logLevel);
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] [%t] [%s:%#] : %v");

        spdlog::set_default_logger(m_logger);
        m_is_initialized = true;
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        fprintf(stderr, "Log initialization failed: %s\n", ex.what());
        exit(1);
    }
}

std::shared_ptr<spdlog::logger> LogWrapper::get_logger() const
{
    if (!m_is_initialized)
    {
        return nullptr;
    }
    return m_logger;
}