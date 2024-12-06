#include <stdio.h>

#include <cassert>
#include <memoryapi.h>

#include "Instruction.h"
#include "Psx.h"

typedef int(__stdcall *FunctionPtr0)();
typedef int(__stdcall *FunctionPtr1)(int);
typedef int(__stdcall *FunctionPtr2)(int, int);
typedef int(__stdcall *FunctionPtr3)(int, int, int);
typedef int(__stdcall *FunctionPtr4)(int, int, int, int);
typedef int(__stdcall *FunctionPtr5)(int, int, int, int, int);
typedef int(__stdcall *FunctionPtr6)(int, int, int, int, int, int);
typedef int(__stdcall *FunctionPtr7)(int, int, int, int, int, int, int);
typedef int(__stdcall *FunctionPtr8)(int, int, int, int, int, int, int, int);
typedef int(__stdcall *FunctionPtr9)(int, int, int, int, int, int, int, int, int);
typedef int(__stdcall *FunctionPtr10)(int, int, int, int, int, int, int, int, int, int);

bool PSX::RunNextInstruction()
{
    // printf("PC: 0x%lx\n", this->pc);
    if (this->panic)
        return false;

    /* Workaround for exit */
    if (this->next_pc == 0 && this->pc > 0)
        return false;

    if (this->pc % 4 != 0)
    {
        this->Interrupt(LoadAddressError);
        return false;
    }

    // emulate branch delay slot: execute instruction, already fetch next
    // instruction at PC (IP)
    Instruction instruction = Instruction(this->Load32(this->pc));

    // if the last instruction was a branch, we're in the delay slot
    this->inDelaySlot = this->branching;
    this->branching = false;

    // Increment PC to point to the next instruction. (each is 32 bit)
    this->current_pc = this->pc;
    this->pc = this->next_pc;
    this->next_pc = this->next_pc + 4;

    // emulate branch delay slot: execute pending loads, if there are none, load
    // $zero which is NOP
    this->SetRegister(this->load.registerIndex, this->load.value);
    this->load = {{ZERO}, 0};  // reset load register

    // execute next instrudction
    this->DecodeAndExecute(instruction);

    // copy to actual registers
    for (int i = 0; i < NREGISTERS; ++i) regs[i] = out_regs[i];

    return true;
}

bool PSX::ProbeMemory(void *addr) const
{
    MEMORY_BASIC_INFORMATION mbi;

    if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0)
    {
        return false;
    }

    if (mbi.State != MEM_COMMIT || !(mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY)))
    {
        return false;  // Memory is either not committed or not writable
    }

    return true;
}

uint32_t PSX::Load32(const uint32_t &address) const
{
    if (ProbeMemory((void *)address))
        return *((uint32_t *)address);

    printf("LOAD32:STUB_Invalid_memory_access:0x%lx\n", address);
    return 0;
}

uint16_t PSX::Load16(const uint32_t &address) const
{
    if (ProbeMemory((void *)address))
        return *(uint16_t *)address;

    printf("LOAD16:STUB_Invalid_memory_access:0x%lx\n", address);
    return 0;
}

uint8_t PSX::Load8(const uint32_t &address) const
{
    if (ProbeMemory((void *)address))
        return *(BYTE *)address;

    printf("LOAD8:STUB_Invalid_memory_access:0x%lx\n", address);
    return 0;
}

void PSX::Store32(const uint32_t &address, const uint32_t &value)
{
    if (ProbeMemory((void *)address))
        *((uint32_t *)address) = value;
    else
        printf("STORE32:STUB_Invalid_memory_access:0x%lx\n", address);
}

void PSX::Store16(const uint32_t &address, const uint16_t &value)
{
    if (ProbeMemory((void *)address))
        *((uint16_t *)address) = value;
    else
        printf("STORE16:STUB_Invalid_memory_access:0x%lx\n", address);
}

void PSX::Store8(const uint32_t &address, const uint8_t &value)
{
    if (ProbeMemory((void *)address))
        *(BYTE *)address = value;
    else
        printf("STORE8:STUB_Invalid_memory_access:0x%lx\n", address);
}

void PSX::DecodeAndExecute(const Instruction &instruction)
{
    switch (instruction.function())
    {
        // http://mipsconverter.com/opcodes.html
        // http://problemkaputt.de/PSX-spx.htm#cpuspecifications
        case 0b000000:
            switch (instruction.subfunction())
            {
                case 0b000000: this->OP_SLL(instruction); break;
                case 0b000010: this->OP_SRL(instruction); break;
                case 0b000011: this->OP_SRA(instruction); break;
                case 0b000100: this->OP_SLLV(instruction); break;
                case 0b000110: this->OP_SRLV(instruction); break;
                case 0b000111: this->OP_SRAV(instruction); break;
                case 0b001000: this->OP_JR(instruction); break;
                case 0b001001: this->OP_JALR(instruction); break;
                case 0b001100: this->OP_SYSCALL(instruction); break;
                case 0b001101: this->OP_BREAK(instruction); break;
                case 0b010000: this->OP_MFHI(instruction); break;
                case 0b010001: this->OP_MTHI(instruction); break;
                case 0b010010: this->OP_MFLO(instruction); break;
                case 0b010011: this->OP_MTLO(instruction); break;
                case 0b011000: this->OP_MULT(instruction); break;
                case 0b011001: this->OP_MULTU(instruction); break;
                case 0b011010: this->OP_DIV(instruction); break;
                case 0b011011: this->OP_DIVU(instruction); break;
                case 0b100000: this->OP_ADD(instruction); break;
                case 0b100001: this->OP_ADDU(instruction); break;
                case 0b100010: this->OP_SUB(instruction); break;
                case 0b100011: this->OP_SUBU(instruction); break;
                case 0b100100: this->OP_AND(instruction); break;
                case 0b100101: this->OP_OR(instruction); break;
                case 0b100110: this->OP_XOR(instruction); break;
                case 0b100111: this->OP_NOR(instruction); break;
                case 0b101010: this->OP_SLT(instruction); break;
                case 0b101011: this->OP_SLTU(instruction); break;
                default: this->OP_ILLEGAL(instruction);
            }
            break;
        case 0b000001: this->OP_BXX(instruction); break;
        case 0b000010: this->OP_J(instruction); break;
        case 0b000011: this->OP_JAL(instruction); break;
        case 0b000100: this->OP_BEQ(instruction); break;
        case 0b000101: this->OP_BNE(instruction); break;
        case 0b000110: this->OP_BLEZ(instruction); break;
        case 0b000111: this->OP_BGTZ(instruction); break;
        case 0b001000: this->OP_ADDI(instruction); break;
        case 0b001001: this->OP_ADDIU(instruction); break;
        case 0b001010: this->OP_SLTI(instruction); break;
        case 0b001011: this->OP_SLTIU(instruction); break;
        case 0b001100: this->OP_ANDI(instruction); break;
        case 0b001101: this->OP_ORI(instruction); break;
        case 0b001110: this->OP_XORI(instruction); break;
        case 0b001111: this->OP_LUI(instruction); break;
        case 0b010000: this->OP_COP0(instruction); break;
        case 0b010001: this->OP_COP1(instruction); break;
        case 0b010010: this->OP_COP2(instruction); break;
        case 0b010011: this->OP_COP3(instruction); break;
        case 0b100000: this->OP_LB(instruction); break;
        case 0b100001: this->OP_LH(instruction); break;
        case 0b100010: this->OP_LWL(instruction); break;
        case 0b100011: this->OP_LW(instruction); break;
        case 0b100100: this->OP_LBU(instruction); break;
        case 0b100101: this->OP_LHU(instruction); break;
        case 0b100110: this->OP_LWR(instruction); break;
        case 0b101000: this->OP_SB(instruction); break;
        case 0b101001: this->OP_SH(instruction); break;
        case 0b101010: this->OP_SWL(instruction); break;
        case 0b101011: this->OP_SW(instruction); break;
        case 0b101110: this->OP_SWR(instruction); break;
        case 0b110000: this->OP_LWC0(instruction); break;
        case 0b110001: this->OP_LWC1(instruction); break;
        case 0b110010: this->OP_LWC2(instruction); break;
        case 0b110011: this->OP_LWC3(instruction); break;
        case 0b111000: this->OP_SWC0(instruction); break;
        case 0b111001: this->OP_SWC1(instruction); break;
        case 0b111010: this->OP_SWC2(instruction); break;
        case 0b111011: this->OP_SWC3(instruction); break;
        default: this->OP_ILLEGAL(instruction);
    }
}

uint32_t PSX::GetRegister(const int &i) const
{
    return this->regs[i];
}

uint32_t PSX::GetRegister(const RegisterIndex &t) const
{
    return this->regs[t.index];
}

void PSX::SetRegister(const RegisterIndex &t, const uint32_t &v)
{
    this->out_regs[ZERO] = 0;  // r0 is always zero
    this->out_regs[t.index] = v;
}

void PSX::Interrupt(const Exception &interrupt)
{
    this->next_pc = this->pc + 4;

    // Get SSN
    auto SSN = this->GetRegister(V0);
    switch (SSN)
    {
        case 47: /* HOST_CALL */ HostCall(); break;
        case 0: /* PRINT */
            printf("%s", (char *)this->GetRegister(A0));
            this->next_pc = this->GetRegister(RA);
            break;
        case 1: /* PRINTF_1 */
            // printf("%s: 0x%lx\n", (char *)this->GetRegister(A0), this->GetRegister(A1));
            printf((char *)this->GetRegister(A0), this->GetRegister(A1));
            this->next_pc = this->GetRegister(RA);
            break;
        case 2: /* PRINTF_2 */
            // printf("%s: 0x%lx, 0x%lx\n", (char *)this->GetRegister(A0), this->GetRegister(A1),
            // this->GetRegister(A2));
            printf((char *)this->GetRegister(A0), this->GetRegister(A1), this->GetRegister(A2));
            this->next_pc = this->GetRegister(RA);
            break;
        case 3: /* PRINTF_3 */
            // printf("%s: 0x%lx, 0x%lx, 0x%lx\n", (char *)this->GetRegister(A0), this->GetRegister(A1),
            // this->GetRegister(A2), this->GetRegister(A3)); this->DbgDumpMemory(this->GetRegister(A1), 30);
            printf((char *)this->GetRegister(A0), this->GetRegister(A1), this->GetRegister(A2), this->GetRegister(A3));
            this->next_pc = this->GetRegister(RA);
            break;
        case 4: /* PRINTF_4 */
        {
            auto stackArg = this->Load32(this->regs[SP] + 16);
            printf(
                (char *)this->GetRegister(A0), this->GetRegister(A1), this->GetRegister(A2), this->GetRegister(A3),
                stackArg
            );
            this->next_pc = this->GetRegister(RA);
            break;
        }
        default: printf("[!] Unhandled Syscall %i", SSN); Panic();
    }
}

void PSX::HostCall()
{
    // Load module and get function address
    auto dll = (char *)this->GetRegister(A0);
    auto function = (char *)this->GetRegister(A1);
    auto hModule = LoadLibraryA(dll);
    if (hModule == 0)
    {
        printf("Error resolving module: %s\n", dll);
        this->Panic();
    }
    auto pProc = GetProcAddress(hModule, function);
    if (pProc == 0)
    {
        printf("Error resolving function: %s\n", function);
        this->Panic();
    }
    auto nargs = this->GetRegister(A2);

    // printf("VM_host_call: %s->%s (%i args)\n", dll, function, nargs);

    // parse arguments and invoke function
    int ret = 0;

    if (nargs == 0)
    {
        /* No arguments */
        ret = ((FunctionPtr0)pProc)();
    }
    else
    {
        // the actual argument 1 (4) is still in a register
        int arg1 = GetRegister(A3);

        // the rest is on the stack
        auto stackpointer = this->regs[SP];
        // this->DbgDisplayStack();

        switch (nargs)
        {
            case 0: ret = ((FunctionPtr0)pProc)(); break;
            case 1: ret = ((FunctionPtr1)pProc)(arg1); break;
            case 2:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                ret = ((FunctionPtr2)pProc)(arg1, arg2);
                break;
            }
            case 3:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                ret = ((FunctionPtr3)pProc)(arg1, arg2, arg3);
                break;
            }
            case 4:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                ret = ((FunctionPtr4)pProc)(arg1, arg2, arg3, arg4);
                break;
            }
            case 5:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                auto arg5 = this->Load32(stackpointer + 28);
                ret = ((FunctionPtr5)pProc)(arg1, arg2, arg3, arg4, arg5);
                break;
            }
            case 6:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                auto arg5 = this->Load32(stackpointer + 28);
                auto arg6 = this->Load32(stackpointer + 32);
                ret = ((FunctionPtr6)pProc)(arg1, arg2, arg3, arg4, arg5, arg6);
                break;
            }
            case 7:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                auto arg5 = this->Load32(stackpointer + 28);
                auto arg6 = this->Load32(stackpointer + 32);
                auto arg7 = this->Load32(stackpointer + 36);
                ret = ((FunctionPtr7)pProc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
                break;
            }
            case 8:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                auto arg5 = this->Load32(stackpointer + 28);
                auto arg6 = this->Load32(stackpointer + 32);
                auto arg7 = this->Load32(stackpointer + 36);
                auto arg8 = this->Load32(stackpointer + 40);
                ret = ((FunctionPtr8)pProc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                break;
            }
            case 9:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                auto arg5 = this->Load32(stackpointer + 28);
                auto arg6 = this->Load32(stackpointer + 32);
                auto arg7 = this->Load32(stackpointer + 36);
                auto arg8 = this->Load32(stackpointer + 40);
                auto arg9 = this->Load32(stackpointer + 44);
                ret = ((FunctionPtr9)pProc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
                break;
            }
            case 10:
            {
                auto arg2 = this->Load32(stackpointer + 16);
                auto arg3 = this->Load32(stackpointer + 20);
                auto arg4 = this->Load32(stackpointer + 24);
                auto arg5 = this->Load32(stackpointer + 28);
                auto arg6 = this->Load32(stackpointer + 32);
                auto arg7 = this->Load32(stackpointer + 36);
                auto arg8 = this->Load32(stackpointer + 40);
                auto arg9 = this->Load32(stackpointer + 44);
                auto arg10 = this->Load32(stackpointer + 48);
                ret = ((FunctionPtr10)pProc)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                break;
            }
            default: printf("Narg %i not implemented\n", nargs); this->Panic();
        }
    }

    // save return value
    this->out_regs[V0] = ret;

    // this->DbgPrintRegisters();

    // jump to ReturnAddress next
    this->next_pc = this->GetRegister(RA);
}
