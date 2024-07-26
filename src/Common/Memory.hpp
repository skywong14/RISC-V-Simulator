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
    static const int MEMORY_SIZE = 0x400000;
    Memory();
    bool check_addr(uint addr);

    // 读取 32 位（4 字节）数据
    int read_word(uint addr);

    // 写入 32 位（4 字节）数据到指定地址
    void write_word(uint addr, int imm);

    void display_memory(uint beg_addr, uint end_addr);
private:
    unsigned char mem[MEMORY_SIZE];
};




#endif //RISC_V_SIMULATOR_MEMORY_HPP
