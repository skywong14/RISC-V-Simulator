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

// Register File存放着将要向此寄存器更新值的ROB entry tag
// 如果为0，代表此寄存器的值可用，否则，意味着该寄存器的值会被前面的指令更新，用该寄存器作为源操作数的需要等待CDB将更新后的值传输过来。
class RegisterFile {
private:
    static const int REG_NUM = 32;
    Register<int> registers[REG_NUM], busy[REG_NUM], tag[REG_NUM];
public:
    RegisterFile() {
        for (int i = 0; i < REG_NUM; ++i) {
            registers[i] = 0;
            busy[i] = 0;
            tag[i] = 0;
        }
    }

    void tick() {
        for (int i = 0; i < REG_NUM; i++){
            registers[i].tick();
            busy[i].tick();
            tag[i].tick();
        }
    }

    bool is_busy(int id) {
        return busy[id];
    }

    int read(int id) {
        if (!busy[id]) {
            return id == 0 ? 0 : registers[id];
        } else {
            throw std::runtime_error("Register is busy");
        }
    }
    // 只能由ROB通过CDB去更新，写后释放
    void write(int id, int val) {
        registers[id] = val;
        busy[id] = 0;
        tag[id] = 0;
    }
    void setTag(int reg, int tag_) {
        tag[reg] = tag_;
        busy[reg] = 1;
    }
    int getTag(int id) {
        return tag[id];
    }

    void debug() {
        static const std::string rf_name[32] = {"0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                                              "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                                              "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                                              "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
        std::cout << "Register File Status:\n";
        for (int i = 0; i < REG_NUM; ++i) {
            std::cout << "  R" << i << " (" << rf_name[i] << "): "
                      << "Value=" << registers[i]
                      << ", Valid=" << (busy[i] ? "true" : "false")
                      << ", Tag=" << tag[i] << "\n";
        }
    }
};


#endif //RISC_V_SIMULATOR_REGISTERFILE_HPP
