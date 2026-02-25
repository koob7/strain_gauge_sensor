cmake_minimum_required(VERSION 3.22)
enable_language(C ASM CXX)


add_library(modules STATIC
    Src/device.cpp
    Src/interface.cpp
    Src/multiplekser.cpp
    Src/scheduler.cpp
    Src/serializer.cpp
    Src/usart_control.cpp
    Src/adc.cpp
    Src/adc_opamp.cpp
    Src/hx711.cpp
)

target_include_directories(modules PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(modules PUBLIC stm32cubemx)

target_link_libraries(${CMAKE_PROJECT_NAME} modules)