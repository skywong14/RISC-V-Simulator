cmake_minimum_required(VERSION 3.22)
project(RISC_V_Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(code
        src/Module/main.cpp
        src/Common/Register.hpp
        src/Common/Memory.hpp
        src/Module/Decoder.hpp
        src/Module/CPU.hpp
        src/Common/Memory.cpp
        src/Module/ALU.hpp
        src/Module/ReorderBuffer.hpp
        src/Common/RegisterFile.hpp
        src/Module/ReservationStation.hpp
        src/Module/LSB.hpp
        src/Common/utils.hpp
        src/Module/InstructionQueue.hpp
        src/Module/BranchPredictor.hpp
        src/Common/CDB.hpp
        src/Module/InstructionQueue.cpp
        src/Module/LSB.cpp
        src/Module/ReorderBuffer.cpp
        src/Module/ReservationStation.cpp
        src/Common/utils.cpp
        src/Module/Decoder.cpp
        src/Module/ALU.cpp)
