//
// Created by skywa on 2024/7/26.
//
#include <iostream>
#include "../Common/Memory.hpp"
#include "Decoder.hpp"
#include "CPU.hpp"

void debug1(Memory& mem) {
    std::cout << "------debug1-----" << std::endl;
    mem.display_memory(0, 0b00000111);
    mem.display_memory(0b00001000, 0b00011111);
    std::cout << std::dec;
}
int main() {
    freopen("input.txt", "r", stdin);
    Memory mem;
    mem.load_memory_from_stream(std::cin);

    std::cout<<std::bitset<32>(mem.read_word(4))<<std::endl;
    debug1(mem);
    CPU cpu(mem);
    cpu.Run();
    return 0;
}
