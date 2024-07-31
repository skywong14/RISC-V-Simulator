//
// Created by skywa on 2024/7/31.
//
#include "ReservationStation.hpp"

void ReservationStation::RSEntry::tick(){
    calcType.tick(); dataSource.tick();
    opcode.tick();
    Vj.tick(); Vk.tick(); Qj.tick(); Qk.tick();
    A.tick(); robEntry.tick(); busy.tick();
}

void
ReservationStation::RSEntry::putInstruction(CalcType calcType_, DataSource dataSource_, uint val1, uint val2, uint val3,
                                            uint RoBEntry_, RegisterFile &rf_) {
    busy = true;
    robEntry = RoBEntry_;
    calcType = calcType_;
    dataSource = dataSource_;
    if (dataSource_ == DataSource::TwoReg){
        if (!rf_.is_busy(val1)) {
            Vj = rf_.readRegister(val1); Qj = -1;
        } else {
            Vj = 0;  Qj = rf_.getTag(val1);
        }
        if (!rf_.is_busy(val2)) {
            Vk = rf_.readRegister(val2); Qk = -1;
        } else {
            Vk = 0; Qk = rf_.getTag(val2);
        }
    } else if (dataSource_ == DataSource::RegNImm){
        A = val2;
        if (!rf_.is_busy(val1)) {
            Vj = rf_.readRegister(val1); Qj = -1;
        } else {
            Vj = 0;  Qj = rf_.getTag(val1);
        }
        Vk = 0; Qk = -1;
    } else if (dataSource_ == DataSource::TwoRegNImm){
        if (!rf_.is_busy(val1)) {
            Vj = rf_.readRegister(val1); Qj = -1;
        } else {
            Vj = 0;  Qj = rf_.getTag(val1);
        }
        if (!rf_.is_busy(val2)) {
            Vk = rf_.readRegister(val2); Qk = -1;
        } else {
            Vk = 0; Qk = rf_.getTag(val2);
        }
        A = val3;
    }
}

void ReservationStation::tickRegister() {
    flushFlag.tick();
    for (int i = 0; i < StationSize; i++)
        data[i].tick();
}

bool ReservationStation::available() {
    for (int i = 0; i < StationSize; i++)
        if (!data[i].busy) return true;
    return false;
}

void ReservationStation::insertEntry(CalcType calcType_, DataSource dataSource, uint val1, uint val2, uint val3,
                                     uint RoBEntry_) {
    if (!available()) throw std::runtime_error("busy when joining");
    for (int i = 0; i < StationSize; i++)
        if (!data[i].busy) {
//            std::cout<<"inserting instruction to RS"<<std::endl;
            data[i].putInstruction(calcType_, dataSource, val1, val2, val3, RoBEntry_, rf);
            break;
        }
}

void ReservationStation::Run() {
    for (int i = 0; i < StationSize; i++)
        if (data[i].busy && data[i].Qj == -1 && data[i].Qk == -1){
            // execute
            uint result = 0;
            if (static_cast<int>((CalcType)data[i].calcType) < 14) {
                if (data[i].dataSource == DataSource::TwoReg)
                    result = alu.Execute(data[i].calcType, data[i].Vj, data[i].Vk, data[i].robEntry);
                else if (data[i].dataSource == DataSource::RegNImm)
                    result = alu.Execute(data[i].calcType, data[i].Vj, data[i].A, data[i].robEntry);
                rob.updateEntry(data[i].robEntry, result);
                data[i].busy = false;
            } else if (static_cast<int>((CalcType)data[i].calcType) < 19) {
                // load
//                std::cout<<"Loading to "<<data[i].robEntry<<','<<static_cast<int>(data[i].calcType.read())<<"...";
                if (data[i].dataSource == DataSource::RegNImm) {
                    // load step 1
                    data[i].A = alu.Execute(CalcType::Add, data[i].Vj, data[i].A, data[i].robEntry);
                    data[i].dataSource = DataSource::Imm;
                    data[i].Vj = rob.getValue(data[i].robEntry); // the lsbEntryId
//                    std::cout<<"A: "<<data[i].A.current()<<std::endl;
                } else {
                    // load step 2
                    //now Vj stores the lsbEntryId
                    if (lsb.ableToLoad(data[i].Vj)) {
                        uint length = 0;
                        if (static_cast<int>((CalcType)data[i].calcType) == 14 || static_cast<int>((CalcType)data[i].calcType) == 17) length = 1;
                        else if (static_cast<int>((CalcType)data[i].calcType) == 15 || static_cast<int>((CalcType)data[i].calcType) == 18) length = 2;
                        else if (static_cast<int>((CalcType)data[i].calcType) == 16) length = 4;
                        uint unsignedFlag = 0;
                        if (static_cast<int>((CalcType)data[i].calcType) == 17 || static_cast<int>((CalcType)data[i].calcType) == 18) unsignedFlag = 1;
                        result = lsb.load(length, data[i].A, unsignedFlag);
                        lsb.loadSuccess(data[i].Vj);
//                      std::cout<<"Result: "<<result<<std::endl;
                        rob.updateEntry(data[i].robEntry, result);
                        data[i].busy = false;
                        // step 2 done
                    }
                    // else wait for lsb
                }
            } else {
                // store
                result = alu.Execute(CalcType::Add, data[i].Vk, data[i].A, 0); //robEntry is not used
                rob.updateStoreEntry(data[i].robEntry, data[i].Vj, result);
                data[i].busy = false;
            }
            break; // only one instruction can be executed in one cycle
        }
}

void ReservationStation::updateEntry(uint robEntry, uint value) {
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

void ReservationStation::tick() {
    if (flushFlag) flush();
    Run();
    tickRegister();
//    PrintState(); // debug
}

ReservationStation::ReservationStation(ALU &alu_, RegisterFile &rf_, RoB &rob_, LSB& lsb_): alu(alu_), rf(rf_), rob(rob_), lsb(lsb_){
    for (int i = 0; i < StationSize; i++)
        data[i].busy.write(false);
    tickRegister();
}

void ReservationStation::PrintState() {
    std::cout << "Reservation Station:" << std::endl;
    for (int i = 0; i < StationSize; i++){
        std::cout << "    Entry " << i << ": ";
        if (data[i].busy) {
            std::cout << "Busy, ";
            std::cout << "CalcType: " << static_cast<int>((CalcType)data[i].calcType) << ", ";
            std::cout << "DataSource: " << static_cast<int>((DataSource)data[i].dataSource) << ", ";
            std::cout << "Vj: " << data[i].Vj << ", ";
            std::cout << "Vk: " << data[i].Vk << ", ";
            std::cout << "Qj: " << data[i].Qj << ", ";
            std::cout << "Qk: " << data[i].Qk << ", ";
            std::cout << "A: " << data[i].A << ", ";
            std::cout << "RoBEntry: " << data[i].robEntry << std::endl;
        } else {
            std::cout << "Not Busy" << std::endl;
        }
    }
    std::cout << std::endl;
}

void ReservationStation::NotifyFlush() {
    flushFlag = true;
}

void ReservationStation::flush() {
    for (int i = 0; i < StationSize; i++)
        data[i].busy = false;
    flushFlag = false;
}
