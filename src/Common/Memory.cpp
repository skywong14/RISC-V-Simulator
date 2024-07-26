//
// Created by skywa on 2024/7/26.
//
#include "Memory.hpp"

Memory::Memory():mem{}{}

bool Memory::check_addr(uint addr) {
    if (0 <= addr && addr < MEMORY_SIZE)
        return true;
    return false;
}

void Memory::write_word(uint addr, int imm)  {
    if (!check_addr(addr)) return;
    *(int *) (mem + addr) = imm;
}

int Memory::read_word(uint addr)  {
    if (!check_addr(addr)) return 0; // 如果地址无效，返回 0
    return *(int *) (mem + addr); // 将地址处的数据解释为 Immediate 类型并返回
}

void Memory::display_memory(uint beg_addr, uint end_addr) {
    for (uint i = beg_addr; i <= end_addr; i++) {
        std::cout << std::hex << (unsigned) mem[i] << " ";
    }
    std::cout << std::endl;
}
