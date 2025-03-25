cmake_minimum_required(VERSION 3.25.1)
project(PSXecute-BOF)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message(FATAL_ERROR "Only Linux is supported.")
endif()

# --- Debug vs. Release Build Flags ---
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    set(CMAKE_C_FLAGS "-m64 -g") # -g for debug symbols, remove optimizations
    set(CMAKE_CXX_FLAGS "-m64 -g -Wno-shift-count-overflow -fno-exceptions") # -g, remove optimizations, keep -fno-exceptions

else() # Default to Release if CMAKE_BUILD_TYPE is not set or unknown
    message(STATUS "Building in Release mode")
    set(CMAKE_C_FLAGS "-m64")
    set(CMAKE_CXX_FLAGS "-m64 -g -Wno-shift-count-overflow -Os -fno-exceptions -fomit-frame-pointer -s -flto")
endif()

add_library(PSXecute-BOF OBJECT
    main.c
    beacon.h
)
