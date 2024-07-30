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
public:
    BranchPredictor(){
        reg = 0b10;
    }
    void updateInfo(uint PC_, uint state_){
        if (state_) {
            // Jump
            if (reg != 0b11) reg = reg + 1;
        } else {
            // Not Jump
            if (reg != 0b00) reg = reg - 1;
        }
    }
    bool predict(uint PC_){
        return (reg & 0b10) != 0;
    }
    void tick(){
        reg.tick();
    }
};


#endif //RISC_V_SIMULATOR_BRANCHPREDICTOR_HPP
