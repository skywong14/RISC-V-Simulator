//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_REGISTERFILE_HPP
#define RISC_V_SIMULATOR_REGISTERFILE_HPP
#pragma once
#include "Register.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>
using uint = unsigned int;
// Register File存放着将要向此寄存器更新值的ROB entry tag
// 如果为0，代表此寄存器的值可用，否则，意味着该寄存器的值会被前面的指令更新，用该寄存器作为源操作数的需要等待CDB将更新后的值传输过来。
class RegisterFile {
private:
    static const int REG_NUM = 32;
    Register<uint> registers[REG_NUM];
    Register<int> tag[REG_NUM]; // 对应的 ROB 条目
    Register<bool> busy[REG_NUM];  // 寄存器是否busy(等待写回)
    Register<bool> flushFlag; // true if flush is needed
public:
    RegisterFile() {
        for (int i = 0; i < REG_NUM; ++i) {
            registers[i] = 0;
            busy[i] = false;
            tag[i] = -1;
        }
    }

    void NotifyFlush() {
        flushFlag = true;
    }

    void tickAll() {
        flushFlag.tick();
        for (int i = 0; i < REG_NUM; i++){
            registers[i].tick();
            busy[i].tick();
            tag[i].tick();
        }
    }
    void flush(){
        for (int i = 0; i < REG_NUM; ++i) {
            busy[i] = false;
            tag[i] = 0;
        }
        flushFlag = false;
    }
    void tick(){
        if (flushFlag) {
            flush();
            tickAll();
            return;
        }
        tickAll();
    }

    bool is_busy(uint id) {
        return id != 0 && busy[id];
    }
    uint forceReadRegister(uint id) {
        return id == 0 ? 0 : registers[id];
    }
    uint readRegister(uint id) {
        if (!busy[id]) {
            return id == 0 ? 0 : registers[id];
        } else {
            throw std::runtime_error("Register is busy");
        }
    }
    void writeRegister(uint reg, uint value, uint robEntry) {
        if (reg == 0) return;
        registers[reg] = value;
        if (busy[reg] && tag[reg] == robEntry) {
            busy[reg] = false;
            tag[reg] = -1;
        }
    }
    void updateRegisterStatus(uint reg, int robEntry) {
        if (reg == 0) return;
        tag[reg] = robEntry;
        busy[reg] = true;
    }
    int getTag(uint id) {
        return tag[id];
    }

    void debug() {
        static const std::string rf_name[32] = {"0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                                              "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                                              "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                                              "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
        std::cout << "Register File Status:\n";
        for (int i = 0; i < REG_NUM; ++i) {
            //统一用16进制输出val，并且8位对齐
            std::cout << "  R" << i << " (" << rf_name[i] << "): "
                      << "Val=0x" << std::hex << std::setw(8) << std::setfill('0') << registers[i]
                      << ", busy=" << (busy[i] ? "true" : "false")
                      << ", Tag=" << std::dec <<tag[i] << "\n";
            //恢复十进制

        }
    }
};


#endif //RISC_V_SIMULATOR_REGISTERFILE_HPP
