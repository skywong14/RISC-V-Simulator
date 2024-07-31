//
// Created by skywa on 2024/7/26.
//

#ifndef RISC_V_SIMULATOR_DECODER_HPP
#define RISC_V_SIMULATOR_DECODER_HPP
#pragma once
#include <bitset>
#include <iostream>
#include <cstring>
using uint = unsigned int;

enum class Opcode {
    LUI, AUIPC, JAL, JALR,
    BEQ, BNE, BLT, BGE,
    BLTU, BGEU, LB, LH,
    LW, LBU, LHU, SB,
    SH, SW, ADDI, SLTI,
    SLTIU, XORI, ORI, ANDI,
    SLLI, SRLI, SRAI, ADD,
    SUB, SLL, SLT, SLTU,
    XOR, SRL, SRA, OR,
    AND, UNKNOWN
};
uint OpValue(Opcode opcode);
std::string toString(Opcode opcode);

class Instruction{
private:
    uint instructionNumber;
public:
    Opcode opcode;
    uint rd;
    uint funct3, funct7;
    uint rs1, rs2;
    uint imm;
    Instruction(): Instruction(0) {}
    Instruction(uint input) {
        instructionNumber = input;
        parser();
    }
    void parser(){
        uint opcodeNum = instructionNumber & 0x7F; //0-6
        rd = (instructionNumber >> 7) & 0x1F;  //7-11
        funct3 = (instructionNumber >> 12) & 0x7; //12-14
        rs1 = (instructionNumber >> 15) & 0x1F; //15-19
        rs2 = (instructionNumber >> 20) & 0x1F; //20-24
        funct7 = (instructionNumber >> 25) & 0x7F; //25-31

        switch (opcodeNum) {
            case 0x37:
                // LUI
                opcode = Opcode::LUI;
                imm = instructionNumber & 0xFFFFF000;
                break;
            case 0x17:
                // AUIPC
                opcode = Opcode::AUIPC;
                imm = instructionNumber & 0xFFFFF000;
                break;
            case 0x6F:
                // JAL
                opcode = Opcode::JAL;
                imm = ((instructionNumber >> 21) & 0x3FF) << 1 | // imm[10:1]
                      ((instructionNumber >> 20) & 0x1) << 11 | // imm[11]
                      ((instructionNumber >> 12) & 0xFF) << 12 | // imm[19:12]
                      ((instructionNumber >> 31) << 20); // imm[20]
                if (imm & 0x100000) imm |= 0xFFE00000; // sign extend
                break;
            case 0x67:
                // JALR
                opcode = Opcode::JALR;
                imm = (instructionNumber >> 20) & 0xFFF;
                if (imm & 0x800) imm |= 0xFFFFF000; // sign extend
                break;
            case 0x63:
                // BEQ, BNE, BLT, BGE, BLTU, BGEU
                switch (funct3) {
                    case 0b000:
                        opcode = Opcode::BEQ;
                        break;
                    case 0b001:
                        opcode = Opcode::BNE;
                        break;
                    case 0b100:
                        opcode = Opcode::BLT;
                        break;
                    case 0b101:
                        opcode = Opcode::BGE;
                        break;
                    case 0b110:
                        opcode = Opcode::BLTU;
                        break;
                    case 0b111:
                        opcode = Opcode::BGEU;
                        break;
                    default:
                        throw std::runtime_error("Unknown funct3 for branch instruction");
                }
                imm = ((instructionNumber >> 8) & 0xF) << 1 | // imm[4:1]
                      ((instructionNumber >> 25) << 5) | // imm[10:5]
                      ((instructionNumber >> 7) & 0x1) << 11 | // imm[11]
                      ((instructionNumber >> 31) << 12); // imm[12]
                if (imm & 0x1000) imm |= 0xFFFFE000; // sign extend
                break;
            case 0x03:
                // LB, LH, LW, LBU, LHU
                switch (funct3) {
                    case 0b000:
                        opcode = Opcode::LB;
                        break;
                    case 0b001:
                        opcode = Opcode::LH;
                        break;
                    case 0b010:
                        opcode = Opcode::LW;
                        break;
                    case 0b100:
                        opcode = Opcode::LBU;
                        break;
                    case 0b101:
                        opcode = Opcode::LHU;
                        break;
                    default:
                        throw std::runtime_error("Unknown funct3 for load instruction");
                }
                imm = (instructionNumber >> 20) & 0xFFF;
                if (imm & 0x800) imm |= 0xFFFFF000; // sign extend (if imm < 0)
                break;
            case 0x23:
                // SB, SH, SW
                switch (funct3) {
                    case 0b000:
                        opcode = Opcode::SB;
                        break;
                    case 0b001:
                        opcode = Opcode::SH;
                        break;
                    case 0b010:
                        opcode = Opcode::SW;
                        break;
                    default:
                        throw std::runtime_error("Unknown funct3 for store instruction");
                }
                imm = ((instructionNumber >> 7) & 0x1F) | // imm[4:0]
                      ((instructionNumber >> 25) << 5); // imm[11:5]
                if (imm & 0x800) imm |= 0xFFFFF000; // sign extend
                break;
            case 0x13:
                // ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
                switch (funct3) {
                    case 0b000:
                        opcode = Opcode::ADDI;
                        break;
                    case 0b010:
                        opcode = Opcode::SLTI;
                        break;
                    case 0b011:
                        opcode = Opcode::SLTIU;
                        break;
                    case 0b100:
                        opcode = Opcode::XORI;
                        break;
                    case 0b110:
                        opcode = Opcode::ORI;
                        break;
                    case 0b111:
                        opcode = Opcode::ANDI;
                        break;
                    case 0b001:
                        opcode = Opcode::SLLI;
                        break;
                    case 0b101:
                        // SRLI and SRAI are differentiated by the 7th bit of funct7
                        if ((funct7 & 0b0100000) == 0b0100000) {
                            opcode = Opcode::SRAI;
                        } else {
                            opcode = Opcode::SRLI;
                        }
                        break;
                    default:
                        throw std::runtime_error("Unknown funct3 for immediate instruction");
                }
                imm = (instructionNumber >> 20) & 0xFFF;
                if (imm & 0x800) imm |= 0xFFFFF000; // sign extend  (if imm < 0)
                break;
            case 0x33:
                // ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
                switch (funct3) {
                    case 0b000:
                        // ADD and SUB
                        if ((funct7 & 0b0100000) == 0b0100000) {
                            opcode = Opcode::SUB;
                        } else {
                            opcode = Opcode::ADD;
                        }
                        break;
                    case 0b001:
                        opcode = Opcode::SLL;
                        break;
                    case 0b010:
                        opcode = Opcode::SLT;
                        break;
                    case 0b011:
                        opcode = Opcode::SLTU;
                        break;
                    case 0b100:
                        opcode = Opcode::XOR;
                        break;
                    case 0b101:
                        // SRL and SRA
                        if ((funct7 & 0b0100000) == 0b0100000) {
                            opcode = Opcode::SRA;
                        } else {
                            opcode = Opcode::SRL;
                        }
                        break;
                    case 0b110:
                        opcode = Opcode::OR;
                        break;
                    case 0b111:
                        opcode = Opcode::AND;
                        break;
                    default:
                        throw std::runtime_error("Unknown funct3 for register instruction");
                }
                imm = 0;
                break;
            default:
                opcode = Opcode::UNKNOWN;
                break;
        }
    }
    void input(uint input){
        instructionNumber = input;
        parser();
    }
    void debug(uint curPC = 0) {
        std::cout << "Instruction: " << std::bitset<32>(instructionNumber) << std::endl;
        std::cout << "    PC: 0x"  << std::hex << curPC << std::endl << std::dec;
        std::cout << "    Opcode: " << toString(opcode)  << std::endl;
        std::cout << "    rd: " << std::bitset<5>(rd) << std::endl;
        std::cout << "    funct3: " << std::bitset<3>(funct3) << std::endl;
        std::cout << "    rs1: " << std::bitset<5>(rs1) << std::endl;
        std::cout << "    rs2: " << std::bitset<5>(rs2) << std::endl;
        std::cout << "    funct7: " << std::bitset<7>(funct7) << std::endl;
        std::cout << "    imm: " << std::bitset<32>(imm) << std::endl;
    }
};

/*
class Decoder{
public:
    Instruction instruction;
    Decoder() = default;
    void input(uint input) { instruction.input(input); }
    void decode(){
    }
    void Run(){

    }
    void tick(){
        Run();
    }
};
*/

#endif //RISC_V_SIMULATOR_DECODER_HPP
