//
// Created by skywa on 2024/7/31.
//
#include "LSB.hpp"

uint LSB::load(uint length, uint addr, uint unsigned_flag) {
    uint val = 0;
//    std::cout<<"[lsb] load: addr:"<<addr<<", length:"<<length<<std::endl;
    if (length == 1) val = mem.read_byte(addr);
    if (length == 2) val = mem.read_half_word(addr);
    if (length == 4) val = mem.read_word(addr);
    if (unsigned_flag && length == 1) val = (val & 0xff);
    if (unsigned_flag && length == 2) val = (val & 0xffff);
    return val;
}

void LSB::tick() {
    if (flushFlag) flush();
    tickRegister();
}

void LSB::store(uint length, uint addr, uint value) {
//    std::cout<<"[lsb] store: addr:"<<addr<<", value:"<<value<<", length:"<<length<<std::endl;
    if (length == 1) mem.write_byte(addr, value);
    if (length == 2) mem.write_half_word(addr, value);
    if (length == 4) mem.write_word(addr, value);
}

uint LSB::loadInstruction(uint PC){
    return mem.read_word(PC);
}

LSB::LSB(Memory &mem_): mem(mem_){}

void LSB::tickRegister() {
    flushFlag.tick();
    head.tick(); tail.tick();
    for (int i = 0; i < BufferSize; i++) {
        busy[i].tick();
        mode[i].tick();
        addr[i].tick();
        length[i].tick();
    }
}

void LSB::flush() {
    flushFlag = false;
    head = 0; tail = 0;
    for (int i = 0; i < BufferSize; i++) {
        busy[i] = 0;
        mode[i] = 0;
        addr[i] = 0;
        length[i] = 0;
    }
}

void LSB::NotifyFlush() {
    flushFlag = true;
}

bool LSB::available() {
    return !busy[tail];
}
