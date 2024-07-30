//
// Created by skywa on 2024/7/29.
//

#ifndef RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
#define RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
#include "../Common/Register.hpp"
#include "../Common/Memory.hpp"
#include "../Common/RegisterFile.hpp"
#include "ReorderBuffer.hpp"
#include "LSB.hpp"

using uint = unsigned int;

class InstructionQueue{
    static const int queueSize = 8;
    Register<uint> head, tail;
    Register<uint> instructions[queueSize];
    Register<uint> curPC[queueSize];
    Register<bool> busy[queueSize];
    Register<uint> PC;
    ReservationStation& rs;
    RoB& rob;
    LSB& lsb;
    BranchPredictor& bp;

public:
    InstructionQueue(ReservationStation& rs_, RoB& rob_, LSB& lsb_, BranchPredictor& bp_): rs(rs_), rob(rob_), lsb(lsb_), bp(bp_) {
        head = 0; tail = 0;
        PC = 0;
        for (int i = 0; i < queueSize; i++){
            busy[i] = false;
        }
        tickRegister();
    }

    uint readPC() {
        return PC;
    }
    void writePC(uint value) {
        PC = value;
    }

    void tickRegister(){
        head.tick(); tail.tick();
        for (int i = 0; i < queueSize; i++){
            busy[i].tick();
            instructions[i].tick();
        }
    }
    void flush(){
        head = 0;
        tail = 0;
        for (int i = 0; i < queueSize; i++)
            busy[i] = false;
        tickRegister();
    }
    void newInstruction(){
        if (!busy[tail]) {

            busy[tail] = true;
            instructions[tail] = lsb.loadInstruction(PC);
            curPC[tail] = PC;
            PC = PC + 4;
            tail = (tail + 1) % queueSize;
        }
    }
    void executeInstruction(){


        if (!(rob.available() && rs.available())) return; // wait

        uint robEntry;

        Instruction instruction(instructions[head]);
        if (OpValue(instruction.opcode) == 0x63) {
            // Predict Branch
            bool jumpFlag = bp.predict(PC);
            robEntry = rob.insertEntry(RoBType::BRANCH, jumpFlag, PC + instruction.imm, PC);
            if (jumpFlag) {
                PC = PC + instruction.imm;
            } else {
                PC = PC + 4;
            }
        } else if (OpValue(instruction.opcode) == 0x23){
            // Store
            robEntry = rob.insertEntry(RoBType::STORE, 0, instruction.rd, PC);
            PC = PC + 4;
        } else if (instruction.opcode == Opcode::JALR){
            robEntry = rob.insertEntry(RoBType::JALR, PC + 4, instruction.rd, PC);
            PC = PC + 4;
        } else if (instruction.opcode == Opcode::JAL){
            robEntry = rob.insertEntry(RoBType::REGISTER, PC + 4, instruction.rd, PC);
            PC = PC + toTwosComplement(instruction.imm);
        }  else {
            robEntry = rob.insertEntry(RoBType::REGISTER, 0, instruction.rd, PC);
            PC = PC + 4;
        }
        //todo
        switch (instruction.opcode) {
            case Opcode::LUI:
                rob.updateEntry(robEntry, static_cast<uint>(instruction.imm << 12));
                break;
            case Opcode::AUIPC:
                rob.updateEntry(robEntry, PC + static_cast<uint>(instruction.imm << 12));
                break;
            case Opcode::JAL:
                rob.updateEntry(robEntry);
                break;
            case Opcode::JALR:
                rob.updateEntry(robEntry);
                break;
            case Opcode::BEQ:
                rs.insertEntry(CalcType::Equal, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
                break;
            case Opcode::BNE:
                rs.insertEntry(CalcType::NotEqual, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
                break;
            case Opcode::BLT:
                rs.insertEntry(CalcType::Less, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
                break;
            case Opcode::BGE:
                rs.insertEntry(CalcType::GreaterEQ, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
                break;
            case Opcode::BLTU:
                rs.insertEntry(CalcType::LessUnsigned, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
                break;
            case Opcode::BGEU:
                rs.insertEntry(CalcType::GreaterEQUnsigned, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
                break;
            case Opcode::LB:
                rs.insertEntry(CalcType::LB, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
                break;
            case Opcode::LH:
                rs.insertEntry(CalcType::LH, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
                break;
            case Opcode::LW:
                rs.insertEntry(CalcType::LW, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
                break;
            case Opcode::LBU:
                rs.insertEntry(CalcType::LBU, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
                break;
            case Opcode::LHU:
                rs.insertEntry(CalcType::LHU, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
                break;
            case Opcode::SB:
                rs.insertEntry(CalcType::SB, DataSource::TwoRegNImm, instruction.rs2, instruction.rs1, instruction.imm, robEntry);
                break;
            case Opcode::SH:
                rs.insertEntry(CalcType::SH, DataSource::TwoRegNImm, instruction.rs2, instruction.rs1, instruction.imm, robEntry);
                break;
            case Opcode::SW:
                rs.insertEntry(CalcType::SW, DataSource::TwoRegNImm, instruction.rs2, instruction.rs1, instruction.imm, robEntry);
                break;
        }
    }

    void Run(){
        if (!busy[head]) return;
        executeInstruction();
        newInstruction();
    }
    void tick(){
        Run();
        tickRegister();
    }
};


#endif //RISC_V_SIMULATOR_INSTRUCTIONQUEUE_HPP
