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
    AND
};
uint OpValue(Opcode opcode) {
    switch (opcode) {
        case Opcode::LUI: return 0x37;
        case Opcode::AUIPC: return 0x17;
        case Opcode::JAL: return 0x6F;
        case Opcode::JALR: return 0x67;
        case Opcode::BEQ: return 0x63;
        case Opcode::BNE: return 0x63;
        case Opcode::BLT: return 0x63;
        case Opcode::BGE: return 0x63;
        case Opcode::BLTU: return 0x63;
        case Opcode::BGEU: return 0x63;
        case Opcode::LB: return 0x03;
        case Opcode::LH: return 0x03;
        case Opcode::LW: return 0x03;
        case Opcode::LBU: return 0x03;
        case Opcode::LHU: return 0x03;
        case Opcode::SB: return 0x23;
        case Opcode::SH: return 0x23;
        case Opcode::SW: return 0x23;
        case Opcode::ADDI: return 0x13;
        case Opcode::SLTI: return 0x13;
        case Opcode::SLTIU: return 0x13;
        case Opcode::XORI: return 0x13;
        case Opcode::ORI: return 0x13;
        case Opcode::ANDI: return 0x13;
        case Opcode::SLLI: return 0x13;
        case Opcode::SRLI: return 0x13;
        case Opcode::SRAI: return 0x13;
        case Opcode::ADD: return 0x33;
        case Opcode::SUB: return 0x33;
        case Opcode::SLL: return 0x33;
        case Opcode::SLT: return 0x33;
        case Opcode::SLTU: return 0x33;
        case Opcode::XOR: return 0x33;
        case Opcode::SRL: return 0x33;
        case Opcode::SRA: return 0x33;
        case Opcode::OR: return 0x33;
        case Opcode::AND: return 0x33;
        default: return -1; // Unknown opcode
    }
}
/*
 * 根据给出的每个指令的opcode，以下是它们对应的目标地址类型：

1. **LUI (0x37)**：将立即数左移 12 位后存放在目标寄存器中。
2. **AUIPC (0x17)**：将当前 PC 加上立即数左移 12 位后存放在目标寄存器中。
3. **JAL (0x6F)**：将当前地址加上偏移量存放在目标寄存器中，并跳转到新的地址执行。
4. **JALR (0x67)**：将当前地址加上立即数后存放在目标寄存器中，并跳转到新的地址执行。
5. **BEQ, BNE, BLT, BGE, BLTU, BGEU (0x63)**：这些条件分支指令的目标地址是跳转到的目标地址（PC相对寻址）。
6. **LB, LH, LW, LBU, LHU (0x03)**：从内存中加载数据，目标地址是存放加载数据的目标寄存器。
7. **SB, SH, SW (0x23)**：将寄存器中的数据存储到内存中，目标地址是存储的内存地址。
8. **ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI (0x13)**：对一个寄存器和一个立即数进行操作，目标地址是存放操作结果的寄存器。
9. **ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND (0x33)**：对两个寄存器中的数据进行操作，目标地址是存放操作结果的寄存器。

这些目标地址描述了每个指令在执行后数据存放的位置，是RISC-V指令集中基本的执行规则。
 */
struct Destination{
    int type; // 0: register 1:memory with imm(+0) 2: memory with imm + reg[rs1]
    uint imm, rs1;
    Destination():type(0), imm(0), rs1(0) {}
};

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
                imm = ((instructionNumber >> 21) & 0x3FF) | ((instructionNumber >> 20) & 0x1) << 10 | ((instructionNumber >> 12) & 0xFF) << 11 | ((instructionNumber >> 31) << 20);
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
                imm = ((instructionNumber >> 8) & 0xF) | ((instructionNumber >> 25) << 5) | ((instructionNumber >> 7) & 0x1) << 11 | ((instructionNumber >> 31) << 12);
                if (imm & 0x1000) imm |= 0xFFFFE000; // sign extend (if imm < 0)
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
                imm = ((instructionNumber >> 7) & 0x1F) | ((instructionNumber >> 25) << 5);
                if (imm & 0x800) imm |= 0xFFFFF000; // sign extend (if imm < 0)
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
                throw std::runtime_error("Unknown Opcode");
                break;
        }
    }
    void input(uint input){
        instructionNumber = input;
        parser();
    }
    void debug() {
        std::cout << "---------------" << std::endl;
        std::cout << "Opcode: " << std::bitset<7>(static_cast<uint32_t>(opcode)) << std::endl;
        std::cout << "rd: " << std::bitset<5>(rd) << std::endl;
        std::cout << "funct3: " << std::bitset<3>(funct3) << std::endl;
        std::cout << "rs1: " << std::bitset<5>(rs1) << std::endl;
        std::cout << "rs2: " << std::bitset<5>(rs2) << std::endl;
        std::cout << "funct7: " << std::bitset<7>(funct7) << std::endl;
        std::cout << "imm: " << std::bitset<32>(imm) << std::endl;
        std::cout << "---------------" << std::endl;
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
