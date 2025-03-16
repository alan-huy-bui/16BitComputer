#include <stdio.h>

//  Special Registers
#define SP  5
#define LR  6
#define PSR 7

//  Special Memory Locations
#define ROM 0x000
#define SB  0xFFFF


/**
 * Variable Declarations: Simulated Registers and Memory
 */
static unsigned short int registers[8];
static unsigned short int memory[65538];
static unsigned short int instructionRegister;


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

    //  Initialize Program Status Register (PSR) with First Instruction Address (Beginning of ROM)
    registers[PSR] = ROM;

    //  Initialize Flags to Z in Program Status Register (PSR)
    registers[PSR] |= (1 << 14);

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

    while (hardCodeStop < 10) {
        instructionRegister = memory[registers[PSR] & 0x1FFF];

        printf("\nInstruction: ");
        printf("%d", instructionRegister);
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

    //  Retrieve Flags
    unsigned int flags = (registers[PSR] >> 13) & 0x0007;

    //  Declare Possible Instruction Parameters as Local Variables
    unsigned int destinationRegister;
    unsigned int sourceRegister;
    unsigned int source2Register;
    unsigned int offsetRegister;
    unsigned int immediate;


    //  Main Logic for Servicing Instructions
    switch(opcode) {

        /**
         * Instruction:
         * Unconditional Branch (B)
         */
        case(0b0000):
            registers[PSR] &= 0x0FFF;
            immediate = instruction & 0x0FFF;
            registers[PSR] |= immediate;

            break;


        /**
         * Instruction:
         * Conditional Branch (BEQ / BNE)
         */
        case(0b0001):
            switch((instruction << 11) & 0x0001) {
                case(0b0):
                    if (flags == 0b010) {
                        registers[PSR] &= 0x0FFF;
                        immediate = instruction & 0x07FF;
                        registers[PSR] |= immediate;
                    }
                    break;

                case(0b1):
                    if (flags != 0b010) {
                        registers[PSR] &= 0x0FFF;
                        immediate = instruction & 0x07FF;
                        registers[PSR] |= immediate;
                    }
                    break;
            }

            break;


        /**
         * Instruction:
         * Conditional Branch (BGT / BLT)
         */
        case(0b0010):
            switch((instruction << 11) & 0x0001) {
                case(0b0):
                    if (flags == 0b001) {
                        registers[PSR] &= 0x0FFF;

                        immediate = instruction & 0x07FF;
                        registers[PSR] |= immediate;
                    }
                    break;

                case(0b1):
                    if (flags == 0b100) {
                        registers[PSR] &= 0x0FFF;

                        immediate = instruction & 0x07FF;
                        registers[PSR] |= immediate;
                    }
                    break;    
            }

            break;


        /**
         * Instruction:
         * Load Word (LDR)
         */
        case(0b0011):
            registers[PSR]++;

            destinationRegister = (instruction >> 7) & 0x7;
            sourceRegister = (instruction >> 4) & 0x7;
            offsetRegister = (instruction >> 1) & 0x7;
            immediate = instruction & 0xF;

            switch((instruction >> 10) & 0x3) {
                case(0b00):
                    registers[destinationRegister] = memory[registers[sourceRegister]];
                    break;

                case(0b01):
                    registers[destinationRegister] = memory[registers[sourceRegister] + registers[offsetRegister]];
                    break;

                case(0b10):
                    registers[destinationRegister] = memory[registers[sourceRegister] + immediate];
                    break;

                case(0b11):
                    registers[destinationRegister] = instruction & 0x007F;
                    break;
            }

            break;


        /**
         * Instruction:
         * Store Word (STR)
         */
        case(0b0100):
            registers[PSR]++;

            destinationRegister = (instruction >> 4) & 0x0007;
            sourceRegister = (instruction >> 7) & 0x0007;
            offsetRegister = (instruction >> 1) & 0x0007;
            immediate = instruction & 0x000F;

            switch((instruction >> 10) & 0x0003) {
                case(0b00):
                    memory[registers[destinationRegister]] = registers[sourceRegister];
                    break;

                case(0b01):
                    memory[registers[destinationRegister] + registers[offsetRegister]] = registers[sourceRegister];
                    break;

                case(0b10):
                    memory[registers[destinationRegister] + immediate] = registers[sourceRegister];
                    break;
            }

            break;


        /**
         * Instruction:
         * Move Word (MOV)
         */
        case(0b0101):
            registers[PSR]++;

            destinationRegister = (instruction >> 8) & 0x0007;
            sourceRegister = (instruction >> 5) & 0x0007;
            immediate = instruction & 0x00FF;

            switch((instruction >> 11) & 0x0001) {
                case(0b0):
                    registers[destinationRegister] = registers[sourceRegister];
                    break;

                case(0b1):
                    registers[destinationRegister] = immediate;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Logical And (AND)
         */
        case(0b0110):
            registers[PSR]++;

            destinationRegister = (instruction >> 8) & 0x0007;
            sourceRegister = (instruction >> 5) & 0x0007;
            source2Register = (instruction >> 2) & 0x0007;

            switch((instruction >> 11) & 0x1) {
                case(0b0):
                    registers[destinationRegister] = registers[destinationRegister] & registers[sourceRegister];
                    break;

                case(0b1):
                    registers[destinationRegister] = registers[sourceRegister] & registers[source2Register];
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Logical Or (OR)
         */
        case(0b0111):
            registers[PSR]++;

            destinationRegister = (instruction >> 8) & 0x0007;
            sourceRegister = (instruction >> 5) & 0x0007;
            source2Register = (instruction >> 2) & 0x0007;

            switch((instruction >> 11) & 0x1) {
                case(0b0):
                    registers[destinationRegister] = registers[destinationRegister] | registers[sourceRegister];
                    break;

                case(0b1):
                    registers[destinationRegister] = registers[sourceRegister] | registers[source2Register];
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Logical Not (NOT)
         */
        case(0b1000):
            registers[PSR]++;

            destinationRegister = (instruction >> 8) & 0x7;
            registers[destinationRegister] = ~registers[destinationRegister];

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Add (ADD)
         */
        case(0b1001):
            registers[PSR]++;

            destinationRegister = (instruction >> 8) & 0x0007;
            sourceRegister = (instruction >> 5) & 0x0007;
            source2Register = (instruction >> 2) & 0x0007;
            immediate = instruction & 0x001F;

            switch((instruction >> 11) & 0x0001) {
                case(0b0):
                    registers[destinationRegister] = registers[sourceRegister] - registers[source2Register];
                    break;

                case(0b1):
                    registers[destinationRegister] = registers[sourceRegister] - immediate;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Subtract (SUB)
         */
        case(0b1010):
            registers[PSR]++;

            destinationRegister = (instruction >> 8) & 0x0007;
            sourceRegister = (instruction >> 5) & 0x0007;
            source2Register = (instruction >> 2) & 0x0007;
            immediate = instruction & 0x001F;

            switch((instruction >> 11) & 0x0001) {
                case(0b0):
                    registers[destinationRegister] = registers[sourceRegister] + registers[source2Register];
                    break;

                case(0b1):
                    registers[destinationRegister] = registers[sourceRegister] - immediate;
                    break;
            }

            setFlags(destinationRegister);
            break;


        /**
         * Instruction:
         * Compare (CMP)
         */
        case(0b1011):
            registers[PSR]++;

            sourceRegister = (instruction >> 8) & 0x0007;
            source2Register = (instruction >> 5) & 0x0007;
            immediate = (instruction) & 0x00FF;

            int temp;

            switch((instruction >> 11) & 0x0001) {
                case(0b0):
                    temp = registers[sourceRegister] - registers[source2Register];
                    break;

                case(0b1):
                    temp = registers[sourceRegister] - immediate;
                    break;
            }

            setFlags(temp);
            break;


        /**
         * Instruction:
         * Push Register onto Stack (PUSH)
         */
        case(0b1100):
            registers[PSR]++;

            sourceRegister = (instruction >> 9) & 0x0007;

            registers[SP]--;
            memory[registers[SP]] = registers[sourceRegister];

            break;


        /**
         * Instruction:
         * Pop off Stack into Register (POP)
         */
        case(0b1101):
            registers[PSR]++;

            destinationRegister = (instruction >> 9) & 0x0007;

            registers[destinationRegister] = memory[registers[SP]];
            registers[SP]++;

            break;


        /**
         * Instruction:
         * Reserved
         */
        case(0b1110):
            registers[PSR]++;
            break;


        /**
         * Instruction:
         * Halt Program (HALT)
         */
        case(0b1111):
            registers[PSR]++;
            break;

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
    registers[PSR] &= 0x1FFF;

    //  Sets Flags According to Modified Register
    if (registers[modifiedRegister] < 0) {
        registers[PSR] |= (1 << 15);
    }
    else if (registers[modifiedRegister] > 0) {
        registers[PSR] |= (1 << 13);
    }
    else {
        registers[PSR] |= (1 << 12);
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

    for (int i = 0; i < 5; i++) {

        printf("      R");
        printf("%d", i);
        printf(": ");
        printf("%d", registers[i]);
        printf("\n");

    }

    printf("      SP: ");
    printf("%d", registers[SP]);
    printf("\n");

    printf("      LR: ");
    printf("%d", registers[LR]);
    printf("\n");

    printf("      PSR: ");
    printf("%d", registers[PSR]);
    printf("\n");

    printf("====================\n\n");

}
