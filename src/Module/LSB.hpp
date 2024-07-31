//
// Created by skywa on 2024/7/28.
//

#ifndef RISC_V_SIMULATOR_LSB_HPP
#define RISC_V_SIMULATOR_LSB_HPP
#pragma once
#include "Common/Memory.hpp"
#include "Common/RegisterFile.hpp"

class LSB{
private:
    static const int BufferSize = 5;
    Memory& mem;
    Register<uint> head, tail;
    Register<uint> busy[BufferSize];
    Register<uint> mode[BufferSize]; // 0: store, 1: load
    Register<uint> addr[BufferSize];
    Register<uint> length[BufferSize];
public:
    Register<bool> flushFlag;
    LSB(Memory& mem_);
    uint loadInstruction(uint PC);
    void store(uint length, uint addr, uint value);
    uint load(uint length, uint addr, uint unsigned_flag);
    void tick();
    void tickRegister();
    void flush();
    bool available();
    void NotifyFlush();
};

#endif //RISC_V_SIMULATOR_LSB_HPP
