#include "registers.h"
#include "memory.h"
#include <stdio.h>
#include <math.h>
#include <mem.h>
#include <stdlib.h>


void execAddR8toRM8(byte *memory,registers* reg){
    halfRegister *destinationRegister,*sourceRegister;
    if(LOGGER){
        printf("AddR8toRM8\n");
    }
    reg->ip++;
    destinationRegister = getRegister(getRMField(memory[reg->ip]),reg);
    sourceRegister = getRegister(getModifier(memory[reg->ip]),reg);
    memory[*destinationRegister] = (byte)(memory[*destinationRegister] + *sourceRegister);
    reg->ip++;
}
void execAdd(byte *memory,registers* reg){
    halfRegister *destinationRegister,*sourceRegister;
    if(LOGGER){
        printf("Add\n");
    }
    reg->ip+=1;
    destinationRegister = getRegister(getRMField(memory[reg->ip]),reg);
    sourceRegister = getRegister(getModifier(memory[reg->ip]),reg);
    *destinationRegister = *sourceRegister + *destinationRegister;
    reg->ip+=1;
}
void execXor(byte *memory,registers* reg){
    wholeRegister *destinationRegister,*sourceRegister;
    if(LOGGER){
        printf("Xor\n");
    }
    if(reg->operandOverride){
        reg->operandOverride = 0;
        reg->ip+=1;
        destinationRegister = (wholeRegister*)getRegister(getRMField(memory[reg->ip]),reg);
        sourceRegister = (wholeRegister*)getRegister(getModifier(memory[reg->ip]),reg);
        *destinationRegister = *sourceRegister ^ *destinationRegister;
        reg->ip+=1;
    }else{
        printf("Xor without operand override index was not implemented");
        exit(-1);
    }
}
void execIncrementEDX(byte *memory,registers* reg){
    if(LOGGER){
        printf("IncrementEDX\n");
    }
    if(reg->operandOverride){
        reg->operandOverride = 0;
        reg->edx = reg->edx+1;
    }else{
        printf("Increment EDX without prefix was not implemented");
        exit(-1);
    }

    reg->ip++;
}
void execIncrementEBX(byte *memory,registers* reg){
    if(LOGGER){
        printf("IncrementEBX\n");
    }
    if(reg->operandOverride){
        reg->operandOverride = 0;
        reg->ebx = reg->ebx+1;
    }else{
        printf("Increment EBX without prefix was not implemented");
        exit(-1);
    }
    reg->ip++;
}
void execDecrementECX(byte *memory,registers* reg){
    if(LOGGER){
        printf("DecrementECX\n");
    }
    if(reg->operandOverride){
        reg->operandOverride = 0;
        reg->ecx = reg->ecx - 1;
    }else{
        reg->cx -= 1;
    }
    reg->ip++;
}
void execJumpNotEqual(byte *memory,registers* reg){
    if(LOGGER){
        printf("JumpNotEqual\n");
    }
    reg->ip++;
    if(reg->isEqual){
        reg->ip++;
    }else{
        reg->ip++;
    }
}
void execJumpNotParity(byte *memory,registers* reg){
    if(LOGGER){
        printf("JumpNotParity\n");
    }
    reg->ip++;
}
void execCompare(byte *memory,registers* reg){
    halfRegister *destinationRegister;
    if(LOGGER){
        printf("Compare\n");
    }
    reg->ip++;
    destinationRegister = getRegister(getRMField(memory[reg->ip]),reg);
    if(*destinationRegister == memory[reg->ip+1]){
        reg->isEqual = 1;
    }else{
        reg->isEqual = 0;
    }
    reg->ip += 2;
}
void execMoveToR8(byte *memory,registers* reg){
    halfRegister *destinationRegister, addressInMemory, *otherRegister;
    if(LOGGER){
        printf("MoveToR8\n");
    }
    reg->ip++;
    if(getAdressingMode(memory[reg->ip])==0){
        destinationRegister = getRegister(getModifier(memory[reg->ip]),reg);
        otherRegister = getRegister(getRMField(memory[reg->ip]),reg);
        addressInMemory = 0;
        if(!(reg->operandOverride || reg->segmentOverride || reg->addressOverride)) {
            addressInMemory = (halfRegister) (memory[reg->ip + 1] + memory[reg->ip + 2] * 0x100);
            reg->ip += 3;
        }else if(reg->addressOverride){
            reg->addressOverride = 0;
            if(otherRegister == &reg->sp){ /* SIB */
                /* 43  mov al, byte ptr es:[esi+eax] */
                if (memory[reg->ip+1] == 0x06){
                    addressInMemory = reg->ax + reg->si;
                    reg->ip += 2;
                }else{
                    printf("Unimplemeted SIB mode for moveToR8\n");
                    exit(-1);
                }

            }else{

                /* 42  mov al, byte ptr ds:[edx]*/
                addressInMemory = *otherRegister;
                reg->ip++;
            }
        }

        if(reg->segmentOverride) {
            printf("seg\n");
            reg->segmentOverride = 0;
            addressInMemory += reg->es;
        }else {
            addressInMemory += reg->ds;
        }
        *destinationRegister = memory[addressInMemory];
    }else{
        printf("Unimplemented Addressing mode\n");
        exit(-1);
    }
    /* muze byt jine*/
}
void execMoveFromSegment(byte *memory,registers* reg){
    halfRegister *destinationRegister,*segmentRegister;
    if(LOGGER){
        printf("MoveFromSegment\n");
    }
    reg->ip+=1;
    if(getAdressingMode(memory[reg->ip])==3){
        destinationRegister = getRegister(getRMField(memory[reg->ip]),reg);
        segmentRegister = getSegment(getModifier(memory[reg->ip]),reg);
        *destinationRegister = *segmentRegister;
    }else{
        printf("Unimplemented Addressing mode\n");
        exit(-1);
    }
    reg->ip+=1;
}
void execMoveToSegment(byte *memory,registers* reg){
    halfRegister *sourceRegister,*segmentRegister;
    if(LOGGER){
        printf("MoveToSegment\n");
    }
    reg->ip+=1;
    if(getAdressingMode(memory[reg->ip])==3){
        sourceRegister = getRegister(getRMField(memory[reg->ip]),reg);
        segmentRegister = getSegment(getModifier(memory[reg->ip]),reg);
        *segmentRegister = *sourceRegister;
    }else{
        printf("Unimplemented Addressing mode\n");
        exit(-1);
    }
    reg->ip+=1;
}
void execMoveAH(byte *memory,registers* reg){
    if(LOGGER){
        printf("MoveAH\n");
    }
    reg->ip++;
    reg->ah = memory[reg->ip++];
    reg->ip+=2;
}
void execMoveAX(byte *memory,registers* reg){
    if(LOGGER){
        printf("MoveAX\n");
    }
    reg->ip++;
    reg->ax = memory[reg->ip++];
    reg->ax += memory[reg->ip++]*0x100;
}
void execMoveDX(byte *memory,registers* reg){
    if(LOGGER){
        printf("MoveDX\n");
    }
    reg->ip++;
    reg->dx = memory[reg->ip++];
    reg->dx += memory[reg->ip++]*0x100;
}
void execMoveBX(byte *memory,registers* reg){
    if(LOGGER){
        printf("MoveBX\n");
    }
    reg->ip++;
    reg->bx = memory[reg->ip++];
    reg->bx += memory[reg->ip++]*0x100;
}
void execMoveSI(byte *memory,registers* reg){
    if(LOGGER){
        printf("MoveSI\n");
    }
    reg->ip++;
    reg->si = memory[reg->ip++];
    reg->si += memory[reg->ip++]*0x100;
}
void execMoveDI(byte *memory,registers* reg){
    if(LOGGER){
        printf("MoveDI\n");
    }
    reg->ip++;
    reg->di = memory[reg->ip++];
    reg->di += memory[reg->ip++]*0x100;
}
void execMoveIMM16toRM16(byte *memory,registers* reg){
    halfRegister *destinationRegister, addressInMemory, value;
    if(LOGGER){
        printf("MoveIMM16toRM16\n");
    }
    reg->ip++;
    if(getAdressingMode(memory[reg->ip])==2){
        destinationRegister = getRegister(getRMField(memory[reg->ip]),reg);
        addressInMemory = (halfRegister)(reg->ds + *destinationRegister + memory[reg->ip+1] + memory[reg->ip+2]*0x100);
        value = (halfRegister)(memory[reg->ip+3] + memory[reg->ip+4]*0x100);
        memory[addressInMemory] = (byte)value;
        reg->ip += 5;
    }else{
        printf("Unimplemeted Addressing mode when writing to memory\n");
    }
}
int execInterrupt(byte *memory,registers* reg){
    if(LOGGER){
        printf("Interrupt\n");
    }
    reg->ip++;
    if(memory[reg->ip] == 0x10){
        if(LOGGER){
            printf("->Setting screen mode\n");
        }
        memset(memory+displayPointer,0,80*25*2);
        reg->ip++;
        return 10;
    }else if(memory[reg->ip] == 0x20){
        if(LOGGER){
            printf("->Quitting\n");
        }
        int i;
        byte ch,color;
        for(i=0; i< 80*25*4; i++){
            if(i%80 == 0){printf("|\n");};
            ch = memory[displayPointer + i];
            if(ch>0x20 && ch<0xff){
                printf("%c",ch);
            }else{
                printf(" ");
            }
            color = memory[displayPointer + 2*i+1];
        }
        return 20;
    }else if(memory[reg->ip] == 0x21){
        int i;
        halfRegister addressInMemory;
        byte ch,color;
        for (i=0; i< 80*25*2; i++){
            addressInMemory = (halfRegister)(reg->dx +i);
            ch =  memory[addressInMemory];
            if (ch == 0x24){
                break;
            }
            printf("%c (%x) ",ch, ch);
            memory[displayPointer + 2 * i] = ch;
        }
        return 21;
    }else{
        return -1;
    }
}
void execJump(byte *memory,registers* reg){
    if(LOGGER){
        printf("Jump\n");
    }
    reg->ip++;
    /*tady je ten podelany skok o -1, ma sezrat 2 byty ale vysledne jen sebe*/
}
void execIncrement(byte *memory,registers* reg){
    wholeRegister * destinationRegister;
    if(LOGGER){
        printf("Increment\n");
    }
    reg->ip++;
    destinationRegister = (wholeRegister*)getRegister(getRMField(memory[reg->ip]),reg);
    *destinationRegister = *destinationRegister+1;
    reg->ip++;
}

void printMemory(byte *memory, int memSize) {
    int i;
    printf("MEMORY DUMP: %d bytes\n",memSize);
    for (i = codeOffset; i < (codeOffset+memSize); i++) {
        printf("%02X", memory[i]);
    }
    printf("\n");
}

void printByte(byte b){
    int i;

    printf("%02X\n", b);

    for(i=0;i<8;i++){
        printf("%i", b & 0x01);
        b = b >> 1;
        if(i==2){
            printf(" ");
        }
        if(i==5){
            printf(" ");
        }
    }
    printf("\n", b);
}

int getRMField(byte addrMode){  /*first lower*/
    return addrMode&7;
}

int getAdressingMode(byte addrMode){ /*second middle*/
    return addrMode>> 6;
}

int getModifier(byte addrMode){
    return (addrMode>>3)&7;
}

halfRegister* getRegister(int correctPartOfAddrMode, registers* reg){
    switch (correctPartOfAddrMode)
    {
        case 0: return &reg->ax;
        case 1: return &reg->cx;
        case 2: return &reg->dx;
        case 3: return &reg->bx;
        case 4: return &reg->sp;
        case 5: return &reg->bp;
        case 6: return &reg->si;
        case 7: return &reg->di;
        default:
            printf("Unknown destination register\n");
            exit(-1);
    }

}

halfRegister* getSegment(int correctPartOfAddrMode, registers* reg){
    switch (correctPartOfAddrMode)
    {
        case 0: return &reg->es;
        case 1: return &reg->cs;
        case 3: return &reg->ds;
        default:
            printf("Unknown segment register\n");
            exit(-1);
    }
}