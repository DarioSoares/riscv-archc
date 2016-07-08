/*************************************************
**************************************************
*	@file		riscv_gdb_funcs.cpp
*	@author		Dário Dias
*	@email		dsd@cin.ufpe.br		
*	@version	0.20
*	@date		8 July 2016
*	@brief		The ArchC Risc-V functional model.
**************************************************
*************************************************/

//TODO - Using the mips code

#include "riscv.H"
/*
// 'using namespace' statement to allow access to all
// riscv-specific datatypes
using namespace riscv_parms;

int riscv::nRegs(void) {
   return 73;
}


ac_word riscv::reg_read( int reg ) {
  // general purpose registers 
  if ( ( reg >= 0 ) && ( reg < 32 ) )
    return RB.read( reg );
  return 0;
}


void riscv::reg_write( int reg, ac_word value ) {
  // general purpose registers
  if ( ( reg >= 0 ) && ( reg < 32 ) )
    RB.write( reg, value );
  else
    {
      // lo, hi
      if ( ( reg >= 33 ) && ( reg < 35 ) )
        RB.write( reg - 1, value );
      else
        // pc
        if ( reg == 37 )
          ac_pc = value;
    }
}


unsigned char riscv::mem_read( unsigned int address ) {
  return IM->read_byte( address );
}


void riscv::mem_write( unsigned int address, unsigned char byte ) {
  IM->write_byte( address, byte );
}*/
