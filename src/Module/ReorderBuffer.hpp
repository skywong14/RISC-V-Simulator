//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_REORDERBUFFER_HPP
#define RISC_V_SIMULATOR_REORDERBUFFER_HPP
#pragma once
#include "../Common/Register.hpp"
#include "../Common/Decoder.hpp"
#include "../Common/RegisterFile.hpp"
#include <cstring>
#include <iostream>

class RoB {
public:
    static const int Size = 10;
    struct RoBEntry {
        Register<uint> dest; // 目的寄存器
        Register<uint> value;  // 计算结果
        Register<bool> ready; // 结果是否已准备好
        Register<bool> busy;
        Register<std::string> instruction; // 指令类型 todo
        void tick() {
            dest.tick(); value.tick(); ready.tick(); busy.tick();
        }
        RoBEntry() {
            dest = 0; busy = false; ready = false; value = 0;
        }
    }entries[Size];
    Register<uint> head, tail; // [head, tail) has data

    // 构造函数，初始化 RoB 的大小
    RoB() {
        head = 0;
        tail = 0;
    }

    bool available() {

    }
    // 插入新条目
    int insertEntry(const std::string& instruction, int dest) {
        if (entries[tail].busy) {
            throw std::overflow_error("RoB is full");
        }
        entries[tail].instruction = instruction;
        entries[tail].dest = dest;
        entries[tail].ready = false;
        entries[tail].busy = true;

        uint robEntry = tail;
        tail = (tail + 1) % Size;
        return robEntry;
    }

    // 更新条目结果
    void updateEntry(int robEntry, int value) {
        if (robEntry < 0 || robEntry >= Size) {
            throw std::out_of_range("Invalid RoB entry");
        }
        entries[robEntry].value = value;
        entries[robEntry].ready = true;
    }

    // 提交条目
    void commitEntry() {
        if (!entries[head].busy || !entries[head].ready) {
            throw std::runtime_error("No entry to commit or entry not ready");
        }
        // 提交条目的操作
//        std::cout << "Committing: " << entries[head].instruction
//                  << " with result: " << entries[head].value
//                  << " to register: " << entries[head].dest << std::endl;
        entries[head].busy = false;
        head = (head + 1) % Size;
    }

    // 打印 RoB 状态
    /*void printStatus() const {
        for (int i = 0; i < Size; i++) {
            std::cout << "RoB Entry " << i
                      << ": Instruction = " << entries[i].instruction
                      << ", Dest = " << entries[i].dest
                      << ", Value = " << entries[i].value
                      << ", Ready = " << entries[i].ready
                      << ", Busy = " << entries[i].busy << std::endl;
        }
    }*/
};


#endif //RISC_V_SIMULATOR_REORDERBUFFER_HPP
