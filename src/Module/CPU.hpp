//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_CPU_HPP
#define RISC_V_SIMULATOR_CPU_HPP
#pragma once
#include "InstructionQueue.hpp"
#include "LSB.hpp"
#include "ALU.hpp"
#include "BranchPredictor.hpp"
#include "ReservationStation.hpp"
#include "ReorderBuffer.hpp"
#include "Common/RegisterFile.hpp"

class CPU{
private:
    bool debugFlag = true;
    uint clock;
public:
    bool halted;
    InstructionQueue iq;
    LSB lsb;
    RoB rob;
    RegisterFile rf;
    ALU alu;
    BranchPredictor bp;
    ReservationStation rs;
    explicit CPU(Memory& mem): iq(rs, rob, lsb, bp), lsb(mem), rob(rs, bp, lsb, iq, rf), rf(), alu(), bp(), rs(alu, rf, rob, lsb){
        clock = 0;
        halted = false;
    }
    void Exit(){
        halted = true;
    }
    void Run(){
        extern int commit_cnt;
        while (!halted){
            clock++;
//            std::cout<<"-----Clock "<<clock<<"-----"<<std::endl;
            iq.tick();
            lsb.tick();
            rob.tick();
            rf.tick();
            bp.tick();
            rs.tick();
            alu.tick();
//            rob.printStatus();
//            rf.debug();
//            std::cout<<std::endl<<std::endl;
            if (rob.Halted()) Exit();
            if (commit_cnt  >=  1694 - 2 && false) {
                std::cout<<"----------------"<<std::endl;
                std::cout << "commit_cnt: " << commit_cnt << std::endl;
                rob.printStatus();
                lsb.debug();
                rs.PrintState();
                rf.debug();
                std::cout<<"----------------"<<std::endl;
//                debugFlag = false;
            }
        }
    }
};


#endif //RISC_V_SIMULATOR_CPU_HPP
