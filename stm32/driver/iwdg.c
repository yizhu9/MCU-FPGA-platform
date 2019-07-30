 /*
 * FILE               : iwdg.c
 * DESCRIPTION        : This file is iwdg driver.
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
 
//----------------------include----------------------------//
#include "board.h"
#include "..\include\iwdg.h"
#include "..\fwlib\inc\stm32f4xx_iwdg.h"


//------------------ Function Prototype ------------------//
static int  initialize(void);
static void feed_dog(void);


//----------------------variable--------------------------//
IWDG_T iwdg = {
	.initialize = initialize,
	.feed_dog = feed_dog
};


//-----------------Function-----------------------------//
/*
 * Name               : initialize
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int initialize(void)
{  
#ifdef Enable_WDT		
	//启动WDT
	//1s喂狗
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);         //取消寄存器的写保护
	IWDG_SetPrescaler(4);                                 //设置独立看门狗的预分频系数
	IWDG_SetReload(625);                                  //设置独立看门狗的重装载值
	IWDG_ReloadCounter();                                 //重装载计数器	
	IWDG_Enable();                                        //使能独立看门狗
#endif  	
	
	return 0;
}


/*
 * Name               : feed_dog
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static void feed_dog(void)
{
	IWDG_ReloadCounter();
}
