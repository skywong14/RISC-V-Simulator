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
    Memory& mem;
public:
    LSB(Memory& mem_);
    uint loadInstruction(uint PC);
    void store(uint length, uint addr, uint value);
    uint load(uint length, uint addr, uint unsigned_flag);
    void tick();
};

#endif //RISC_V_SIMULATOR_LSB_HPP
