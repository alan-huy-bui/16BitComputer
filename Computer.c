#include <stdio.h>
#include <stdbool.h>

//  Special Registers
#define SP  6
#define LR  7

//  Special Memory Locations
#define ROM 0x0000
#define SB  0xFFFF

//  Condition Code Flags
#define N   0b100
#define Z   0b010
#define P   0b001


/**
 * Variable Declarations: Simulated Registers and Memory
 */
static unsigned short int registers[8];
static unsigned short int memory[65538];
static unsigned short int instructionRegister;
static unsigned short int programCounter;
static unsigned short int programStatusRegister;

static bool executionEnable;


/**
 * Function Declarations
 */
void initializeComputer(unsigned short int* programPointer, int programSize);
static void runComputer();
static void serviceInstruction(unsigned short int instruction);
static void setFlags();
static void getProgramStatus();


/**
 * Initializes the Computer by
 *      Writing Assembled Machine Code to ROM,
 *      Initializing Registers to their Appropriate Value,
 *      Beginning Execution of Each Instruction
 * 
 * Inputs:  Pointer to Assembled Program, Program Size
 * Outputs: None
 */
void initializeComputer(unsigned short int* programPointer, int programSize) {

    //  Write Assembled Program to ROM
    for (int i = 0; i < programSize; i++) {
        memory[ROM + i] = *(programPointer + i);
    }

    //  Initialize All Registers to 0x0000
    for (int i = 0; i < 8; i++) {
        registers[i] = 0;
    }

    //  Initialize Stack Pointer (SP) to Stack Base (SB)
    registers[SP] = SB;

    //  Initialize Program Counter (PC) with First Instruction Address (Beginning of ROM)
    programCounter = ROM;

    //  Initialize Flags to Z in Program Status Register (PSR)
    programStatusRegister = (1 << 14);

    //  Begin Running Computer
    runComputer();

}



/**
 * Runs Program by Retrieving Next Instruction, Placing it into IR, and Servicing the Instruction
 * 
 * Inputs: None
 * Outputs: None
 */
static void runComputer(void) {

    int hardCodeStop = 0;

    executionEnable = true;

    while ((hardCodeStop < 10) & (executionEnable)) {
        instructionRegister = memory[programCounter];

        printf("\nInstruction: 0x");
        printf("%X", instructionRegister);
        printf("\n");

        serviceInstruction(instructionRegister);
        getProgramStatus();

        hardCodeStop++;
    }

}



/**
 * Services Instruction Given By:
 *      Modifying Registers, and/or
 *      Reading/Writing to Memory, and/or
 *      Setting Condition Code Bits, and/or
 *      Incrementing or Modifying PSR
 * 
 * Inputs:  16-bit Instruction from Instruction Register (IR)
 * Outputs: None
 */
static void serviceInstruction(unsigned short int instruction) {

    //  Retreieve Opcode
    unsigned short int opcode = instruction >> 12;

    //  Retrieve Extension
    unsigned short int ext1b = (instruction >> 11) & 0x0001;
    unsigned short int ext2b = (instruction >> 10) & 0x0002;

    //  Retrieve Flags
    unsigned int flags = (programStatusRegister >> 13) & 0x0007;

    //  Define Possible Instruction Parameters
    unsigned int destinationRegister = instruction & 0x0007;
    unsigned int sourceRegister = (instruction >> 3) & 0x0007;
    unsigned int source2Register = (instruction >> 6) & 0x0007;
    unsigned int imm12 = instruction & 0x0FFF;
    unsigned int imm11 = instruction & 0x07FF;
    unsigned int imm9 = (instruction >> 3) & 0x01FF;
    unsigned int imm8 = (instruction >> 3) & 0x00FF;
    unsigned int imm7 = (instruction >> 3) & 0x007F;
    unsigned int imm4 = (instruction >> 6) & 0x000F;


    //  Main Logic for Servicing Instructions
    switch(opcode) {

        /**
         * Instruction:
         * Unconditional Branch (BRR)
         */
        case(0x0):
            programCounter = imm12;

            break;


        /**
         * Instruction:
         * Conditional Branch with Equality (BEQ / BNE)
         */
        case(0x1):
            switch(ext1b) {
                case(0b0):
                    if (flags == Z) {
                        programCounter = imm11;
                    }
                    break;

                case(0b1):
                    if (flags != Z) {
                        programCounter = imm11;
                    }
                    break;
            }

            break;


        /**
         * Instruction:
         * Conditional Branch with Comparison (BGT / BLT)
         */
        case(0x2):
            switch(ext1b) {
                case(0b0):
                    if (flags == P) {
                        programCounter = imm11;
                    }
                    break;

                case(0b1):
                    if (flags == N) {
                        programCounter = imm11;
                    }
                    break;    
            }

            break;


        /**
         * Instruction:
         * Load Register (LDR)
         */
        case(0x3):
            programCounter++;

            switch(ext2b) {
                case(0b00):
                    registers[destinationRegister] = memory[registers[sourceRegister]];
                    break;

                case(0b01):
                    registers[destinationRegister] = memory[registers[sourceRegister] + registers[source2Register]];
                    break;

                case(0b10):
                    registers[destinationRegister] = memory[registers[sourceRegister] + imm4];
                    break;

                case(0b11):
                    registers[destinationRegister] = imm12;
                    break;
            }

            break;


        /**
         * Instruction:
         * Store Register (STR)
         */
        case(0x4):
            programCounter++;

            switch(ext2b) {
                case(0b00):
                    memory[registers[destinationRegister]] = registers[sourceRegister];
                    break;

                case(0b01):
                    memory[registers[destinationRegister] + registers[source2Register]] = registers[sourceRegister];
                    break;

                case(0b10):
                    memory[registers[destinationRegister] + imm4] = registers[sourceRegister];
                    break;
            }

            break;


        /**
         * Instruction:
         * Move (MOV)
         */
        case(0x5):
            programCounter++;

            switch(ext1b) {
                case(0b0):
                    registers[destinationRegister] = registers[sourceRegister];
                    break;

                case(0b1):
                    registers[destinationRegister] = imm8;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Bitwise Logical And (AND)
         */
        case(0x6):
            programCounter++;

            switch(ext2b) {
                case(0b00):
                    registers[destinationRegister] = registers[destinationRegister] & registers[sourceRegister];
                    break;

                case(0b01):
                    registers[destinationRegister] = registers[destinationRegister] & imm7;
                    break;

                case(0b10):
                    registers[destinationRegister] = registers[sourceRegister] & registers[source2Register];
                    break;

                case(0b11):
                    registers[destinationRegister] = registers[sourceRegister] & imm4;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Bitwise Logical Or (OR)
         */
        case(0x7):
            programCounter++;

            switch(ext2b) {
                case(0b00):
                    registers[destinationRegister] = registers[destinationRegister] | registers[sourceRegister];
                    break;

                case(0b01):
                    registers[destinationRegister] = registers[destinationRegister] | imm7;
                    break;

                case(0b10):
                    registers[destinationRegister] = registers[sourceRegister] | registers[source2Register];
                    break;

                case(0b11):
                    registers[destinationRegister] = registers[sourceRegister] | imm4;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Bitwise Logical Not (NOT)
         */
        case(0x8):
            programCounter++;

            registers[destinationRegister] = ~registers[destinationRegister];

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Addition (ADD)
         */
        case(0x9):
            programCounter;

            switch(ext2b) {
                case(0b00):
                    registers[destinationRegister] = registers[destinationRegister] + registers[sourceRegister];
                    break;

                case(0b01):
                    registers[destinationRegister] = registers[destinationRegister] + imm7;
                    break;

                case(0b10):
                    registers[destinationRegister] = registers[sourceRegister] + registers[source2Register];
                    break;

                case(0b11):
                    registers[destinationRegister] = registers[sourceRegister] + imm4;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Subtraction (SUB)
         */
        case(0xA):
            programCounter;

            switch(ext2b) {
                case(0b00):
                    registers[destinationRegister] = registers[destinationRegister] - registers[sourceRegister];
                    break;

                case(0b01):
                    registers[destinationRegister] = registers[destinationRegister] - imm7;
                    break;

                case(0b10):
                    registers[destinationRegister] = registers[sourceRegister] - registers[source2Register];
                    break;

                case(0b11):
                    registers[destinationRegister] = registers[sourceRegister] - imm4;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Compare (CMP)
         */
        case(0xB):
            programCounter++;

            int difference;

            switch(ext1b) {
                case(0b0):
                    difference = registers[destinationRegister] - registers[sourceRegister];
                    break;

                case(0b1):
                    difference = registers[destinationRegister] - imm8;
                    break;
            }

            setFlags(difference);
            break;


        /**
         * Instruction:
         * Push Register onto Stack (PSH)
         */
        case(0xC):
            programCounter++;

            registers[SP]--;
            memory[registers[SP]] = registers[sourceRegister];

            break;


        /**
         * Instruction:
         * Pop off Stack into Register (POP)
         */
        case(0xD):
            programCounter++;

            registers[destinationRegister] = memory[registers[SP]];
            registers[SP]++;

            break;


        /**
         * Instruction:
         * Jump (JMP)
         */
        case(0xE):
            programCounter = memory[registers[destinationRegister] + imm9];
            break;


        /**
         * Instruction:
         * Halt Program Execution (HLT)
         */
        case(0xF):
            programCounter++;
            
            executionEnable = false;

            break;
    }
}



/**
 * Sets N, Z, P Flags in Program Status Register (PSR)
 * 
 * Inputs:  Register Number of Register Modified in Previous Instruction
 * Outputs: None
 */
static void setFlags(int modifiedRegister) {

    //  Clear Current Flags
    programStatusRegister &= 0x1FFF;

    //  Sets Flags According to Modified Register
    if (registers[modifiedRegister] < 0) {
        programStatusRegister |= (N << 13);
    }
    else if (registers[modifiedRegister] > 0) {
        programStatusRegister |= (P << 13);
    }
    else {
        programStatusRegister |= (Z << 13);
    }

}



/**
 * Helper Function that Prints Program Registers
 * 
 * Inputs:  None
 * Outputs: None
 */
static void getProgramStatus(void) {

    printf("====================\n");
    printf("  Program Status\n");
    printf("    Registers:\n");

    for (int i = 0; i < 6; i++) {

        printf("      R");
        printf("%d", i);
        printf(":  0x");
        printf("%X", registers[i]);
        printf("\n");

    }

    printf("      SP:  0x");
    printf("%X", registers[SP]);
    printf("\n");

    printf("      LR:  0x");
    printf("%X", registers[LR]);
    printf("\n");

    printf("      PSR: 0x");
    printf("%X", programStatusRegister);
    printf("\n");

    printf("      PC:  0x");
    printf("%X", programCounter);
    printf("\n");

    printf("====================\n\n");

}
