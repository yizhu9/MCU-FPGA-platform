/*
 * FILE                : fpga_flag.h
 * DESCRIPTION         : This file is for fpga_flag.c
 * Author              : 
 * Rev                 : 1.0
 * Date                : 2017-04-04
 */
#ifndef __fpga_flag_h__
#define __fpga_flag_h__

//--------------------------- Define ---------------------------//
#define FPGA_FLAG  GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10)

//----------------------- Include files ------------------------//

//-------------------------- Typedef ---------------------------//
typedef struct {
	int (* initialize)(void);
}FPGA_FLAG_T;

//--------------------------- Extern ---------------------------//
extern FPGA_FLAG_T  fpga_flag;

#endif //__fpga_flag_h__
