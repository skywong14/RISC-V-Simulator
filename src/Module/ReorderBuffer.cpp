//
// Created by skywa on 2024/7/31.
//
#include "ReorderBuffer.hpp"

void RoB::tick() {
    if (flushFlag) flush();
    commitEntry();
    tickRegister();
}

void RoB::updateEntry(uint robEntry) {
    entries[robEntry].ready = true;
    //Notify RS
    rs.updateEntry(robEntry, entries[robEntry].value);
}


void RoB::tickRegister(){
    flushFlag.tick();
    haltFlag.tick();
    head.tick(); tail.tick();
    for (int i = 0; i < Size; i++)
        entries[i].tick();
}

void RoB::updateStoreEntry(uint robEntry, uint value, uint dest) {
    // STORE
//    std::cout<<"[Store] value: "<<value<<" dest: "<<dest<<std::endl;
    entries[robEntry].lsbEntry = entries[robEntry].value;
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
        bool flushNow = false;
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
                    iq.flushOldPC(entries[head].PC + 4);
//                    iq.PC = entries[head].PC + 4;
                } else {
                    // jump in fact
//                    iq.PC = entries[head].dest;
                    iq.flushOldPC(entries[head].dest);
                }
                flushNow = true;
                break;
            case RoBType::STOREB:
                //store value
                if (!lsb.ableToStore(entries[head].lsbEntry)) return; //wait
                lsb.store(1, entries[head].dest, entries[head].value);
                lsb.storeSuccess(entries[head].lsbEntry);
                break;
            case RoBType::STOREH:
                //store value
                if (!lsb.ableToStore(entries[head].lsbEntry)) return; //wait
                lsb.store(2, entries[head].dest, entries[head].value);
                lsb.storeSuccess(entries[head].lsbEntry);
                break;
            case RoBType::STOREW:
                //store value
                if (!lsb.ableToStore(entries[head].lsbEntry)) return; //wait
                lsb.store(4, entries[head].dest, entries[head].value);
                lsb.storeSuccess(entries[head].lsbEntry);
                break;
            case RoBType::REGISTER:
                rf.writeRegister(entries[head].dest, entries[head].value, head);
                break;
            case RoBType::JALR:
                rf.writeRegister(entries[head].dest, entries[head].PC + 4, head);
//                std::cout<<"[JALR] new PC: "<<entries[head].value<<std::endl;
                iq.flushOldPC(entries[head].value); // value = rs1 + imm
                flushAll();
                break;
            case RoBType::BRANCH:
                throw std::runtime_error("Impossible");
                break;
            case RoBType::EXIT:
                // 输出a0寄存器的后8位
                std::cout << (rf.forceReadRegister(10) & 0xff) << std::endl;
                haltFlag = true; //throw std::runtime_error("Exit~");
                break;
        }
//        commitDebug();
        head = (head + 1) % Size;
        entries[head].busy = false; entries[head].ready = false;
        if (flushNow) flushAll();
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
        std::cout << "    Entry " << i << ": ";
        Register<uint> v; Register<bool> b;
        std::cout << "Type: " << toString(entries[i].type.read()) << " ";
        v = entries[i].dest; std::cout << "Dest: " << v << " ";
        v = entries[i].value; std::cout << "Value: " << v << " ";
        b = entries[i].ready; std::cout << "Ready: " << b << " ";
        b = entries[i].busy; std::cout << "Busy: " << b << " ";
        v = entries[i].PC; std::cout << "PC: 0x" << std::hex << v << std::endl << std::dec;
    }
}

RoB::RoB(ReservationStation &rs_, BranchPredictor &bp_, LSB &lsb_, InstructionQueue &iq_, RegisterFile& rf_):
            rs(rs_), bp(bp_), lsb(lsb_), iq(iq_), rf(rf_) {
    head = 0; tail = 0;
    haltFlag = false;
    for (int i = 0; i < Size; i++) {
        entries[i].busy = false;
    }
}
std::string toString(RoBType type){
    switch (type) {
        case RoBType::REGISTER:
            return "REGISTER";
        case RoBType::BRANCH:
            return "BRANCH";
        case RoBType::JALR:
            return "JALR";
        case RoBType::BRANCH_SUCCESS:
            return "BRANCH_SUCCESS";
        case RoBType::BRANCH_FAIL:
            return "BRANCH_FAIL";
        case RoBType::EXIT:
            return "EXIT";
        case RoBType::STOREB:
            return "STOREB";
        case RoBType::STOREH:
            return "STOREH";
        case RoBType::STOREW:
            return "STOREW";
    }
    return "UNKNOWN";
}
void RoB::commitDebug() {
    extern int commit_cnt;
    commit_cnt++;
    std::cout<<"[Commit] "<<commit_cnt<<std::endl;
    std::cout << "Commit Entry: " << head.read() << std::endl;
    std::cout << "    Type: " << toString(entries[head].type) << std::endl;
    std::cout << "    Dest: " << entries[head].dest.read() << std::endl;
    std::cout << "    Value: " << entries[head].value.read() << std::endl;
    std::cout << "    PC: 0x" << std::hex << entries[head].PC.read() << std::endl;
    std::cout << std::dec;
}

void RoB::NotifyFlush() {
    flushFlag = true;
}

void RoB::flush() {
    head = 0; tail = 0;
    for (int i = 0; i < Size; i++)
        entries[i].busy = false;
    flushFlag = false;
}

void RoB::flushAll() {
//    std::cout<<"[Flush All]"<<std::endl;
    rf.NotifyFlush();
    iq.NotifyFlush();
    rs.NotifyFlush();
    lsb.NotifyFlush();
    NotifyFlush();
}

bool RoB::Halted() {
    return haltFlag;
}

uint RoB::getValue(uint robEntry) {
    return entries[robEntry].value;
}
