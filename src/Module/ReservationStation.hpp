//
// Created by skywa on 2024/7/28.
//

#ifndef RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
#define RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
#pragma once
#include "Common/Register.hpp"
#include "Common/RegisterFile.hpp"
#include "ALU.hpp"
#include "ReorderBuffer.hpp"
#include <cstring>
#include <iostream>
using uint = unsigned int;


enum class DataSource{
    TwoReg, RegNImm, TwoRegNImm, Imm
};


class RoB;
class LSB;

class   ReservationStation{
private:
    ALU& alu;
    RegisterFile& rf;
    RoB& rob;
    LSB& lsb;
public:
    Register<int> updateBufferEntry;
    Register<uint> updateBufferVal;
    Register<bool> flushFlag;
    static const int StationSize = 10;
    struct RSEntry{
        Register<CalcType> calcType;
        Register<Opcode> opcode;
        Register<uint> Vj, Vk;
        Register<int> Qj, Qk;
        Register<uint> A; // 立即数或地址，用于记录指令中的常量值或内存地址
        Register<int> robEntry; //该指令在RoB中编号
        Register<DataSource> dataSource;
        Register<bool> busy;
        void tick();
        void putInstruction(CalcType calcType_, DataSource dataSource_,
                            uint val1, uint val2, uint val3, uint RoBEntry_, RegisterFile& rf_);
    };
    RSEntry data[StationSize];

    ReservationStation(ALU& alu_, RegisterFile& rf_, RoB& rob_, LSB& lsb_);
    void tickRegister();

    bool available();
    void insertEntry(CalcType calcType_, DataSource dataSource, uint val1, uint val2, uint val3, uint RoBEntry_);
    void Run();

    void updateEntry(uint robEntry, uint value);

    void tick();
    void NotifyFlush();
    void flush();

    void PrintState();
    void loadUpdateBuffer();
};


#endif //RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
