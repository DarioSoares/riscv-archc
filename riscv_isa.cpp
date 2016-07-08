/*************************************************
**************************************************
*	@file		riscv_isa.cpp
*	@author		Dário Dias
*	@email		dsd@cin.ufpe.br		
*	@version	0.20
*	@date		8 July 2016
*	@brief		The ArchC Risc-V functional model.
**************************************************
*************************************************/

#include <math.h>

#include  "riscv_isa.H"
#include  "riscv_isa_init.cpp"
#include  "riscv_bhv_macros.H"

//If you want debug information for this model, uncomment next line
//#define DEBUG_MODEL

#include "ac_debug_model.H"

//CSR addresses
#define CSR_MHARTID 0xf10


using namespace riscv_parms;

void ac_behavior(instruction){

	dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);
	ac_pc = ac_pc + 4;

};

void ac_behavior( R_type ){}
void ac_behavior( I_type ){}
void ac_behavior( S_type ){}
void ac_behavior( SB_type ){}
void ac_behavior( U_type ){}
void ac_behavior( UJ_type ){}

//TODO imm needs to be sign-extended

void ac_behavior(begin){

	dbg_printf("@@@ begin behavior @@@\n");
	for (int regNum = 0; regNum < 31; regNum ++){
    	RB[regNum] = 0;
	}

}

void ac_behavior(end){

	dbg_printf("@@@ end behavior @@@\n");

}

void ac_behavior(lui){

	dbg_printf("lui r%d, %#x\n", rd, imm);
	RB[rd] = imm << 12;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(auipc){

	dbg_printf("auipc r%d, %#x\n", rd, imm);
	unsigned int offset;
	offset = imm << 12;
	RB[rd] = offset + ac_pc - 4;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(jal){

	unsigned int offset = (imm20 << 19) | (imm19to12 << 11) | (imm11 << 10) | imm10to1;
	unsigned int addr;
	if (imm20 == 1)
		addr = ac_pc + ((offset << 1) | 0xFFF00000) - 4;
	else
		addr = ac_pc + (offset << 1) - 4;
	//shows the wrong value to imm	
	dbg_printf("jal r%d, %#x\n", rd, offset);
	if (rd != 0)
		RB[rd] = ac_pc;
	ac_pc = (ac_pc & 0xF0000000) | addr;
	dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr);
	dbg_printf("Return = %#x\n\n", RB[rd]);

}

void ac_behavior(jalr){

	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;
	dbg_printf("jalr r%d, r%d, %#x\n", rd, rs1, s_imm);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	unsigned int addr = RB[rs1] + s_imm;
	dbg_printf("addr = %#x\n", addr);
	addr = addr >> 1;
	addr = addr << 1;
	dbg_printf("addr = %#x\n", addr);
	if (rd != 0)
		RB[rd] = ac_pc;
	ac_pc = (ac_pc & 0xF0000000) | addr, 1;
	dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr);
	dbg_printf("Return = %#x\n\n", RB[rd]);

}


void ac_behavior(addi){
	
	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;
	dbg_printf("imm = %#x\n", s_imm);
	dbg_printf("addi r%d, r%d, %d\n", rd, rs1, s_imm);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	RB[rd] = RB[rs1] + s_imm;
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(slti){

	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;
	dbg_printf("slti r%d, r%d, %d\n", rd, rs1, s_imm);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	if( (ac_Sword) RB[rs1] < (ac_Sword) s_imm )
		RB[rd] = 1;
	else
		RB[rd] = 0;
	dbg_printf("Result = %#x\n\n", RB[rd]);
};

void ac_behavior(sltiu){

	
	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;
	dbg_printf("slti r%d, r%d, %d\n", rd, rs1, s_imm);
	if( (ac_Uword) RB[rs1] < (ac_Uword) s_imm )
		RB[rd] = 1;
	else
		RB[rd] = 0;
	dbg_printf("Result = %#x\n\n", RB[rd]);
};


void ac_behavior(sbreak){

	//Instruction not implemented
	dbg_printf("sbreak\n\n");
	exit(EXIT_FAILURE);

}

void ac_behavior(lw){
	
	int word;
	int offset = imm;
	if ((imm >> 11) == 1) {
		offset = offset | 0xFFFFF000;
	}
	dbg_printf("lw r%d, %d(r%d)\n", rd, offset, rs1);
	dbg_printf("Addr = %d\n", RB[rs1] + offset);
	RB[rd] = DM.read(RB[rs1] + offset);	
	dbg_printf("Result = %#x\n\n", RB[rd]);
	
}

void ac_behavior(lh){

	int offset = imm;
	short int half;
	if ((imm >> 11) == 1)
		offset = offset | 0xFFFFF000;
	dbg_printf("lh r%d, %d(r%d)\n", rd, offset, rs1);
	half = DM.read_half(RB[rs1] + offset);
	if ((half >> 15) == 1)
		RB[rd] = half | 0xFFFF0000;
	else
		RB[rd] = half;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(lhu){

	int offset = imm;
	if ((imm >> 11) == 1)
		offset = offset | 0xFFFFF000;
	dbg_printf("lhu r%d, %d(r%d)\n", rd, offset, rs1);
	RB[rd] = DM.read_half(RB[rs1] + offset);
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(lb){

	int offset = imm;
	char byte;
	if ((imm >> 11) == 1)
		offset = offset | 0xFFFFF000;
	dbg_printf("lb r%d, %d(r%d)\n", rd, offset, rs1);
	byte = DM.read_byte(RB[rs1] + offset);
	if ((byte >> 7) == 1)
		RB[rd] = byte | 0xFFFFFF00;
	else
		RB[rd] = byte;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(lbu){

	int offset = imm;
	if ((imm >> 11) == 1)
		offset = offset | 0xFFFFF000;
	dbg_printf("lbu r%d, %d(r%d)\n", rd, offset, rs1);
	RB[rd] = DM.read_byte(RB[rs1] + offset);
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(sw){

	int offset = (imm11to5 << 5) | imm4to0;
	if ((imm11to5 >> 6) == 1)
		offset = offset | 0xFFFFF000;
	dbg_printf("sw r%d, %d(r%d)\n", rs2, offset, rs1);
	DM.write(RB[rs1] + offset, RB[rs2]);
	dbg_printf("Addr = %d\n", RB[rs1] + offset);
	dbg_printf("Result = %#x\n\n", RB[rs2]);
	//Used only for debugging (write to address 0)
	if ((RB[rs1] + offset) == 0)	
		printf("##### Output = %d #####\n", RB[rs2]);

}

void ac_behavior(sb){

	int offset = (imm11to5 << 5) | imm4to0;
	if ((imm11to5 >> 6) == 1)
		offset = offset | 0xFFFFF000;
	unsigned char byte = RB[rs2] & 0xFF;
	dbg_printf("sb r%d, %d(r%d)\n", rs2, offset, rs1);
	DM.write_byte(RB[rs1] + offset, byte);
	dbg_printf("Result = %#x\n\n", byte);

}

void ac_behavior(sh){

	int offset = (imm11to5 << 5) | imm4to0;
	if ((imm11to5 >> 6) == 1)
		offset = offset | 0xFFFFF000;
	short int half = RB[rs2] & 0xFFFF;
	dbg_printf("sh r%d, %d(r%d)\n", rs2, offset, rs1);
	DM.write_half(RB[rs1] + offset, half);
	dbg_printf("Result = %#x\n\n", half);

}

void ac_behavior(xori){
	
	//Signal extending
	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;	
	dbg_printf("xori r%d, r%d, %#x\n", rd, rs1, s_imm);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	RB[rd] = RB[rs1] ^ s_imm;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(ori){
	
	//Signal extending
	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;
	dbg_printf("ori r%d, r%d, %#x\n", rd, rs1, s_imm);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	RB[rd] = RB[rs1] | s_imm;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(andi){
	
	//Signal extending
	int s_imm;
	dbg_printf("imm = %#x\n", imm);
	if ((imm >> 11) == 1)
		s_imm = imm | 0xFFFFF000;
	else s_imm = imm;
	dbg_printf("andi r%d, r%d, %#x\n", rd, rs1, s_imm);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	RB[rd] = RB[rs1] & s_imm;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}


void ac_behavior(beq){
	int offset = (imm12 << 11) | (imm11 << 10) | (imm10to5 << 4) | imm4to1;
	unsigned int addr;
	//Signal extending	
	if(imm12 == 1){
		addr = ac_pc + ((offset << 1) | 0xFFFFF000) - 4;
	}
	else
		addr = ac_pc + (offset << 1) - 4;
	//shows the wrong value to imm	
	dbg_printf("beq r%d, r%d, %#x\n", rs1, rs2, offset);
	dbg_printf("addr = %#x\n", addr);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	if (RB[rs1] == RB[rs2]){
		ac_pc = (ac_pc & 0xF0000000) | addr;
		dbg_printf("Taken to %#x\n\n", (ac_pc & 0xF0000000) | addr);
	}
	else
		dbg_printf("Not taken\n\n");
}

void ac_behavior(bne){
	int offset = (imm12 << 11) | (imm11 << 10) | (imm10to5 << 4) | imm4to1;
	unsigned int addr;
	//Signal extending	
	if(imm12 == 1){
		addr = ac_pc + ((offset << 1) | 0xFFFFF000) - 4;
	}
	else
		addr = ac_pc + (offset << 1) - 4;
	//shows the wrong value to imm	
	dbg_printf("bne r%d, r%d, %#x\n", rs1, rs2, offset);
	dbg_printf("addr = %#x\n", addr);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	if (RB[rs1] != RB[rs2]){
		ac_pc = (ac_pc & 0xF0000000) | addr;
		dbg_printf("Taken to %#x\n\n", (ac_pc & 0xF0000000) | addr);
	}
	else
		dbg_printf("Not taken\n\n");
}

void ac_behavior(blt){
	int offset = (imm12 << 11) | (imm11 << 10) | (imm10to5 << 4) | imm4to1;
	unsigned int addr;
	//Signal extending	
	if(imm12 == 1){
		addr = ac_pc + ((offset << 1) | 0xFFFFF000) - 4;
	}
	else
		addr = ac_pc + (offset << 1) - 4;
	//shows the wrong value to imm	
	dbg_printf("blt r%d, r%d, %#x\n", rs1, rs2, offset);
	dbg_printf("addr = %#x\n", addr);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	if ((ac_Sword) RB[rs1] < (ac_Sword) RB[rs2]){
		ac_pc = (ac_pc & 0xF0000000) | addr;
		dbg_printf("Taken to %#x\n\n", (ac_pc & 0xF0000000) | addr);
	}
	else
		dbg_printf("Not taken\n\n");
}

void ac_behavior(bltu){
	int offset = (imm12 << 11) | (imm11 << 10) | (imm10to5 << 4) | imm4to1;
	unsigned int addr;
	//Signal extending	
	if(imm12 == 1){
		addr = ac_pc + ((offset << 1) | 0xFFFFF000) - 4;
	}
	else
		addr = ac_pc + (offset << 1) - 4;	
	dbg_printf("bltu r%d, r%d, %#x\n", rs1, rs2, offset);
	dbg_printf("addr = %#x\n", addr);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	if ((ac_Uword) RB[rs1] < (ac_Uword) RB[rs2]){
		ac_pc = (ac_pc & 0xF0000000) | addr;
		dbg_printf("Taken to %#x\n\n", (ac_pc & 0xF0000000) | addr);
	}
	else
		dbg_printf("Not taken\n\n");
}

void ac_behavior(bge){
	int offset = (imm12 << 11) | (imm11 << 10) | (imm10to5 << 4) | imm4to1;
	unsigned int addr;
	//Signal extending	
	if(imm12 == 1){
		addr = ac_pc + ((offset << 1) | 0xFFFFF000) - 4;
	}
	else
		addr = ac_pc + (offset << 1) - 4;
	//shows the wrong value to imm	
	dbg_printf("bge r%d, r%d, %#x\n", rs1, rs2, offset);
	dbg_printf("addr = %#x\n", addr);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	if ((ac_Sword) RB[rs1] >= (ac_Sword) RB[rs2]){
		ac_pc = (ac_pc & 0xF0000000) | addr;
		dbg_printf("Taken to %#x\n\n", (ac_pc & 0xF0000000) | addr);
	}
	else
		dbg_printf("Not taken\n\n");
}

void ac_behavior(bgeu){
	int offset = (imm12 << 11) | (imm11 << 10) | (imm10to5 << 4) | imm4to1;
	unsigned int addr;
	//Signal extending	
	if(imm12 == 1){
		addr = ac_pc + ((offset << 1) | 0xFFFFF000) - 4;
	}
	else
		addr = ac_pc + (offset << 1) - 4;	
	dbg_printf("bgeu r%d, r%d, %#x\n", rs1, rs2, offset);
	dbg_printf("addr = %#x\n", addr);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	if ((ac_Uword) RB[rs1] >= (ac_Uword) RB[rs2]){
		ac_pc = (ac_pc & 0xF0000000) | addr;
		dbg_printf("Taken to %#x\n\n", (ac_pc & 0xF0000000) | addr);
	}
	else
		dbg_printf("Not taken\n\n");
}

void ac_behavior(instr_and){
	
	dbg_printf("instr_and r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	RB[rd] = RB[rs1] & RB[rs2];
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(instr_xor){
	
	dbg_printf("instr_xor r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	RB[rd] = RB[rs1] ^ RB[rs2];
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(instr_or){
	
	dbg_printf("instr_or r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	dbg_printf("rs2 = %#x\n", RB[rs2]);
	RB[rd] = RB[rs1] | RB[rs2];
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(add){
	
	dbg_printf("add r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	RB[rd] = RB[rs1] + RB[rs2];
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(sub){
	
	dbg_printf("sub r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	RB[rd] = RB[rs1] - RB[rs2];
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(sll){
	
	dbg_printf("sll r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	short int shamt = RB[rs2];
	shamt = shamt << 11;
	shamt = shamt >> 11;
	RB[rd] = RB[rs1] << shamt;
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(srl){
	
	dbg_printf("srl r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	short int shamt = RB[rs2];
	shamt = shamt << 11;
	shamt = shamt >> 11;
	RB[rd] = RB[rs1] >> shamt;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(sra){
	
	dbg_printf("sra r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	short int shamt = RB[rs2];
	shamt = shamt << 11;
	shamt = shamt >> 11;
	RB[rd] = (ac_Sword) RB[rs1] >> shamt;
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(slt){
	
	dbg_printf("slt r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	if ((ac_Sword)RB[rs1] < (ac_Sword)RB[rs2])
		RB[rd] = 1;
	else
		RB[rd] = 0;
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(sltu){
	
	dbg_printf("slt r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	if ((ac_Uword)RB[rs1] < (ac_Uword)RB[rs2])
		RB[rd] = 1;
	else
		RB[rd] = 0;
	dbg_printf("Result = %d\n\n", RB[rd]);

}

//includes srli and srai instructions
void ac_behavior(srli){
	
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	unsigned int shamt = imm & 0xFF;
	if ((imm >> 10) == 1) {
		dbg_printf("srai ");
		RB[rd] = (ac_Sword) RB[rs1] >> shamt;
	}
	else {
		dbg_printf("srli ");
		RB[rd] = RB[rs1] >> shamt;
	}
	dbg_printf(" r%d, r%d, %#x\n", rd, rs1, imm);
	dbg_printf("shamt = %d\n", shamt);
	dbg_printf("Result = %#x\n\n", RB[rd]);

}

void ac_behavior(slli){
	
	dbg_printf("rs1 = %#x\n", RB[rs1]);
	unsigned int shamt = imm & 0xFF;
	RB[rd] = RB[rs1] << shamt;
	dbg_printf("slli r%d, r%d, %#x\n", rd, rs1, imm);
	dbg_printf("shamt = %d\n", shamt);
	dbg_printf("Result = %#x\n\n", RB[rd]);

}


void ac_behavior(mul){

	long long result;
	int half_result;
	dbg_printf("mul r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	result = (ac_Sword) RB[rs1];
  	result *= (ac_Sword) RB[rs2];
	half_result = (result & 0xFFFFFFFF);
	RB[rd] = half_result;	// the low half
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(mulh){

	long long result;
	int half_result;
	dbg_printf("mulh r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	result = (ac_Sword) RB[rs1];
  	result *= (ac_Sword) RB[rs2];
	half_result = ((result >> 32) & 0xFFFFFFFF);
	RB[rd] = half_result;	// the high half
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(mulhu){

	unsigned long long result;
	unsigned int half_result;
	dbg_printf("mulhu r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	result = (ac_Uword) RB[rs1];
  	result *= (ac_Uword) RB[rs2];
	half_result = ((result >> 32) & 0xFFFFFFFF);
	RB[rd] = half_result;	// the high half
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(mulhsu){

	long long result;
	int half_result;
	dbg_printf("mulhsu r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	result = (ac_Sword) RB[rs1];
  	result *= (ac_Uword) RB[rs2];
	half_result = ((result >> 32) & 0xFFFFFFFF);
	RB[rd] = half_result;	// the high half
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(div){

	dbg_printf("div r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	RB[rd] = (ac_Sword) RB[rs1] / (ac_Sword) RB[rs2];
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(divu){

	dbg_printf("divu r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	RB[rd] = RB[rs1] / RB[rs2];
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(rem){

	dbg_printf("rem r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	RB[rd] = (ac_Sword) RB[rs1] % (ac_Sword) RB[rs2];
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(remu){

	dbg_printf("remu r%d, r%d, r%d\n", rd, rs1, rs2);
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	RB[rd] = RB[rs1] % RB[rs2];
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amoswap_w){

	dbg_printf("amoswap.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load	
	DATA_PORT->write(RB[rs1], RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amoadd_w){

	dbg_printf("amoadd.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load	
	DATA_PORT->write(RB[rs1], RB[rd] + RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amoxor_w){

	dbg_printf("amoxor.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load	
	DATA_PORT->write(RB[rs1], RB[rd] ^ RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amoand_w){

	dbg_printf("amoand.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load	
	DATA_PORT->write(RB[rs1], RB[rd] & RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amoor_w){

	dbg_printf("amoor.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load	
	DATA_PORT->write(RB[rs1], RB[rd] | RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amomin_w){

	dbg_printf("amomin.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load
	if ((ac_Sword) RB[rd] <= (ac_Sword) RB[rs2])	
		DATA_PORT->write(RB[rs1], RB[rd]); //store
	else
		DATA_PORT->write(RB[rs1], RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amomax_w){

	dbg_printf("amomax.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load
	if ((ac_Sword) RB[rd] >= (ac_Sword) RB[rs2])	
		DATA_PORT->write(RB[rs1], RB[rd]); //store
	else
		DATA_PORT->write(RB[rs1], RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amominu_w){

	dbg_printf("amominu.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load
	if ((ac_Uword) RB[rd] <= (ac_Uword) RB[rs2])	
		DATA_PORT->write(RB[rs1], RB[rd]); //store
	else
		DATA_PORT->write(RB[rs1], RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

void ac_behavior(amomaxu_w){

	dbg_printf("amomaxu.w r%d, r%d, r%d\n", rd, rs1, rs2);
	RB[rd] = DATA_PORT->read(RB[rs1]); //load
	if ((ac_Uword) RB[rd] >= (ac_Uword) RB[rs2])	
		DATA_PORT->write(RB[rs1], RB[rd]); //store
	else
		DATA_PORT->write(RB[rs1], RB[rs2]); //store
	dbg_printf("rs1 = %d\n", RB[rs1]);
	dbg_printf("rs2 = %d\n", RB[rs2]);
	dbg_printf("Result = %d\n\n", RB[rd]);

}

// Privileged Intructions

void ac_behavior(csrrw){

	dbg_printf("csrrw r%d, %#x, r%d\n", rd, imm, rs1);
	switch(imm){
		case CSR_MHARTID:
		RB[rd] = mhartid;
		mhartid = RB[rs1];
		break;
	}	
	dbg_printf("mhartid = %#x\n\n", mhartid.read());

}

void ac_behavior(csrrs){

	dbg_printf("csrrs r%d, %#x, r%d\n", rd, imm, rs1);
	switch(imm){
		case CSR_MHARTID:
			RB[rd] = mhartid;
			mhartid = mhartid | RB[rs1];
			break;
	}	
	dbg_printf("mhartid = %#x\n\n", mhartid.read());

}

void ac_behavior(csrrc){

	dbg_printf("csrrc r%d, %#x, r%d\n", rd, imm, rs1);
	switch(imm){
		case CSR_MHARTID:
			RB[rd] = mhartid;
			mhartid = mhartid ^ (mhartid & RB[rs1]);
			break;
	}	
	dbg_printf("mhartid = %#x\n\n", mhartid.read());

}

void ac_behavior(csrrwi){

	dbg_printf("csrrwi r%d, %#x, %#x\n", rd, imm, rs1);
	switch(imm){
		case CSR_MHARTID:
		RB[rd] = mhartid;
		mhartid = rs1;    //rs1 = zimm
		break;
	}	
	dbg_printf("mhartid = %#x\n\n", mhartid.read());

}

void ac_behavior(csrrsi){

	dbg_printf("csrrsi r%d, %#x, %#x\n", rd, imm, rs1);
	switch(imm){
		case CSR_MHARTID:
			RB[rd] = mhartid;
			mhartid = mhartid | rs1;    //rs1 = zimm
			break;
	}	
	dbg_printf("mhartid = %#x\n\n", mhartid.read());

}

void ac_behavior(csrrci){

	dbg_printf("csrrci r%d, %#x, %#x\n", rd, imm, rs1);
	switch(imm){
		case CSR_MHARTID:
			RB[rd] = mhartid;
			mhartid = mhartid ^ (mhartid & rs1);    //rs1 = zimm
			break;
	}	
	dbg_printf("mhartid = %#x\n\n", mhartid.read());

}






