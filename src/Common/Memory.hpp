//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_MEMORY_HPP
#define RISC_V_SIMULATOR_MEMORY_HPP
#pragma once
#include <iostream>
#include <cstring>

using uint = unsigned int;

class Memory{
private:
    static const int MEMORY_SIZE = 0x400000;
    unsigned char mem[MEMORY_SIZE];
public:
    Memory();
    bool check_addr(uint addr);

    uint read_word(uint addr);

    uint read_half_word(uint addr);

    uint read_byte(uint addr);

    void write_word(uint addr, uint imm);

    void write_half_word(uint addr, uint imm);

    void write_byte(uint addr, uint imm);

    void display_memory(uint beg_addr, uint end_addr);

    void load_memory_from_stream(std::istream &is);
};




#endif //RISC_V_SIMULATOR_MEMORY_HPP
