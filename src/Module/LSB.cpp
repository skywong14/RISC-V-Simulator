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
    Run();
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
        ready[i].tick();
        value[i].tick();
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

uint LSB::insertLoadCommand(uint unsignedFlag_, uint addr_, uint length_) {
    if (!available()) throw std::runtime_error("Insert Load Command to Full LSB");
    busy[tail] = true;
    if (unsignedFlag_) mode[tail] = 1;
    else mode[tail] = 2;
    addr[tail] = addr_;
    ready[tail] = false;
    length[tail] = length_;
    uint ret = tail;
    tail = (tail + 1) % BufferSize;
    return ret;
}

uint LSB::insertStoreCommand(uint addr_, uint length_) {
    if (!available()) throw std::runtime_error("Insert Store Command to Full LSB");
    busy[tail] = true;
    mode[tail] = 0;
    addr[tail] = addr_;
    ready[tail] = false;
    length[tail] = length_;
    uint ret = tail;
    tail = (tail + 1) % BufferSize;
    return ret;
}

void LSB::Run() {
    // checkHead
    if (!busy[head]) return;
    if (busy[head] && ready[head]) {
        busy[head] = false; ready[head] = false;
        head = (head + 1) % BufferSize;
    }
}

bool LSB::ableToLoad(uint entryId) {
    if (head == entryId) return true;
    return false;
}


void LSB::loadSuccess(uint entryId) {
    ready[entryId] = true;
}

bool LSB::ableToStore(uint entryId) {
    if (head == entryId) return true;
    return false;
}

void LSB::storeSuccess(uint entryId) {
    ready[entryId] = true;
}
