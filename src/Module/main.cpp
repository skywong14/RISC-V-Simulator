//
// Created by skywa on 2024/7/26.
//
#include <iostream>
#include "../Common/Memory.hpp"
#include "Decoder.hpp"
#include "CPU.hpp"

int commit_cnt = 0;

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    Memory mem;
    mem.load_memory_from_stream(std::cin);
    CPU cpu(mem);
    cpu.Run();
    return 0;
}
