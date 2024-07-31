//
// Created by skywa on 2024/7/26.
//
#include <sstream>
#include "Memory.hpp"

Memory::Memory():mem{}{}

bool Memory::check_addr(uint addr) {
    if (addr < MEMORY_SIZE)
        return true;
    return false;
}

void Memory::write_word(uint addr, uint imm) {
    if (!check_addr(addr)) return;
    mem[addr] = imm & 0xFF;
    mem[addr + 1] = (imm >> 8) & 0xFF;
    mem[addr + 2] = (imm >> 16) & 0xFF;
    mem[addr + 3] = (imm >> 24) & 0xFF;
}

uint Memory::read_word(uint addr) {
    if (!check_addr(addr)) return 0;
    return mem[addr] | (mem[addr + 1] << 8) | (mem[addr + 2] << 16) | (mem[addr + 3] << 24);
}

void Memory::write_half_word(uint addr, uint imm) {
    if (!check_addr(addr)) return;
    mem[addr] = imm & 0xFF;
    mem[addr + 1] = (imm >> 8) & 0xFF;
}

uint Memory::read_half_word(uint addr) {
    if (!check_addr(addr)) return 0;
    return mem[addr] | (mem[addr + 1] << 8);
}

void Memory::write_byte(uint addr, uint imm)  {
    if (!check_addr(addr)) return;
    *(uint8_t *) (mem + addr) = imm;
}

uint Memory::read_byte(uint addr)  {
    if (!check_addr(addr)) return 0;
    return *(uint8_t *) (mem + addr);
}

void Memory::load_memory_from_stream(std::istream &is) {
    // 如果输入以@开头，那么移动写入指针到@后的地址，这个地址是十六进制的
    // 否则，将输入流中的16进制数据写入内存, 注意，mem中的数据是以字节为单位的
    // 输入格式如下
    // @00000000
    // 37 01 02 00 EF 10 00 04 13 05 F0 0F B7 06 03 00
    // 23 82 A6 00 6F F0 9F FF
    // ...直到文件结束
    std::string line;
    uint addr = 0;
    while (std::getline(is, line)) {
        if (line[0] == '@') {
            addr = std::stoi(line.substr(1), nullptr, 16);
        } else {
            std::istringstream iss(line);
            uint val;
            while (iss >> std::hex >> val) {
                write_byte(addr++, val);
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
