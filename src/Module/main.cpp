//
// Created by skywa on 2024/7/26.
//
#include <iostream>
#include "../Common/Memory.hpp"
#include "Decoder.hpp"
#include "CPU.hpp"


int main() {
    Memory mem;
    mem.load_memory(std::cin);
    CPU cpu(mem);
    cpu.Run();
    return 0;
}
