# LogWrapper for spdlog

`LogWrapper` is a powerful, yet easy-to-use C++ logging module built on top of the acclaimed [`spdlog`](https://github.com/gabime/spdlog) library. It provides a singleton-based, macro-driven interface designed to simplify logging in complex C++ applications while offering advanced features for high-performance and debugging scenarios.

## Features

- **Singleton Access**: A globally accessible logging instance, removing the need to pass logger objects throughout your codebase.
- **Multi-Level Logging**: Standard logging levels (`TRACE`, `DEBUG`, `INFO`, `WARN`, `ERROR`, `CRITICAL`) available through simple macros.
- **Asynchronous Performance**: Configurable async logging to minimize I/O impact on your application's critical path.
- **Log Rotation**: Automatic log file rotation based on size to manage disk space.
- **Per-Call Throttling**: Prevent log spam in high-frequency loops by logging a message at most once per a user-defined interval, right from the macro call.
- **On-Demand Debug Logs**: Instantly create separate, temporary log files for specific debugging tasks with a dedicated macro, without touching the main log configuration.
- **Thread-Safe**: Designed for safe use across multiple threads.

## Prerequisites

- A C++17 compliant compiler (e.g., GCC, Clang, MSVC).
- [`spdlog`](https://github.com/gabime/spdlog) library (header-only is sufficient).

## Quick Start

### 1. Initialization

First, initialize the logger at the beginning of your `main()` function. You can use the default configuration or customize it.

```cpp
#include "LogWrapper.h"

int main() {
    // Use default settings
    LogWrapper::instance().init();

    // Or, provide a custom configuration
    LogConfig config;
    config.loggerName = "MyAwesomeApp";
    config.filePath = "logs/my_awesome_app.log";
    config.logLevel = spdlog::level::debug;
    config.useAsync = true;
    
    LogWrapper::instance().init(config);

    // ... your application code
    return 0;
}
```

### 2. Basic Logging

Use the simple macros anywhere in your code to write to the main log.

```cpp
#include "LogWrapper.h"

void process_data(int user_id) {
    LOG_INFO("Starting data processing for user {}", user_id);

    if (user_id < 0) {
        LOG_WARN("Invalid user_id provided: {}", user_id);
        return;
    }
    
    // ... processing logic ...

    LOG_DEBUG("Successfully processed data for user {}", user_id);
}
```

### 3. Throttled Logging

To log a message from a hot loop without flooding your logs, use the `_THROTTLED` macros. The first argument is the time interval.

```cpp
#include "LogWrapper.h"
#include <chrono>
#include <thread>

void game_update_loop() {
    for (int i = 0; i < 1000; ++i) {
        // This message will only be logged at most once every 5 seconds from this line.
        LOG_INFO_THROTTLED(std::chrono::seconds(5), "Game loop is running... Frame: {}", i);

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Simulate 60 FPS
    }
}
```

### 4. Extra "On-Demand" Debug Logging

When you need to debug a specific feature without cluttering the main log, use the `LOG_EX_` macros. The first argument is a unique `key` that determines the output file. Logs will automatically be written to `logs/extra/<key>.log`.

```cpp
#include "LogWrapper.h"

void complex_algorithm(int input) {
    // This log goes to 'logs/extra/algorithm_trace.log'
    LOG_EX_DEBUG("algorithm_trace", "Starting complex algorithm with input: {}", input);

    // ... calculations ...
    int intermediate_result = input * 42;
    // This also goes to 'logs/extra/algorithm_trace.log'
    LOG_EX_INFO("algorithm_trace", "Intermediate result calculated: {}", intermediate_result);
    
    if (intermediate_result > 9000) {
        // You can use a different key to log to a different file simultaneously
        LOG_EX_WARN("high_value_alerts", "High intermediate value detected: {}", intermediate_result);
    }
    // ... more calculations ...
}
```

This feature is perfect for temporary, targeted debugging. The loggers are created on-demand and managed automatically.