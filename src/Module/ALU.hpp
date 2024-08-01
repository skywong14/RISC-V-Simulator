//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_ALU_HPP
#define RISC_V_SIMULATOR_ALU_HPP
#pragma once
#include "Decoder.hpp"
#include "Common/utils.hpp"
#include <iostream>


//IntegerALU

enum class CalcType {
    Add = 0, Sub = 1, ShiftL = 2, ShiftR = 3, ShiftR_Arith = 4,
    Xor = 5, And = 6, Or = 7, Equal = 8, NotEqual = 9, Less = 10,
    LessUnsigned = 11, GreaterEQ = 12, GreaterEQUnsigned = 13,
    LB = 14, LH = 15, LW = 16, LBU = 17, LHU = 18, SB = 19, SH = 20, SW = 21
};

std::string toString(CalcType type);

class ALU{
private:
    bool busy;
public:
    bool available(){
        return !busy;
    }
    void tick(){
        busy = false;
    }
    uint Execute(CalcType calcType, uint val1, uint val2, uint roBEntry){
        busy = true;
        switch (calcType) {
            case CalcType::Add:
                return val1 + val2;
            case CalcType::Sub:
                return val1 - val2;
            case CalcType::ShiftL:
                return val1 << val2;
            case CalcType::ShiftR: // 无符号右移操作，填充零
                return val1 >> val2;
            case CalcType::ShiftR_Arith: // 有符号右移操作，填充符号位
                return static_cast<uint>(static_cast<int>(val1) >> val2);
            case CalcType::Xor:
                return val1 ^ val2;
            case CalcType::And:
                return val1 & val2;
            case CalcType::Or:
                return val1 | val2;
            case CalcType::Equal:
                return val1 == val2;
            case CalcType::NotEqual:
                return val1 != val2;
            case CalcType::Less:
                return static_cast<uint>(static_cast<int>(val1) < static_cast<int>(val2));
            case CalcType::LessUnsigned:
                return val1 < val2;
            case CalcType::GreaterEQ:
                return static_cast<uint>(static_cast<int>(val1) >= static_cast<int>(val2));
            case CalcType::GreaterEQUnsigned:
                return val1 >= val2;
            default:
                throw std::invalid_argument("Unknown CalcType");
        }
    }
};

#endif //RISC_V_SIMULATOR_ALU_HPP
