//
// Created by skywa on 2024/7/31.
//
#include "Decoder.hpp"
uint OpValue(Opcode opcode){
    switch (opcode) {
        case Opcode::LUI:
            return 0x37;
        case Opcode::AUIPC:
            return 0x17;
        case Opcode::JAL:
            return 0x6F;
        case Opcode::JALR:
            return 0x67;
        case Opcode::BEQ:
            return 0x63;
        case Opcode::BNE:
            return 0x63;
        case Opcode::BLT:
            return 0x63;
        case Opcode::BGE:
            return 0x63;
        case Opcode::BLTU:
            return 0x63;
        case Opcode::BGEU:
            return 0x63;
        case Opcode::LB:
            return 0x03;
        case Opcode::LH:
            return 0x03;
        case Opcode::LW:
            return 0x03;
        case Opcode::LBU:
            return 0x03;
        case Opcode::LHU:
            return 0x03;
        case Opcode::SB:
            return 0x23;
        case Opcode::SH:
            return 0x23;
        case Opcode::SW:
            return 0x23;
        case Opcode::ADDI:
            return 0x13;
        case Opcode::SLTI:
            return 0x13;
        case Opcode::SLTIU:
            return 0x13;
        case Opcode::XORI:
            return 0x13;
        case Opcode::ORI:
            return 0x13;
        case Opcode::ANDI:
            return 0x13;
        case Opcode::SLLI:
            return 0x13;
        case Opcode::SRLI:
            return 0x13;
        case Opcode::SRAI:
            return 0x13;
        case Opcode::ADD:
            return 0x33;
        case Opcode::SUB:
            return 0x33;
        case Opcode::SLL:
            return 0x33;
        case Opcode::SLT:
            return 0x33;
        case Opcode::SLTU:
            return 0x33;
        case Opcode::XOR:
            return 0x33;
        case Opcode::SRL:
            return 0x33;
        case Opcode::SRA:
            return 0x33;
        case Opcode::OR:
            return 0x33;
        case Opcode::AND:
            return 0x33;
        default:
            return -1; // Unknown opcode
    }
}