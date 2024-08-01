//
// Created by skywa on 2024/7/30.
//

#ifndef RISC_V_SIMULATOR_BRANCHPREDICTOR_HPP
#define RISC_V_SIMULATOR_BRANCHPREDICTOR_HPP

#pragma once
#include "../Common/Register.hpp"
using uint = unsigned int;

class BranchPredictor{
private:
    Register<uint> reg;
    Register<uint> regList[64];
public:
    BranchPredictor(){
        reg = 0b10;
        for (int i = 0; i < 64; i++) regList[i] = 0b10;
    }
    void updateInfo(uint PC_, uint state_){
        uint pos = (PC_ >> 2) & 0b111111;
        if (state_) {
            // Jump
            // if (reg != 0b11) reg = reg + 1;
            if (regList[pos] != 0b11) regList[pos] = regList[pos] + 1;
        } else {
            // Not Jump
            // if (reg != 0b00) reg = reg - 1;
            if (regList[pos] != 0b00) regList[pos] = regList[pos] - 1;
        }
    }
    bool predict(uint PC_){
        uint pos = (PC_ >> 2) & 0b111111;
        return (regList[pos] & 0b10) != 0;
    }
    void tick(){
        reg.tick();
        for (int i = 0; i < 64; i++)
            regList[i].tick();
    }
};


#endif //RISC_V_SIMULATOR_BRANCHPREDICTOR_HPP
