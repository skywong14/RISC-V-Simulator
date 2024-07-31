//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_CPU_HPP
#define RISC_V_SIMULATOR_CPU_HPP
#pragma once
#include "InstructionQueue.hpp"
#include "LSB.hpp"
#include "ALU.hpp"

class CPU{
private:
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

    void Run(){
        while (!halted){
            clock++;
            std::cout<<"-----Clock "<<clock<<"-----"<<std::endl;
            iq.tick();
            lsb.tick();
            rob.tick();
            rf.tickAll();
            bp.tick();
            rs.tick();
            alu.tick();
            std::cout<<std::endl<<std::endl;
            if (clock > 100) halted = true;
        }
    }
};


#endif //RISC_V_SIMULATOR_CPU_HPP
