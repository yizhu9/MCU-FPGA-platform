/*C**************************************************************************
* NAME:   alarm_task.c
*----------------------------------------------------------------------------
* Copyright (c) 2017.
*----------------------------------------------------------------------------
* RELEASE:     2017.04.04
* REVISION:    1.0     
*----------------------------------------------------------------------------
* PURPOSE:          
*   This file contains the led display task and attached routines.
*****************************************************************************/


/*_____ I N C L U D E S ____________________________________________________*/
#include "board.h"               /* board configuration */
#include "led.h"
#include "alarm_task.h"          /* alarm task definition */


/*_____ D E F I N I T I O N ________________________________________________*/
/* LED闪烁周期参数 */
#define  LED_ON_FL    (200/SCHEDULER_TICK)
#define  LED_OFF_FL   (800/SCHEDULER_TICK)


/*_____ D E C L A R A T I O N ______________________________________________*/
/* LED */
extern  uint32_t  led_flash_tick;      //all leds 闪烁计时tick  
extern  uint32_t  led_flash_pn;        //闪烁LED的亮灭时间段标志: 0 - 灭; 1 - 亮
extern  uint32_t  led_light_mode[3];   //每个led点亮方式:  0 - 灭; 1 - 恒亮; 2 - 闪烁



/*F**************************************************************************
* NAME: alarm_task_init
*----------------------------------------------------------------------------
* PARAMS:
* return:
*----------------------------------------------------------------------------
* PURPOSE: alarm task initialization
*****************************************************************************/
void alarm_task_init(void)
{
  /* for led initialize */  
	led_flash_pn = 0; 
  led_light_mode[0] = 2;  //红灯闪烁
  led_light_mode[1] = 0;  //绿灯灭
	led_light_mode[2] = 0;  //兰灯灭					
}


/*F**************************************************************************
* NAME: alarm_task
*----------------------------------------------------------------------------
* PARAMS:
* return:
*----------------------------------------------------------------------------
* PURPOSE: alarm task
*****************************************************************************/
void alarm_task(void)
{ 
  //1. LED           
  if ((led_light_mode[0] == 2) || (led_light_mode[1] == 2) || (led_light_mode[2] == 2))
  { //有需要处理的闪烁LED  
    if (led_flash_pn && (led_flash_tick >= LED_ON_FL))
    { //处于亮时间段, 可翻转为灭
      if (led_light_mode[0] == 2)
      { //红灯为闪烁    
        LED_RED_OFF;      //灭      
      }
      if (led_light_mode[1] == 2)
      { //绿灯为闪烁
        LED_GREEN_OFF;     //灭             
      }	
      if (led_light_mode[2] == 2)
      { //兰灯为闪烁
        LED_BLUE_OFF;     //灭             
      }				
      led_flash_tick = 0;
      led_flash_pn = 0;
    }
    else if (!led_flash_pn && (led_flash_tick >= LED_OFF_FL))
    { //处于灭时间段, 可翻转为亮
      if (led_light_mode[0] == 2)
      { //红灯为闪烁
        LED_RED_ON;       //亮
      }
      if (led_light_mode[1] == 2)
      { //绿灯为闪烁
        LED_GREEN_ON;     //亮       
      }	
      if (led_light_mode[2] == 2)
      { //兰灯为闪烁
        LED_BLUE_ON;      //亮       
      }			
      led_flash_tick = 0;
      led_flash_pn = 1;              
    }        
  }  
}//end FUNC()
