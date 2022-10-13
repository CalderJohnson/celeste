#include <stdio.h>
#include <vm.h>
#include <instructions.h>
#include <interrupts.h>
#include <disk.h>
#include <screen.h>
#include <clock.h>

/* update flags that can be updated given the most recent output of the ALU (sign, zero, parity) */
static void update_flags (const int32_t result) {
    if (result >> 0x1F) {
        registers[R_FLAGS] |= FL_SIGN;
    }
    else {
        registers[R_FLAGS] &= ~FL_SIGN;
    }
    if (result % 2 == 0) {
        registers[R_FLAGS] |= FL_PARITY;
        if (result == 0) {
            registers[R_FLAGS] |= FL_ZERO;
        }
    }
    else {
        registers[R_FLAGS] &= ~(FL_PARITY | FL_ZERO);
    }
    if (result > 0) {
        registers[R_FLAGS] |= FL_GREATER;
    }
    else {
        registers[R_FLAGS] &= ~FL_GREATER;
    }
}

/* NO OPERATION operation */
static void op_nop () {
    return; //no operation
}

/* HALT operation */
static void op_hlt () {
    celeste.halted = true; //hang until next external interrupt
    return;
}

/* INTERRUPT operation */
static void op_int (const uint32_t instruction) {
    const uint8_t interrupt_code = (instruction >> 0x8) & 0xFF;
    call_interrupt(interrupt_code);
    return;
}

/* LOAD EFFECTIVE ADDRESS operation */
static void op_lea (const uint8_t spec, const uint32_t instruction) {
    const uint16_t addr = instruction & 0xFFFF;
    const uint8_t reg = (instruction >> 0x10) & 0xF;
    switch (spec) {
        case 0x0: //lea to register
            registers[reg] = addr;
            break;
        case 0x1: //lea to memory
            memory[registers[reg]] = addr;
            break;
    }
    return;
}

/* MOVE operation */
static void op_mov (const uint8_t spec, const uint32_t instruction) {
    const uint16_t data = instruction & 0xFFFF; //addr/immv
    const uint8_t reg_1 = (instruction >> 0x10) & 0xF; //destination register (except when moving to an address)
    const uint8_t reg_2 = (instruction >> 0xC) & 0xF; //source register
    switch (spec) {
        case 0x0: //src -> dest
            registers[reg_1] = registers[reg_2];
            break;
        case 0x1: //addr pointed to by src -> dest
            registers[reg_1] = memory[registers[reg_2]];
            break;
        case 0x2: //src -> addr pointed to by dest
            memory[registers[reg_1]] = registers[reg_2];
            break;
        case 0x3: //addr pointed to by src -> addr pointed to by dest
            memory[registers[reg_1]] = memory[registers[reg_2]];
            break;
        case 0x4: //data at addr -> dest
            registers[reg_1] = memory[data];
            break;
        case 0x5: //data at addr -> addr pointed to by dest
            memory[registers[reg_1]] = memory[data];
            break;
        case 0x6: //immv -> dest
            registers[reg_1] = data;
            break;
        case 0x7: //immv -> addr pointed to by dest
            memory[registers[reg_1]] = data;
            break;
        case 0x8: //reg_1 -> addr
            memory[data] = registers[reg_1];
            break;
        case 0x9: //addr pointed to by reg_1 -> addr
            memory[data] = memory[registers[reg_1]];
            break;
    }
    return;
}

/* PUSH operation */
static void op_push (const uint8_t spec, const uint32_t instruction) {
    const uint16_t data = instruction & 0xFFFF;
    const uint8_t reg = (instruction >> 0x10) & 0xF;
    switch (spec) {
        case 0x0: //push register
            memory[--registers[R_SP]] = registers[reg];
            break;
        case 0x1: //push contents of addr pointed to by register
            memory[--registers[R_SP]] = memory[registers[reg]];
            break;
        case 0x2: //push contents of addr
            memory[--registers[R_SP]] = memory[data];
            break;
        case 0x3: //push contents of immv
            memory[--registers[R_SP]] = data;
            break;
    }
    return;
}

/* POP operation */
static void op_pop (const uint8_t spec, const uint32_t instruction) {
    const uint16_t data = instruction & 0xFFFF;
    const uint8_t reg = (instruction >> 0x10) & 0xF;
    switch (spec) {
        case 0x0: //pop to register
            registers[reg] = memory[registers[R_SP]++];
            break;
        case 0x1: //pop to addr pointed to by register
            memory[registers[reg]] = memory[registers[R_SP]++];
            break;
        case 0x2: //pop to addr
            memory[data] = memory[registers[R_SP]++];
            break;
    }
    return;
}

/* JUMP operation */
static void op_jmp (const uint8_t spec, const uint32_t instruction) {
    const uint16_t addr = instruction & 0xFFFF;
    const uint8_t reg = (instruction >> 0x10) & 0xF;
    switch(spec) {
        case 0x0: //jump to address in register
            registers[R_IP] = registers[reg];
            break;
        case 0x1: //jump to address pointed to by register
            registers[R_IP] = memory[registers[reg]];
            break;
        case 0x2: //jump to addr
            registers[R_IP] = addr;
            break;
    }
    return;
}

/* COMPARE operation */
static void op_cmp (const uint8_t spec, const uint32_t instruction) {
    int32_t result;
    const uint16_t data = instruction & 0xFFFF;
    const uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    const uint8_t reg_2 = (instruction >> 0xC) & 0xF;
    switch(spec) {
        case 0x0: //comparing 2 registers
            result = registers[reg_1] - registers[reg_2];
            break;
        case 0x1: //comparing 1 register and 1 location in memory
            result = registers[reg_1] - memory[data];
            break;
        case 0x2: //comparing 1 register and 1 immv
            result = registers[reg_1] - data;
            break;
    }
    update_flags(result);
    return;
}

/* JUMP IF ZERO operation */
static void op_jz (const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & FL_ZERO) {
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF CARRY operation */
static void op_jc(const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & FL_CARRY) {
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF SIGN operation */
static void op_js (const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & FL_SIGN) {
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF PARITY operation */
static void op_jp (const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & FL_PARITY) {
        op_jmp(spec, instruction);
    }
    return;
}


/* JUMP IF EQUAL operation */
static void op_je (const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & FL_ZERO) { //after a cmp, zero flag is set if results are equal
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF NOT EQUAL operation */
static void op_jne (const uint8_t spec, const uint32_t instruction) {
    if (!(registers[R_FLAGS] & FL_ZERO)) { //after a cmp, zero flag is set if results are equal
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF GREATER THAN operation */
static void op_jg (const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & FL_GREATER) {
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF GREATER THAN OR EQUAL TO operation */
static void op_jge (const uint8_t spec, const uint32_t instruction) {
    if (registers[R_FLAGS] & (FL_ZERO | FL_GREATER)) {
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF LESS THAN operation */
static void op_jl (const uint8_t spec, const uint32_t instruction) {
    if (!(registers[R_FLAGS] & (FL_ZERO | FL_GREATER))) {
        op_jmp(spec, instruction);
    }
    return;
}

/* JUMP IF LESS THAN OR EQUAL TO operation */
static void op_jle (const uint8_t spec, const uint32_t instruction) {
    if (!(registers[R_FLAGS] & FL_GREATER)) {
        op_jmp(spec, instruction);
    }
    return;
}

/* CALL TO FUNCTION operation */
static void op_call (const uint8_t spec, const uint32_t instruction) {
    memory[--registers[R_SP]] = registers[R_IP]; //push return address
    op_jmp(spec, instruction);
    return;
}

/* RETURN FROM FUNCTION operation */
static void op_ret () {
    registers[R_IP] = memory[registers[R_SP]++]; //assumes function prologue/epilogue was correctly implemented
    return;
}

/* INCREMENT operation */
static void op_inc (const uint8_t spec, const uint32_t instruction) {
    uint16_t addr = instruction & 0xFFFF;
    uint8_t reg = (instruction >> 0x10) & 0xF;
    switch (spec) {
        case 0x0: //register
            registers[reg]++;
            break;
        case 0x1: //address in register
            memory[registers[reg]]++;
            break;
        case 0x2: //address
            memory[addr]++;
            break;
    }
    return;
}

/* DECREMENT operation */
static void op_dec (const uint8_t spec, const uint32_t instruction) {
    uint16_t addr = instruction & 0xFFFF;
    uint8_t reg = (instruction >> 0x10) & 0xF;
    switch (spec) {
        case 0x0: //register
            registers[reg]--;
            break;
        case 0x1: //address in register
            memory[registers[reg]]--;
            break;
        case 0x2: //address
            memory[addr]--;
            break;
    }
    return;
}

/* ADD operation */
static void op_add (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] + registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] += registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] += memory[data];
            break;
        case 0x3:
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* SUBTRACT operation */
static void op_sub (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] - registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] -= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] -= memory[data];
            break;
        case 0x3:
            registers[reg_1] -= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* MULTIPLY operation */
static void op_mul (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] * registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] *= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] *= memory[data];
            break;
        case 0x3:
            registers[reg_1] *= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* DIVIDE operation */
static void op_div (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            if (registers[reg_3] == 0) {
                call_trap(TRAP_DIV0);
                return;
            }
            registers[reg_1] = registers[reg_2] / registers[reg_3];
            break;
        case 0x1:
            if (registers[reg_2] == 0) {
                call_trap(TRAP_DIV0);
                return;
            }
            registers[reg_1] /= registers[reg_2];
            break;
        case 0x2:
            if (memory[data] == 0) {
                call_trap(TRAP_DIV0);
                return;
            }
            registers[reg_1] /= memory[data];
            break;
        case 0x3:
            if (data == 0) {
                call_trap(TRAP_DIV0);
                return;
            }
            registers[reg_1] /= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* AND operation */
static void op_and (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] & registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] &= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] &= memory[data];
            break;
        case 0x3:
            registers[reg_1] &= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* OR operation */
static void op_or (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] | registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] |= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] |= memory[data];
            break;
        case 0x3:
            registers[reg_1] |= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* XOR operation */
static void op_xor (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] ^ registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] ^= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] ^= memory[data];
            break;
        case 0x3:
            registers[reg_1] ^= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* NOT operation */
static void op_not (const uint8_t spec, const uint32_t instruction) {
    uint16_t addr = instruction & 0xFFFF;
    uint8_t reg = (instruction >> 0x10) & 0xF;
    switch (spec) {
        case 0x0: //register
            registers[reg] = ~registers[reg];
            break;
        case 0x1: //address in register
            memory[registers[reg]] = ~memory[registers[reg]];
            break;
        case 0x2: //address
            memory[addr] = ~memory[addr];
            break;
    }
    return;
}

/* SHIFT LEFT operation */
static void op_shl (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] << registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] <<= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] <<= memory[data];
            break;
        case 0x3:
            registers[reg_1] <<= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* SHIFT RIGHT operation */
static void op_shr (const uint8_t spec, const uint32_t instruction) {
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = registers[reg_2] >> registers[reg_3];
            break;
        case 0x1:
            registers[reg_1] >>= registers[reg_2];
            break;
        case 0x2:
            registers[reg_1] >>= memory[data];
            break;
        case 0x3:
            registers[reg_1] >>= data;
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* ROTATE LEFT operation */
static void op_rol (const uint8_t spec, const uint32_t instruction) {
    /* rotate left */
    #define ROL(n, d) (n << d) | (n >> (32 - d))

    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = ROL(registers[reg_2], registers[reg_3]);
            break;
        case 0x1:
            registers[reg_1] = ROL(registers[reg_1], registers[reg_2]);
            break;
        case 0x2:
            registers[reg_1] = ROL(registers[reg_1], memory[data]);
            break;
        case 0x3:
            registers[reg_1] = ROL(registers[reg_1], data);
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* ROTATE RIGHT operation */
static void op_ror (const uint8_t spec, const uint32_t instruction) {
    /* rotate right */
    #define ROR(n, d)(n >> d) | (n << (32 - d))
    
    uint16_t data = instruction & 0xFFFF;
    uint8_t reg_1 = (instruction >> 0x10) & 0xF;
    uint8_t reg_2 = (instruction >> 0xC)  & 0xF;
    uint8_t reg_3 = (instruction >> 0x8)  & 0xF;
    switch (spec) {
        case 0x0:
            registers[reg_1] = ROR(registers[reg_2], registers[reg_3]);
            break;
        case 0x1:
            registers[reg_1] = ROR(registers[reg_1], registers[reg_2]);
            break;
        case 0x2:
            registers[reg_1] = ROR(registers[reg_1], memory[data]);
            break;
        case 0x3:
            registers[reg_1] = ROR(registers[reg_1], data);
            break;
    }
    update_flags(registers[reg_1]);
    return;
}

/* IO IN operation */
static void op_in (uint32_t instruction) {
    uint8_t port = (instruction >> 0x8) & 0xFF;
    uint8_t reg = (instruction >> 0x10) & 0xF;
    registers[reg] = memory[DEVICE_OFFSET + port];
}

/* IO OUT operation */
static void op_out (uint32_t instruction) {
    uint8_t port = (instruction >> 0x8) & 0xFF;
    uint8_t reg = (instruction >> 0x10) & 0xF;
    memory[DEVICE_OFFSET + port] = registers[reg];
}

/* one clock cycle */
static void step () {
    uint32_t instruction = memory[registers[R_IP]]; //must be cast to unsigned or left shift will be arithmetic
    registers[R_IP]++;                              //next instruction (control flow instructions will alter it before the next clock cycle)
    uint8_t opcode = instruction >> 0x18;
    uint8_t spec = (instruction >> 0x14) & 0xF;
    switch (opcode) {
        case OP_NOP:
            op_nop();
            break;
        case OP_HLT:
            op_hlt();
            break;
        case OP_INT:
            op_int(instruction);
            break;
        case OP_LEA:
            op_lea(spec, instruction);
            break;
        case OP_MOV:
            op_mov(spec, instruction);
            break;
        case OP_PUSH:
            op_push(spec, instruction);
            break;
        case OP_POP:
            op_pop(spec, instruction);
            break;
        case OP_JMP:
            op_jmp(spec, instruction);
            break;
        case OP_CMP:
            op_cmp(spec, instruction);
            break;
        case OP_JZ:
            op_jz(spec, instruction);
            break;
        case OP_JC:
            op_jc(spec, instruction);
            break;
        case OP_JS:
            op_js(spec, instruction);
            break;
        case OP_JP:
            op_jp(spec, instruction);
            break;
        case OP_JE:
            op_je(spec, instruction);
            break;
        case OP_JNE:
            op_jne(spec, instruction);
            break;
        case OP_JG:
            op_jg(spec, instruction);
            break;
        case OP_JGE:
            op_jge(spec, instruction);
          //todo implement  break;
        case OP_JL:
            op_jl(spec, instruction);
            break;
        case OP_JLE:
            op_jle(spec, instruction);
            break;
        case OP_CALL:
            op_call(spec, instruction);
            break;
        case OP_RET:
            op_ret();
            break;
        case OP_INC:
            op_inc(spec, instruction);
            break;
        case OP_DEC:
            op_dec(spec, instruction);
            break;
        case OP_ADD:
            op_add(spec, instruction);
            break;
        case OP_SUB:
            op_sub(spec, instruction);
            break;
        case OP_MUL:
            op_mul(spec, instruction);
            break;
        case OP_DIV:
            op_div(spec, instruction);
            break;
        case OP_AND:
            op_and(spec, instruction);
            break;
        case OP_OR:
            op_or(spec, instruction);
            break;
        case OP_XOR:
            op_xor(spec, instruction);
            break;
        case OP_NOT:
            op_not(spec, instruction);
            break;
        case OP_SHL:
            op_shl(spec, instruction);
            break;
        case OP_SHR:
            op_shr(spec, instruction);
            break;
        case OP_ROL:
            op_rol(spec, instruction);
            break;
        case OP_ROR:
            op_ror(spec, instruction);
            break;
        case OP_IN:
            op_in(instruction);
            break;
        case OP_OUT:
        	op_out(instruction);
            break;
        default:
            call_trap(TRAP_INVOP);
    }
    return;
}

/* start the machine */
void run (char* diskfile) {
    if(!init_disk(diskfile)) return;
    if(!init_screen()) return;
    init_clock();
    load_program(); /* will be replaced by proper firmware */

    registers[R_FLAGS] = FLAGS_INIT;
    registers[R_IP] = 0x0;

    while (!celeste.halted) {
        step();
        while(!next_clock(CLOCK_SPEED)) {/* hold */}
    }
}
