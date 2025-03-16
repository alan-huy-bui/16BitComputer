#include <stdio.h>
#include "Computer.h"
#include "Assembler.h"

int main() {

    unsigned short int* programPtr = assemble(0, 0);

    initializeComputer(programPtr, 6);

    return 0;
}