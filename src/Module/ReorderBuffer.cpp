//
// Created by skywa on 2024/7/31.
//
#include "ReorderBuffer.hpp"

void RoB::tick() {
    commitEntry();
    tickRegister();
}

void RoB::updateEntry(uint robEntry) {
    entries[robEntry].ready = true;
    //Notify RS
    rs.updateEntry(robEntry, entries[robEntry].value);
}


void RoB::tickRegister(){
    for (int i = 0; i < Size; i++)
        entries[i].tick();
}

void RoB::updateStoreEntry(uint robEntry, uint value, uint dest) {
    // STORE
    entries[robEntry].value = value;
    entries[robEntry].dest = dest;
    entries[robEntry].ready = true;
}

void RoB::updateEntry(uint robEntry, uint value) {
    if (entries[robEntry].type == RoBType::BRANCH) {
        if (value == entries[robEntry].value)  entries[robEntry].type = RoBType::BRANCH_SUCCESS;
        else  entries[robEntry].type = RoBType::BRANCH_FAIL;
    }
    entries[robEntry].value = value;
    entries[robEntry].ready = true;

    //Notify RS

    rs.updateEntry(robEntry, entries[robEntry].value);
}

void RoB::commitEntry() {
    if (entries[head].busy && entries[head].ready) {

        switch (entries[head].type) {
            case RoBType::BRANCH_SUCCESS:
                //update Predictor
                bp.updateInfo(entries[head].PC, entries[head].value); // 1: Jump, 0: Not Jump
                break;
            case RoBType::BRANCH_FAIL:
                //PC, update Predictor, flush
                bp.updateInfo(entries[head].PC, entries[head].value); // 1: Jump, 0: Not Jump
                if (entries[head].value == 0){
                    // not jump in fact
                    iq.PC = entries[head].PC + 4;
                } else {
                    // jump in fact
                    iq.PC = entries[head].dest;
                }
//                flushAll(); //todo
                break;
            case RoBType::STOREB:
                //store value
                lsb.store(1, entries[head].dest, entries[head].value);
                break;
            case RoBType::STOREH:
                //store value
                lsb.store(2, entries[head].dest, entries[head].value);
                break;
            case RoBType::STOREW:
                //store value
                lsb.store(4, entries[head].dest, entries[head].value);
                break;
            case RoBType::REGISTER:
                rf.writeRegister(entries[head].dest, entries[head].value, head);
                break;
            case RoBType::JALR:
                rf.writeRegister(entries[head].dest, entries[head].PC + 4, head);
                iq.PC = entries[head].value; // value = rs1 + imm
//                flushAll(); todo
                break;
            case RoBType::BRANCH:
                throw std::runtime_error("Impossible");
                break;
            case RoBType::EXIT:
                //todo exit
                break;
        }
        head = (head + 1) % Size;
        entries[head].busy = false; entries[head].ready = false;
    }
}

bool RoB::available() {
    return !entries[tail].busy;
}

uint RoB::insertEntry(RoBType type_, uint value_, uint dest_, uint PC) {
    if (entries[tail].busy) {
        throw std::overflow_error("RoB is full");
    }
    entries[tail].type = type_;
    entries[tail].dest = dest_;
    entries[tail].value = value_;
    entries[tail].PC = PC;

    entries[tail].ready = false;
    entries[tail].busy = true;

    uint robEntry = tail;
    tail = (tail + 1) % Size;
    return robEntry;
}

void RoB::printStatus() const {
    std::cout << "RoB Status: " << std::endl;
    for (int i = 0; i < Size; i++) {
        std::cout << "Entry " << i << ": ";
        Register<uint> v; Register<bool> b;
        v = entries[i].dest; std::cout << "Dest: " << v << " ";
        v = entries[i].value; std::cout << "Value: " << v << " ";
        b = entries[i].ready; std::cout << "Ready: " << b << " ";
        b = entries[i].busy; std::cout << "Busy: " << b << " ";
        v = entries[i].PC; std::cout << "PC: " << v << std::endl;
    }
}

RoB::RoB(ReservationStation &rs_, BranchPredictor &bp_, LSB &lsb_, InstructionQueue &iq_, RegisterFile& rf_):
            rs(rs_), bp(bp_), lsb(lsb_), iq(iq_), rf(rf_){
    head = 0; tail = 0;
    for (int i = 0; i < Size; i++) {
        entries[i].busy = false;
    }
}