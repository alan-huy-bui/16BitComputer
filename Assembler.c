#include <stdio.h>

#define MaxProgramSize 65538        // TODO: Develop Memory Map and Change Digit


/**
 * Variable Declarations
 */
unsigned short int assembledProgram[MaxProgramSize];


/**
 * Function Declarations
 */
unsigned short int* assemble(unsigned short int* textProgram, int programLength);


/**
 * Converts Text Program to Machine Code
 * 
 * Inputs: Pointer to Text Program Array, Program Length
 * Outputs: Pointer to Assembled Program Array
 */
unsigned short int* assemble(unsigned short int* textProgram, int programLength) {

    //  TODO: Change to Actual Assembler
    unsigned short int temp[6] = {
        0x5801,     //  MOV R0, #1
        0x590F,     //  MOV R1, #15
        0x4080,     //  STR R1, [R0]
        0x3100,     //  LDR R2, [R0]
        0x6C28,     //  AND R4, R1, R2
        0xF000      //  HALT
    };

    for (int i = 0; i < sizeof(temp); i++) {
        assembledProgram[i] = temp[i];
    }

    return &assembledProgram[0];
}
