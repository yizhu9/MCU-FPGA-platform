/*
 * FILE               : systick.c
 * DESCRIPTION        : This file is systick driver.
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
 
//--------------------------- Include ---------------------------//
#include "board.h"
#include "..\include\systick.h"
#include "..\fwlib\inc\stm32f4xx_syscfg.h"


/* 系统计时 */
extern  uint32_t  gl_sys_tick;	 /* 系统计时,增 tick */                    
extern  uint32_t  gl_dly_tick;	 /* 应答延时计时,减 tick */  

/* LED */
extern  uint32_t  led_flash_tick;    //all leds 闪烁计时tick  

/* USART */			              
extern  uint32_t  recv_state;        //receive state
extern  uint32_t  recv_timer;        //receive time-out, 用于字节间超时判定


//--------------------- Function Prototype ----------------------//
static int initialize(void);

//--------------------------- Variable --------------------------//
SYSTICK_T systick = {
	.initialize = initialize
};

//--------------------------- Function --------------------------//
/*
 * Name               : initialize
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int initialize(void)
{
	//按 SCHEDULER_TICK 定时, SCHEDULER_TICK的单位必须为 ms	
	if (SysTick_Config((SystemCoreClock / 1000) * SCHEDULER_TICK))
	{ //未设置成功
		while(1);
	}
	return 0;
}


/*
 * Name               : SysTick_Handler
 * Description        : 
 * Author             : 
 * Rev                : 1.00
 * Date               : 2017-04-04
 */
void SysTick_Handler(void)
{
  /* 系统计时 */
  gl_sys_tick++;         /* 系统计时,增 tick */  
  if (gl_dly_tick > 0)   /* 应答延时计时,减 tick */  
	  gl_dly_tick--;

	/* 计时变量处理 */
	led_flash_tick++;      //LED闪烁计时
	
  /* USART字节之间接收超时 */  
	if (recv_state != FSA_INIT) 
	{ //非初始状态，需要检测是否超时
		if (recv_timer > 0)
			recv_timer--;
		if (recv_timer == 0)  
			recv_state = FSA_INIT;   //字节之间接收超时，复位至初始状态
	}
}
