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
    CPU(Memory& mem): bp(), lsb(mem), rf(),  alu(), rs(), iq(rs, rob, lsb, bp), rob(){
        clock = 0;
        halted = false;
    }

    void Run(){
        while (!halted){
            clock++;
            iq.tick();
            lsb.tick();
            rob.tick();
            rf.tickAll();
        }


    }
};


#endif //RISC_V_SIMULATOR_CPU_HPP
