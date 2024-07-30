//
// Created by skywa on 2024/7/28.
//

#ifndef RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
#define RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
#pragma once
#include "Common/Register.hpp"
#include "Decoder.hpp"
#include "Common/RegisterFile.hpp"
#include "ALU.hpp"
#include "ReorderBuffer.hpp"
#include <cstring>
#include <iostream>
using uint = unsigned int;

//class RoB;

enum class DataSource{
    TwoReg, RegNImm, TwoRegNImm, Imm
};

class ReservationStation{
private:
    ALU& alu;
    RegisterFile& rf;
    RoB& rob;
public:
    static const int StationSize = 5;
    struct RSEntry{
        Register<CalcType> calcType;
        Register<Opcode> opcode;
        Register<uint> Vj, Vk;
        Register<int> Qj, Qk;
        Register<uint> A; // 立即数或地址，用于记录指令中的常量值或内存地址
        Register<uint> robEntry; //该指令在RoB中编号
        Register<DataSource> dataSource;
        Register<bool> busy;
        void tick(){
            calcType.tick(); dataSource.tick();
            opcode.tick();
            Vj.tick(); Vk.tick(); Qj.tick(); Qk.tick();
            A.tick(); robEntry.tick(); busy.tick();
        }
        void putInstruction(CalcType calcType_, DataSource dataSource_, uint val1, uint val2, uint val3, uint RoBEntry_){
            busy = true;
            robEntry = RoBEntry_;
            calcType = calcType_;
            dataSource = dataSource_;
            if (dataSource == DataSource::TwoReg){
                if (!rf.is_busy(val1)) {
                    Vj = rf.readRegister(val1); Qj = -1;
                } else {
                    Vj = 0;  Qj = rf.getTag(val1);
                }
                if (!rf.is_busy(val2)) {
                    Vk = rf.readRegister(val2); Qk = -1;
                } else {
                    Vk = 0; Qk = rf.getTag(val2);
                }
            } else if (dataSource == DataSource::RegNImm){
                A = val2;
                if (!rf.is_busy(val1)) {
                    Vj = rf.readRegister(val1); Qj = -1;
                } else {
                    Vj = 0;  Qj = rf.getTag(val1);
                }
                Vk = 0; Qk = -1;
            } else if (dataSource == DataSource::TwoRegNImm){
                if (!rf.is_busy(val1)) {
                    Vj = rf.readRegister(val1); Qj = -1;
                } else {
                    Vj = 0;  Qj = rf.getTag(val1);
                }
                if (!rf.is_busy(val2)) {
                    Vk = rf.readRegister(val2); Qk = -1;
                } else {
                    Vk = 0; Qk = rf.getTag(val2);
                }
                A = val3;
            }
        }
    };
    RSEntry data[StationSize];

    ReservationStation(){
        for (int i = 0; i < StationSize; i++){
            data[i].busy.write(false);
        }
        tickRegister();
    }
    void tickRegister(){
        for (int i = 0; i < StationSize; i++)
            data[i].tick();
    }

    bool available(){
        for (int i = 0; i < StationSize; i++)
            if (!data[i].busy) return true;
        return false;
    }
    void insertEntry(CalcType calcType_, DataSource dataSource, uint val1, uint val2, uint val3, uint RoBEntry_){
        if (!available()) throw std::runtime_error("busy when joining");
        for (int i = 0; i < StationSize; i++)
            if (!data[i].busy) {
                data[i].putInstruction(calcType_, dataSource, val1, val2, val3, RoBEntry_);
                break;
            }
    }
    void Run(){
        for (int i = 0; i < StationSize; i++)
            if (data[i].busy && data[i].Qj == -1 && data[i].Qk == -1){
                // execute
                uint result = 0;
                if (static_cast<int>((CalcType)data[i].calcType) < 14) {
                    if (data[i].dataSource == DataSource::TwoReg)
                        result = alu.Execute(data[i].calcType, data[i].Vj, data[i].Vk, data[i].robEntry);
                    else if (data[i].dataSource == DataSource::Imm)
                        result = alu.Execute(data[i].calcType, data[i].Vj, data[i].A, data[i].robEntry);
                    rob.updateEntry(data[i].robEntry, result);
                    data[i].busy = false;
                } else if (static_cast<int>((CalcType)data[i].calcType) < 18) {
                    // load
                    if (data[i].dataSource == DataSource::RegNImm) {
                        data[i].A = alu.Execute(CalcType::Add, data[i].Vj, data[i].A, data[i].robEntry);
                        data[i].dataSource = DataSource::Imm;
                        // step 1 done
                    } else {
                        result = lsb.Execute(data[i].calcType, data[i].A, 0, data[i].robEntry);
                        rob.updateEntry(data[i].robEntry, result);
                        data[i].busy = false;
                        // step 2 done
                    }
                } else {
                    // store
                    rob.updateStoreEntry(data[i].robEntry, data[i].Vj, data[i].A);
                    data[i].busy = false;
                }
                break; // only one instruction can be executed in one cycle
            }
    }

    void updateEntry(uint robEntry, uint value) {
        for (int i = 0; i < StationSize; i++)
            if (data[i].busy){
                if (data[i].Qj == robEntry) {
                    data[i].Vj = value;
                    data[i].Qj = 0;
                }
                if (data[i].Qk == robEntry) {
                    data[i].Vk = value;
                    data[i].Qk = 0;
                }
            }
    }

    void tick() {
        Run();
        tickRegister();
    }
};


#endif //RISC_V_SIMULATOR_RESERVATIONSTATION_HPP
