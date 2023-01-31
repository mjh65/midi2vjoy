# Top-level CMake file for Midi2vJoy (M2V)
#
# Midi2vJoy is a simple Windows utility written in C++ that converts
# messages from a MIDI controller into button presses from a vJoy
# virtual joystick.

cmake_minimum_required(VERSION 3.12)
project(midi2vjoy VERSION 1.0 DESCRIPTION "Windows utility to use MIDI controller to feed vJoy")

add_library(m2v_compiler_flags INTERFACE)
target_compile_features(m2v_compiler_flags INTERFACE cxx_std_17)
if(WIN32)
    target_link_libraries(m2v_compiler_flags INTERFACE wsock32 ws2_32)
endif()

add_subdirectory(lib)


add_executable(midi2vjoy)

target_include_directories(midi2vjoy PRIVATE
    ${vJoySDK_INC}
)

target_sources(midi2vjoy PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)
target_link_libraries(midi2vjoy PUBLIC
    m2v_compiler_flags
    rtmidi
    "${vJoySDK_LIB}"
)

add_custom_command(TARGET midi2vjoy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${vJoySDK_DLL}" $<TARGET_FILE_DIR:midi2vjoy>
)

