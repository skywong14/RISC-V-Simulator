//
// Created by skywa on 2024/7/26.
//
#include <iostream>
#include "../Common/Memory.hpp"
#include "Decoder.hpp"
#include "CPU.hpp"

void debug1(Memory& mem) {
    std::cout << "------debug1-----" << std::endl;
    mem.display_memory(0, 0b00001000);
    mem.display_memory(0b00001000, 0b00100000);
}
int main() {
    freopen("input.txt", "r", stdin);
    Memory mem;
    mem.load_memory_from_stream(std::cin);
    debug1(mem);
    CPU cpu(mem);
    cpu.Run();
    return 0;
}
