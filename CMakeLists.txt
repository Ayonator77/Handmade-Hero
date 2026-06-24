cmake_minimum_required(VERSION 3.20)
project(handmade_win32 LANGUAGES CXX)

# Your source file
add_executable(win32_handmade_msvc WIN32
    code/win32_handmade.cpp
)

# Match your CL flags (/Zi /W4 /EHsc)
target_compile_features(win32_handmade_msvc PRIVATE cxx_std_17)
target_compile_options(win32_handmade_msvc PRIVATE /Zi /W4 /EHsc)
target_link_libraries(win32_handmade_msvc PRIVATE user32 gdi32)

# Put the .exe in ./build (like your current layout)
set_target_properties(win32_handmade_msvc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/build"
)
