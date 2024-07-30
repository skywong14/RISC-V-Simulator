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
#include <cstring>
#include <iostream>

enum class RoBType{
    REGISTER, STORE, BRANCH, JALR, BRANCH_SUCCESS, BRANCH_FAIL, EXIT
};
class RoB {
public:
    static const int Size = 10;
    struct RoBEntry {
        Register<RoBType> type;  // 计算指令
        Register<uint> dest; // 目的寄存器 or 目标内存地址
        Register<uint> value;  // 计算结果
        Register<bool> ready; // 结果是否已准备好
        Register<bool> busy;
        Register<uint> PC;  // 额外内容（如果需要）
        void tick() {
            dest.tick(); value.tick(); ready.tick();
            PC.tick(); type.tick(); busy.tick();
        }
        RoBEntry() {
            dest = 0; busy = false; ready = false; value = 0;
        }
    }entries[Size];
    Register<uint> head, tail; // [head, tail) has data

    // 构造函数，初始化 RoB 的大小
    RoB():head(0), tail(0) {}

    bool available() {
        return !entries[tail].busy;
    }

    //insert robEntry to ReservationStation
    uint insertEntry(RoBType type_, uint value_, uint dest_, uint PC) {
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

    // 更新条目结果
    void updateEntry(uint robEntry, uint value) {
        if (entries[robEntry].type == RoBType::BRANCH) {
            if (value == entries[robEntry].value)  entries[robEntry].type = RoBType::BRANCH_SUCCESS;
            else  entries[robEntry].type = RoBType::BRANCH_FAIL;
        }
        entries[robEntry].value = value;
        entries[robEntry].ready = true;

        //Notify RS

        rs.updateEntry(robEntry, entries[robEntry].value);
    }
    void updateStoreEntry(uint robEntry, uint value, uint dest) {
        // STORE
        entries[robEntry].value = value;
        entries[robEntry].dest = dest;
        entries[robEntry].ready = true;
    }


    void updateEntry(uint robEntry) {
        entries[robEntry].ready = true;
        //Notify RS

        rs.updateEntry(robEntry, entries[robEntry].value);
    }

    // 提交条目
    void commitEntry() {
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
                        PC = entries[head].PC + 4;
                    } else {
                        // jump in fact
                        PC = entries[head].dest;
                    }
                    flushAll(); //todo
                    break;
                case RoBType::STORE:
                    //store value
                    lsb.store(entries[head].dest, entries[head].value);
                    break;
                case RoBType::REGISTER:
                    rf.writeRegister(entries[head].dest, entries[head].value, head);
                    break;
                case RoBType::JALR:
                    rf.writeRegister(entries[head].dest, entries[head].PC + 4, head);
                    PC = entries[head].value; // value = rs1 + imm
                    flushAll();
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
    void tickRegister(){
        for (int i = 0; i < Size; i++)
            entries[i].tick();
    }
    void tick(){
        commitEntry();
        tickRegister();
    }
    // 打印 RoB 状态
    /*
    void printStatus() const {
        std::cout << "RoB Status: " << std::endl;
        for (int i = 0; i < Size; i++) {
            std::cout << "Entry " << i << ": ";
            std::cout << "Type: " << entries[i].type << " ";
            std::cout << "Dest: " << entries[i].dest << " ";
            std::cout << "Value: " << entries[i].value << " ";
            std::cout << "Ready: " << entries[i].ready << " ";
            std::cout << "Busy: " << entries[i].busy << " ";
            std::cout << "PC: " << entries[i].PC << std::endl;
        }
    }
     */
};


#endif //RISC_V_SIMULATOR_REORDERBUFFER_HPP
