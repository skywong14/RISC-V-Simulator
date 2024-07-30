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
    LSB(Memory& mem_): mem(mem_){}
    uint loadInstruction(uint PC){
        return mem.read_word(PC);
    }
    void storeInstruction(uint PC, uint instruction){
        mem.write_word(PC, instruction);
    }
    void storeData(uint addr, uint value){
        mem.storeData(addr, value);
    }
    uint loadData(uint addr){
        return mem.loadData(addr);
    }
    void tick(){

    }
};

#endif //RISC_V_SIMULATOR_LSB_HPP
