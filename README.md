
	== ARCHITECTURE ==============

	16-Bit (Thumb) RISC Architecture
	Heavily Inspired by ARM and LC-3 Architectures

	==============================	


	== MEMORY MAP ================
	
	0x0000-0x5FFF: ROM
	0x6000-0xBFFF: RAM
	0xC000-0xFFFF: Stack
	
	Addressability: 16-Bits
	Address Space: 2^16 = 65536 Locations
	Endianness: N/A
	
	==============================


	== REGISTERS =================
	
	R0		General Purpose
	R1		General Purpose
	R2		General Purpose
	R3		General Purpose
	R4		General Purpose
	R5/SP	Stack Pointer
	R6/LR	Link Register
	R7/PSR*	Program Status Register

	IR		Instruction Register

	As this architecture is heavy inspired by ARM Architectures, we will
	follow a miniature version of the ARM Architecture Procedure Call Standard (AAPCS),
	where R0 is our function input, and R0-R1 are our function outputs.
	R2-R4 must be kept the same through function calls.

	Note.
		R7/PSR contains both the Condition Code Flags and Program Counter,
		where R7 has N[15], Z[14], P[13], and PC[12:0]
	
	==============================
	
	
	== INSTRUCTION SET ===========
	
	OC/I	  PARAMETERS	   MACHINE CODE ([11:0])		       EXAMPLE ASSEMBLY CODE

	0000 B	  PC <- PC + imm12 imm12[11:0]                                 B Label
	
	0001 BEQ  PC <- PC + imm11 0[11], imm11[10:0]                          BEQ Label
             BNE  PC <- PC + imm11 1[11], imm11[10:0]                          BNE Label
	
	0010 BGT  PC <- PC + imm11 0[11], imm11[10:0]                          BGT Label
             BLT  PC <- PC + imm11 0[11], imm11[10:0]                          BLT Label
	
	0011 LDR  DR <- [SR]	   00[11:10], DR[9:7], SR[6:4], 0[3:0]	       LDR DR, [SR]
		  DR <- [SR, OR]   01[11:10], DR[9:7], SR[6:4], OR[3:1], 0[0]  LDR DR, [SR, OR]
		  DR <- [SR, imm4] 10[11:10], DR[9:7], SR[6:4], imm4[3:0]      LDR DR, [SR, #imm4]
		  DR <- =label	   11[11:10], DR[9:7], label[6:0]	       LDR DR, =Label
	
	0100 STR  SR -> [DR]	   00[11:10], SR[9:7], DR[6:4], 0[3:0]	       STR SR, [DR]
		  SR -> [DR, OR]   01[11:10], SR[9:7], DR[6:4], OR[3:1], 0[0]  STR SR, [DR, OR]
		  SR -> [DR, imm4] 10[11:10], SR[9:7], DR[6:4], imm4[3:0]      STR SR, [DR, #imm4]
	
	0101 MOV* DR <- SR	   0[11], DR[10:8], SR[7:5]		       MOV DR, SR
                  DR <- imm8	   1[11], DR[10:8], imm8[7:0]		       MOV DR, #imm8
	
	0110 AND* DR <- DR & SR	   0[11], DR[10:8], SR[7:5], 0[4,0]	       AND DR, SR
	    	  DR <- SR1 & SR2  1[11], DR[10:8], SR1[7:5], SR2[4:2], 0[1:0] AND DR, SR1, SR2
	
	0111 OR*  DR <- DR | SR	   0[11], DR[10:8], SR[7:5], 0[4,0]	       OR DR, SR
	    	  DR <- SR1 | SR2  1[11], DR[10:8], SR1[7:5], SR2[4:2], 0[1:0] OR DR, SR1, SR2
	
	1000 NOT* DR <- ~DR	   DR[11:9], 0[8:0]			       NOT DR
	
	1001 ADD* DR <- SR1 + SR2  0[11], DR[10:8], SR1[7:5], SR2[4:2], 0[1:0] ADD DR, SR1, SR2
                  DR <- SR + imm5  1[11], DR[10:8], SR[7:5], imm5[4:0]         ADD DR, SR, #imm5
	
	1010 SUB* DR <- SR1 - SR2  0[11], DR[10:8], SR1[7:5], SR2[4:2], 0[1:0] SUB DR, SR1, SR2
                  DR <- SR - imm5  1[11], DR[10:8], SR[7:5], imm5[4:0]         SUB DR, SR, #imm5
	
	1011 CMP* NZP(SR1, SR2)    0[11], SR1[10:8], SR2[7:5], 0[4:0]          CMP SR1, SR2
                  NZP(SR, imm8)    1[11], SR[10:8], imm8[7:0]                  CMP SR, #imm8
	
	1100 PUSH [SP] <- SR       SR[11:9], 0[8:0]                            PUSH {SR}
	
	1101 POP  DR <- [SP]       DR[11:9], 0[8:0]                            POP {DR}
	
	1110 --   --					--		       --
	
	1111 HALT --					0[11:0]		       HALT
	
	
	Note.
        *: Instruction Modifies Flags
		DR: Destination Register
		SR: Source Register (1 or 2)
		OR: Offset Register
	
	==============================
	
	
	== ABOUT ME ==================

	Alan Bui
	Department of Electrical and Computer Engineering
	The University of Texas at Austin
	
	Spring 2025

	==============================
