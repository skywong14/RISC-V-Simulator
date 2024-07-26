cmake_minimum_required(VERSION 3.22)
project(RISC_V_Simulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(RISC_V_Simulator
        src/Module/main.cpp
        src/Common/Register.hpp
        src/Common/Memory.hpp
        src/Common/Decoder.hpp
        src/Module/CPU.hpp
        src/Common/Memory.cpp
        src/Module/ALU.hpp
        src/Module/ReorderBuffer.hpp
        src/Common/RegisterFile.hpp)
