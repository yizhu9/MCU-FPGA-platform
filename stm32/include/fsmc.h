/*
 * FILE								: fsmc.h
 * DESCRIPTION				: This file is fsmc file header.
 * Author							: 
 * Rev								: 1.0
 * Date								: 2017-04-04
 */
//------------------------define---------------------------//
#ifndef __fsmc_h__
#define __fsmc_h__

//---------------------Include files-----------------------//

//----------------------- Define --------------------------//
#define fpga_write(offset,data)	 *((volatile unsigned short int *)(0x60000000 + (offset << 21))) = data	
#define fpga_read(offset)	       *((volatile unsigned short int *)(0x60000000 + (offset << 21)))
	
#define fpga_write_SRAM(offset,data)	*((volatile unsigned short int *)(0x60000000 + offset)) = data	
#define fpga_read_SRAM(offset)	      *((volatile unsigned short int *)(0x60000000 + offset))


//----------------- Typedef -----------------------------//
typedef struct{
	int (* initialize)(void);
}FSMC_T;

//---------------- Extern -------------------------------//

extern FSMC_T fsmc;

#endif //__fsmc_h__
