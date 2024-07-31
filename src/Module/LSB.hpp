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
    static const int BufferSize = 1;
    Memory& mem;
    Register<uint> head, tail;
    Register<uint> busy[BufferSize];
    Register<uint> mode[BufferSize]; // 0: store, 1: load unsigned, 2: load signed
    Register<uint> addr[BufferSize];
    Register<uint> length[BufferSize];
    Register<uint> ready[BufferSize];
    Register<uint> value[BufferSize];
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
    uint insertLoadCommand(uint unsignedFlag_, uint addr_, uint length_);
    uint insertStoreCommand(uint addr_, uint length_);
    void Run();
    bool ableToLoad(uint entryId);
    bool ableToStore(uint entryId);
    void loadSuccess(uint entryId);
    void storeSuccess(uint entryId);
};

#endif //RISC_V_SIMULATOR_LSB_HPP
