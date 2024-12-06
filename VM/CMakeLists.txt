cmake_minimum_required(VERSION 3.25.1)
project(PSXecute)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message(FATAL_ERROR "Only Linux is supported.")
endif()

set (CMAKE_C_FLAGS "-m32")
set (CMAKE_CXX_FLAGS "-m32 -Wno-shift-count-overflow -Os -fno-exceptions -fomit-frame-pointer -s -flto")
set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")


add_executable(PSXecute
    main.cpp
    cpu/Psx.cpp
    cpu/Psx.h
    cpu/Instruction.cpp
    cpu/Instruction.h
    cpu/Opcodes.cpp
)
