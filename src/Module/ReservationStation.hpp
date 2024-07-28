//
// Created by skywa on 2024/7/28.
//

#ifndef RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
#define RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
#pragma once
#include "../Common/Register.hpp"
#include "../Common/Decoder.hpp"
#include "../Common/RegisterFile.hpp"
#include <cstring>
#include <iostream>
using uint = unsigned int;
/*
 * 整数运算保留站（Integer ALU RS）：8 个
 * 浮点运算保留站（Floating Point ALU RS）：6 个
 * 加载/存储保留站（Load/Store RS）：6 个
 */
extern RegisterFile RF;
class ReservationStation{
    static const int StationSize = 5;
    struct RSEntry{
//        Register<std::string> name;
        Register<Opcode> opcode;
        Register<uint> Vj, Vk, Qj, Qk;
        Register<uint> A; // 立即数或地址，用于记录指令中的常量值或内存地址
        Register<uint> RoBEntry; //该指令在RoB中编号
        Register<bool> busy;
        void tick(){
//            name.tick();
            opcode.tick();
            Vj.tick(); Vk.tick(); Qj.tick(); Qk.tick();
            A.tick(); RoBEntry.tick(); busy.tick();
        }
        void putInstruction(Decoder instruction, uint RoBEntry_){
            busy = true;
            A = instruction.imm;
            RoBEntry = RoBEntry_;
            if (!RF.is_busy(instruction.rs1)) {
                Vj = RF.readRegister(instruction.rs1); Qj = 0;
            } else {
                Vj = 0;  Qj = instruction.rs1;
            }
            if (!RF.is_busy(instruction.rs2)) {
                Vk = RF.readRegister(instruction.rs2); Qk = 0;
            } else {
                Vk = 0; Qk = instruction.rs2;
            }
        }
    };
    RSEntry data[StationSize];

    ReservationStation(){
        for (int i = 0; i < StationSize; i++){
            data[i].busy.write(false);
        }
        tickAll();
    }
    void tickAll(){
        for (int i = 0; i < StationSize; i++)
            data[i].tick();
    }

    bool available(){
        for (int i = 0; i < StationSize; i++)
            if (!data[i].busy) return true;
        return false;
    }
    void join(Decoder instruction, uint RoBEntry){
        if (!available()) throw std::runtime_error("busy when joining");
        for (int i = 0; i < StationSize; i++)
            if (!data[i].busy) {
                data[i].putInstruction(instruction, RoBEntry);
                break;
            }
    }
    void Run(){

    }

    void tick(){
        Run();
        tickAll();
    }
};


#endif //RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
