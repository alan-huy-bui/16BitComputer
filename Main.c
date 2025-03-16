#include <stdio.h>
#include "Computer.h"
#include "Assembler.h"


int main() {

    //  Assemble Program
    unsigned short int* programPtr = assemble(0, 0);

    //  Run Program
    initializeComputer(programPtr, 6);

    return 0;
}
