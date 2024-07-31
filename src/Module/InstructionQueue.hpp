//
// Created by skywa on 2024/7/29.
//

#ifndef RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
#define RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
#include "Common/Register.hpp"
#include "Common/Memory.hpp"
#include "Common/RegisterFile.hpp"
#include "ReorderBuffer.hpp"
#include "LSB.hpp"

using uint = unsigned int;

class ReservationStation;
class RoB;

class InstructionQueue{
    static const int queueSize = 8;
    Register<uint> head, tail;
    Register<uint> instructions[queueSize];
    Register<uint> curPC[queueSize];
    Register<bool> busy[queueSize];
    ReservationStation& rs;
    RoB& rob;
    LSB& lsb;
    BranchPredictor& bp;

public:
    Register<uint> PC;

    InstructionQueue(ReservationStation& rs_, RoB& rob_, LSB& lsb_, BranchPredictor& bp_);

    uint readPC();
    void writePC(uint value);

    void tickRegister();
    void flush();
    void newInstruction();
    void executeInstruction();

    void Run();
    void tick();
};


#endif //RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
