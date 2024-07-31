//
// Created by skywa on 2024/7/31.
//
#include "LSB.hpp"

uint LSB::load(uint length, uint addr, uint unsigned_flag) {
    uint val = 0;
    if (length == 1) val = mem.read_byte(addr);
    if (length == 2) val = mem.read_half_word(addr);
    if (length == 4) val = mem.read_word(addr);
    if (unsigned_flag && length == 1) val = (val & 0xff);
    if (unsigned_flag && length == 2) val = (val & 0xffff);
    return val;
}

void LSB::tick() {

}

void LSB::store(uint length, uint addr, uint value) {
    if (length == 1) mem.write_byte(addr, value);
    if (length == 2) mem.write_half_word(addr, value);
    if (length == 4) mem.write_word(addr, value);
}

uint LSB::loadInstruction(uint PC){
    return mem.read_word(PC);
}

LSB::LSB(Memory &mem_): mem(mem_){}
