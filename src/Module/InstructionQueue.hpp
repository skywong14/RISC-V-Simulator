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
    static const int queueSize = 16;
    Register<uint> head, tail;
    Register<uint> instructions[queueSize];
    Register<uint> curPC[queueSize];
    Register<bool> busy[queueSize];
    Register<bool> jump[queueSize];
    ReservationStation& rs;
    RoB& rob;
    LSB& lsb;
    BranchPredictor& bp;

public:
    Register<uint> PC;
    Register<uint> flushPC;
    Register<bool> flushFlag;

    InstructionQueue(ReservationStation& rs_, RoB& rob_, LSB& lsb_, BranchPredictor& bp_);

    void tickRegister();
    void flush();
    void NotifyFlush();
    void newInstruction();
    void executeInstruction();
    void flushOldPC(uint value);

    void Run();
    void tick();
};


#endif //RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
