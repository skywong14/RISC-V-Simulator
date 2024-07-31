//
// Created by skywa on 2024/7/31.
//
#include "InstructionQueue.hpp"

InstructionQueue::InstructionQueue(ReservationStation &rs_, RoB &rob_, LSB &lsb_, BranchPredictor &bp_): rs(rs_), rob(rob_), lsb(lsb_), bp(bp_) {
    head = 0; tail = 0;
    PC = 0;
    for (int i = 0; i < queueSize; i++){
        busy[i] = false;
    }
    tickRegister();
}


uint InstructionQueue::readPC()  {
    return PC;
}

void InstructionQueue::writePC(uint value) {
    PC = value;
}

void InstructionQueue::tickRegister() {
    head.tick(); tail.tick(); PC.tick();
    for (int i = 0; i < queueSize; i++) {
        busy[i].tick();
        instructions[i].tick();
        curPC[i].tick();
    }
}

void InstructionQueue::flush() {
    head = 0; tail = 0;
    PC = 0;
    for (int i = 0; i < queueSize; i++)
        busy[i] = false;
    tickRegister();
}

void InstructionQueue::newInstruction() {
    if (!busy[tail]) {
        busy[tail] = true;
        instructions[tail] = lsb.loadInstruction(PC);
        curPC[tail] = PC;
        PC = PC + 4;
        tail = (tail + 1) % queueSize;
    }
}

void InstructionQueue::executeInstruction(){
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
        if (instruction.opcode == Opcode::SB) {
            robEntry = rob.insertEntry(RoBType::STOREB, 0, instruction.rd, PC);
        } else if (instruction.opcode == Opcode::SH) {
            robEntry = rob.insertEntry(RoBType::STOREH, 0, instruction.rd, PC);
        } else if (instruction.opcode == Opcode::SW) {
            robEntry = rob.insertEntry(RoBType::STOREW, 0, instruction.rd, PC);
        }
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
        case Opcode::ADDI:
            rs.insertEntry(CalcType::Add, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::SLTI:
            rs.insertEntry(CalcType::Less, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::SLTIU:
            rs.insertEntry(CalcType::LessUnsigned, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::XORI:
            rs.insertEntry(CalcType::Xor, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::ORI:
            rs.insertEntry(CalcType::Or, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::ANDI:
            rs.insertEntry(CalcType::And, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::SLLI:
            rs.insertEntry(CalcType::ShiftL, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::SRLI:
            rs.insertEntry(CalcType::ShiftR, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::SRAI:
            rs.insertEntry(CalcType::ShiftR_Arith, DataSource::RegNImm, instruction.rs1, instruction.imm, 0, robEntry);
            break;
        case Opcode::ADD:
            rs.insertEntry(CalcType::Add, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::SUB:
            rs.insertEntry(CalcType::Sub, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::SLL:
            rs.insertEntry(CalcType::ShiftL, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::SLT:
            rs.insertEntry(CalcType::Less, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::SLTU:
            rs.insertEntry(CalcType::LessUnsigned, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::XOR:
            rs.insertEntry(CalcType::Xor, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::SRL:
            rs.insertEntry(CalcType::ShiftR, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::SRA:
            rs.insertEntry(CalcType::ShiftR_Arith, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::OR:
            rs.insertEntry(CalcType::Or, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        case Opcode::AND:
            rs.insertEntry(CalcType::And, DataSource::TwoReg, instruction.rs1, instruction.rs2, 0, robEntry);
            break;
        default:
            throw std::invalid_argument("Unknown Opcode");
    }
}

void InstructionQueue::Run() {
    if (!busy[head]) return;
    executeInstruction();
    newInstruction();
}

void InstructionQueue::tick() {
    tickRegister();
    Run();
}

