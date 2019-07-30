/*
 * FILE                : fpga_flag.c
 * DESCRIPTION         : This file is FPGA_FLAG driver.
 * Author              : 
 * Rev                 : 1.0
 * Date                : 2017-04-04
 */
//--------------------------- Include ---------------------------//
#include "..\include\fpga_flag.h"
#include "..\fwlib\inc\stm32f4xx_gpio.h"

//--------------------- Function Prototype ----------------------//
static int initialize(void);

//--------------------------- Variable --------------------------//
FPGA_FLAG_T fpga_flag = {
	.initialize = initialize
};

//--------------------------- Function --------------------------//
/*
 * Name                : initialize
 * Description         : 
 * Author              : 
 * Rev                 : 1.0
 * Date                : 2017-04-04
 */
static int initialize(void)
{
	GPIO_InitTypeDef   GPIO_uInitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_10;               
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_IN;          // ‰»Î      
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_25MHz;          
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //…œ¿≠      

	GPIO_Init(GPIOG, &GPIO_uInitStructure);      

	return 0;
}
