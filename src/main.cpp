#include "LogWrapper.h"
#include <thread>

void module_A_work() {
    for (int i = 0; i < 10; ++i) {
        LOG_INFO_THROTTLED(std::chrono::seconds(2), "Module A is working hard... Iteration: {}", i);
        LOG_EX_INFO("module_A_debug", "Processing item {} in module A.", i);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}


void module_B_work() {
    for (int i = 0; i < 6; ++i) {
        LOG_WARN_THROTTLED(std::chrono::seconds(3), "Module B has a recurring warning. Iteration: {}", i);
        LOG_EX_TRACE("module_B_heavy_trace", "Detailed state for iteration {}: value=_some_complex_state_{}", i, i*1.23);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    LogConfig config;
    config.logLevel = spdlog::level::info;
    LogWrapper::instance().init(config);

    LOG_INFO("Main application started.");

    std::thread t1(module_A_work);
    std::thread t2(module_B_work);

    t1.join();
    t2.join();

    LOG_CRITICAL("Application finished. Check the main log file and the 'logs/extra/' directory for debug logs.");
    return 0;
}