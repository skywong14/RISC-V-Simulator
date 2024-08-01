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
    if (flushFlag) {
        flush();
        tickRegister();
        return; //wait one tick
    }
    Run();
    tickRegister();
//    debug();
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
        ready[i].tick();
    }
}

void LSB::flush() {
//    std::cout<<"Querying LSB Flushed"<<std::endl;
    flushFlag = false;
    head = 0; tail = 0;
    for (int i = 0; i < BufferSize; i++) {
        busy[i] = 0;
        ready[i] = 0;
    }
}

void LSB::NotifyFlush() {
    flushFlag = true;
}

bool LSB::available() {
    return !busy[tail];
}

uint LSB::insertLoadCommand() {
    if (!available()) throw std::runtime_error("Insert Load Command to Full LSB");
    busy[tail] = true;
    ready[tail] = false;
    uint ret = tail;
    tail = (tail + 1) % BufferSize;
//    std::cout<<"Querying insertLoadCommand:"<<ret<<std::endl;
    return ret;
}

uint LSB::insertStoreCommand() {
    if (!available()) throw std::runtime_error("Insert Store Command to Full LSB");
    busy[tail] = true;
    ready[tail] = false;
    uint ret = tail;
    tail = (tail + 1) % BufferSize;
    return ret;
}

void LSB::Run() {
    if (flushFlag) {
        flush();
        return; //wait one tick
    }
    // checkHead
    if (!busy[head]) return;
    if (busy[head] && ready[head]) {
        busy[head] = false; ready[head] = false;
        head = (head + 1) % BufferSize;
    }
}

bool LSB::ableToLoad(uint entryId) {
//    std::cout<<"Querying ableToLoad:"<<entryId<<std::endl;
    if (head == entryId) return true;
    return false;
}


void LSB::loadSuccess(uint entryId) {
//    std::cout<<"Querying loadSuccess:"<<entryId<<std::endl;
    ready[entryId] = true;
}

bool LSB::ableToStore(uint entryId) {
//    std::cout<<"Querying ableToStore"<<std::endl;
    if (head == entryId) return true;
    return false;
}

void LSB::storeSuccess(uint entryId) {
    ready[entryId] = true;
}

void LSB::debug() {
    std::cout << "LSB Status: " << std::endl;
    std::cout << "Head: " << head << ", Tail: " << tail << std::endl;
    for (int i = 0; i < BufferSize; i++) {
        std::cout << "    Entry " << i << ": ";
        if (busy[i]) std::cout << "Busy, Ready: " << ready[i] << std::endl;
        else std::cout << "Not Busy" << std::endl;
    }
    std::cout<<std::endl;
}
