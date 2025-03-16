#ifndef Assembler
#define Assembler

#define MaxProgramSize 65538        // TODO: Develop Memory Map and Change Digit

/**
 * Variable Declarations
 *
unsigned short int assembledProgram[MaxProgramSize];


/**
 * Function Declarations
 */
unsigned short int* assemble(unsigned short int* textProgram, int programLength);

#endif