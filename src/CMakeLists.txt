project(LogWrapperDemo)

find_package(spdlog)

add_executable(${PROJECT_NAME} main.cpp
        LogWrapper.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)