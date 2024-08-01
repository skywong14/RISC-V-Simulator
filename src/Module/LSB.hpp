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
    static const int BufferSize = 10;
    Memory& mem;
    Register<uint> head, tail;
    Register<uint> busy[BufferSize];
    Register<uint> ready[BufferSize];
public:
    Register<bool> flushFlag;
    explicit LSB(Memory& mem_);
    uint loadInstruction(uint PC);
    void store(uint length, uint addr, uint value);
    uint load(uint length, uint addr, uint unsigned_flag);
    void tick();
    void tickRegister();
    void flush();
    void NotifyFlush();

    bool available(); //是否有空位
    uint insertLoadCommand();
    uint insertStoreCommand();
    void Run();
    bool ableToLoad(uint entryId);
    bool ableToStore(uint entryId);
    void loadSuccess(uint entryId);
    void storeSuccess(uint entryId);

    void debug();
};

#endif //RISC_V_SIMULATOR_LSB_HPP
