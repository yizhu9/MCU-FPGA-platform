/*
 * FILE                : led.c
 * DESCRIPTION         : This file is led driver.
 * Author              : 
 * Rev                 : 0.00
 * Date                : 01/03/2016
 */
 
//--------------------------- Include ---------------------------//
#include "..\include\led.h"
#include "..\fwlib\inc\stm32f4xx_rcc.h"


//--------------------- Function Prototype ----------------------//
static int initialize(void);


//--------------------------- Variable --------------------------//
LED_T led = {
	.initialize = initialize
};


//--------------------------- Function --------------------------//
/*
 * Name                : initialize
 * Description         : 
 * Author              : 
 * Rev                 : 0.00
 * Date                : 01/03/2016
 */
static int initialize(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;

	//LED IO初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;    //设置连接三色LED灯的IO端口
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;                          //设置端口为输出模式
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;                         //推挽输出
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;                           //上拉
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;                     //设置速度为第三级

	GPIO_Init(GPIOI,&GPIO_uInitStructure);

	//PI5、PI6、PI7接三色LED灯，PI5、PI6、PI7置高电位，灯熄灭
	GPIO_SetBits(GPIOI, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

	return 0;
}
