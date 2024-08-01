//
// Created by skywa on 2024/8/1.
//
#include "ALU.hpp"

std::string toString(CalcType type){
    switch (type) {
        case CalcType::Add: return "Add";
        case CalcType::Sub: return "Sub";
        case CalcType::ShiftL: return "ShiftL";
        case CalcType::ShiftR: return "ShiftR";
        case CalcType::ShiftR_Arith: return "ShiftR_Arith";
        case CalcType::Xor: return "Xor";
        case CalcType::And: return "And";
        case CalcType::Or: return "Or";
        case CalcType::Equal: return "Equal";
        case CalcType::NotEqual: return "NotEqual";
        case CalcType::Less: return "Less";
        case CalcType::LessUnsigned: return "LessUnsigned";
        case CalcType::GreaterEQ: return "GreaterEQ";
        case CalcType::GreaterEQUnsigned: return "GreaterEQUnsigned";
        case CalcType::LB: return "LB";
        case CalcType::LH: return "LH";
        case CalcType::LW: return "LW";
        case CalcType::LBU: return "LBU";
        case CalcType::LHU: return "LHU";
        case CalcType::SB: return "SB";
        case CalcType::SH: return "SH";
        case CalcType::SW: return "SW";
        default:
            throw std::invalid_argument("Unknown CalcType");
    }
}