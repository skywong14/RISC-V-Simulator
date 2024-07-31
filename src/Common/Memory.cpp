//
// Created by skywa on 2024/7/26.
//
#include "Memory.hpp"

Memory::Memory():mem{}{}

bool Memory::check_addr(uint addr) {
    if (addr < MEMORY_SIZE)
        return true;
    return false;
}

void Memory::write_word(uint addr, uint imm)  {
    if (!check_addr(addr)) return;
    *(int *) (mem + addr) = imm;
}

uint Memory::read_word(uint addr)  {
    if (!check_addr(addr)) return 0; // 如果地址无效，返回 0
    return *(int *) (mem + addr); // 将地址处的数据解释为 Immediate 类型并返回
}

void Memory::write_half_word(uint addr, uint imm)  {
    if (!check_addr(addr)) return;
    *(short *) (mem + addr) = imm;
}

uint Memory::read_half_word(uint addr)  {
    if (!check_addr(addr)) return 0;
    return *(short *) (mem + addr);
}

void Memory::write_byte(uint addr, uint imm)  {
    if (!check_addr(addr)) return;
    *(char *) (mem + addr) = imm;
}

uint Memory::read_byte(uint addr)  {
    if (!check_addr(addr)) return 0;
    return *(char *) (mem + addr);
}

void Memory::load_memory_from_stream(std::istream &is) {
    // 如果输入以@开头，那么移动吸入指针到@后的地址
    // 否则，将输入流中的数据解释为16进制数，存入内存中
    std::string line;
    uint addr = 0;
    while (std::getline(is, line)) {
        if (line[0] == '@') {
            addr = std::stoi(line.substr(1), nullptr, 16);
        } else {
            for (int i = 0; i < line.size(); i += 2) {
                mem[addr++] = std::stoi(line.substr(i, 2), nullptr, 16);
            }
        }
    }
}

// 从地址 beg_addr 到 end_addr 的内存数据
void Memory::display_memory(uint beg_addr, uint end_addr) {
    std::cout << "Memory: from " << beg_addr << " to " << end_addr << std::endl;
    for (uint i = beg_addr; i <= end_addr; i++) {
        std::cout << std::hex << (unsigned) mem[i] << " ";
    }
    std::cout << std::endl;
}
