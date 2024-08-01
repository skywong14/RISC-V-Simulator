//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_REORDERBUFFER_HPP
#define RISC_V_SIMULATOR_REORDERBUFFER_HPP
#pragma once
#include "Common/Register.hpp"
#include "Decoder.hpp"
#include "Common/RegisterFile.hpp"
#include "BranchPredictor.hpp"
#include "InstructionQueue.hpp"
#include "ReservationStation.hpp"
#include "LSB.hpp"
#include <cstring>
#include <iostream>
using uint = unsigned int;
class InstructionQueue;

enum class RoBType{
    REGISTER, BRANCH, JALR, BRANCH_SUCCESS, BRANCH_FAIL, EXIT, STOREB, STOREH, STOREW
};
std::string toString(RoBType type);

class RoB {
private:
    static const bool commitDebugTag = false;
    ReservationStation& rs;
    BranchPredictor& bp;
    LSB& lsb;
    RegisterFile& rf;
    InstructionQueue& iq;
public:
    Register<bool> haltFlag;
    Register<bool> flushFlag;
    static const int Size = 10;
    struct RoBEntry {
        Register<RoBType> type;  // 计算指令
        Register<uint> dest; // 目的寄存器 or 目标内存地址
        Register<uint> value;  // 计算结果
        Register<bool> ready; // 结果是否已准备好
        Register<bool> busy;
        Register<uint> PC;  // 额外内容（如果需要）
        Register<uint> lsbEntry; // 额外内容（如果需要）
        void tick() {
            dest.tick(); value.tick(); ready.tick();
            PC.tick(); type.tick(); busy.tick(); lsbEntry.tick();
        }
        RoBEntry() {
            dest = 0; busy = false; ready = false; value = 0;
        }
    }entries[Size];
    Register<uint> head, tail; // [head, tail) has data

    // 构造函数，初始化 RoB 的大小
    RoB(ReservationStation& rs_, BranchPredictor& bp_, LSB& lsb_, InstructionQueue& iq_, RegisterFile& rf_);

    bool available();
    bool Halted();
    void Run();

    //insert robEntry to ReservationStation
    uint insertEntry(RoBType type_, uint value_, uint dest_, uint PC);

    // 更新条目结果
    void updateEntry(uint robEntry, uint value);
    void updateStoreEntry(uint robEntry, uint value, uint dest);


    void updateEntry(uint robEntry);
    // 提交条目
    void NotifyFlush();
    void commitEntry();
    void tickRegister();
    void tick();
    void flush();
    void flushAll();

    uint getValue(uint robEntry);

    void commitDebug();

    // 打印 RoB 状态

    void printStatus() const;
};


#endif //RISC_V_SIMULATOR_REORDERBUFFER_HPP
