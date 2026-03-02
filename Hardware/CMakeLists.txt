cmake_minimum_required(VERSION 3.22)
enable_language(C ASM CXX)



add_library(hardware STATIC
    Src/hx711_driver.cpp
)

target_include_directories(hardware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(hardware PUBLIC stm32cubemx)

target_link_libraries(${CMAKE_PROJECT_NAME} hardware)