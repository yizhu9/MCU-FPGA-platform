/*
 * FILE                : usart.c
 * DESCRIPTION         : This file is usart driver.
 * Author              : 
 * Rev                 : 1.0
 * Date                : 2017-04-04
 */
//------------------------ Include files ------------------------//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#include "..\FWlib\inc\stm32f4xx_rcc.h"
#include "..\FWlib\inc\stm32f4xx_gpio.h"
#include "..\FWlib\inc\stm32f4xx_usart.h"

#include "board.h"
#include "..\include\usart.h"


//--------------------------- Variable --------------------------// 
/* USART */			
extern  char      msg_buf[MAX_RecvFrame];     // received message, used for proceding
extern  uint32_t  msg_buf_valid;	            // received valid flag
extern  char      recv_buf[MAX_RecvFrame];    // receiving buffer               
extern  uint32_t  recv_state;                 // receive state
extern  uint32_t  recv_timer;                 // receive time-out, 用于字节间超时判定
//extern  uint8_t   recv_chksum;              // computed checksum
extern  uint32_t  recv_ctr;                   // reveiving pointer 


static int send_string_to_usart4(char * str);
static int send_byte_to_usart4(char data);
static int initialize_usart4(unsigned long int baudrate);
static int my_printf4(const char * fmt,...);
static int send_buffer4(void * ,int);


UART_T usart4 = {
	//.receive_ok_flag=0,
	//.counter=0,
	.send_byte   = send_byte_to_usart4,
	.send_string = send_string_to_usart4,
	.initialize  = initialize_usart4,
	.printf      = my_printf4,
	.send_buffer = send_buffer4
};


//--------------------------- Function --------------------------//
//USART4 initialize
/*
 * Name               : initialize_usart4
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int initialize_usart4(unsigned long int baudrate)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);             // GPIO时钟使能
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                        //PA0为复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                        //PA1为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);           //PA0引脚复用为UART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);           //PA1引脚复用为UART4
	
	USART_DeInit(UART4);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);            //UART4时钟使能
	  
	USART_InitStructure.USART_BaudRate = baudrate;                   //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No ;             //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //收发模式
	USART_Init(UART4, &USART_InitStructure);                         //初始化UART4
	USART_Cmd(UART4, ENABLE);                                        //使能UART4
	//USART_ITConfig(UART4,USART_IT_PE,ENABLE);             //USART_IT_PE   : if a parity error occur
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);             //USART_IT_RXNE : if receive buffer register is not empty
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	return 0;
}

/*
 * Name               : send_buffer4
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int send_buffer4(void * buf,int len)                                  
{
	char *p = (char *)buf;
	
	if (len <= 0)
		return -1;
	
	while(len --){
		send_byte_to_usart4(*p);
		p ++;
	}
	
	return 0;
}

/*
 * Name               : send_byte_to_usart4
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int send_byte_to_usart4(char data)
{
	//while(!(USART_GetFlagStatus(UART4,USART_FLAG_TC) == 1));        //等待发送数据完成
	while(!(USART_GetFlagStatus(UART4,USART_FLAG_TXE) == 1));         //ZZX: 等待发送寄存器空
	USART_SendData(UART4,data);                                       //将数据写入数据寄存器中
	
	return 0;
}

/*
 * Name               : send_string_to_usart4
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int send_string_to_usart4(char * str)
{
	while(*str != '\0'){
		while(!(USART_GetFlagStatus(UART4,USART_FLAG_TC) == 1));
		USART_SendData(UART4,*str++);	
	}
	return 0;
}

/*
 * Name               : printf
 * Description        : 
 * Author             : 
 * Rev                : 0.00
 * Date               : 11/21/2015
 */
static int my_printf4(const char * fmt,...)
{
	__va_list arg_ptr; 
	char buf[UART_BUFFER_SIZE];
  		
	memset(buf,'\0',sizeof(buf));

	va_start(arg_ptr,fmt);
	vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	send_string_to_usart4(buf);

	return 0;
}


/*
 * Name               : UART4_IRQHandler
 * Description        : 
 * Author             : 
 * Rev                : 1.0
 * Date               : 2017-04-04
*  Note               : 由 startup_stm32f40_41xxx.s 挂入 
 */
void UART4_IRQHandler(void)
{	
  char c;	
    
  if (USART_GetFlagStatus(UART4,USART_FLAG_RXNE) != RESET)
  { //接收中断(接收到一个字节)
    c = USART_ReceiveData(UART4);	  //读DR自动清RXNE	        
    switch (recv_state)
    {
      case FSA_INIT://是否为帧头
                    if (c == FRAME_STX)
										{ //为帧头, 开始新的一帧                        
											recv_ctr = 0;				
											recv_timer = RECV_TIMEOUT;						
											recv_state = FSA_DATA; 							
										}
										break;				 																			
										
      case FSA_DATA://读取命令串
					          recv_buf[recv_ctr] = c;                                      
										if (recv_ctr == 15)             
											recv_state = FSA_END;   //已经收到指定长度的命令数据
										else
											recv_ctr++;   //还未结束						 
                    recv_timer = RECV_TIMEOUT;
                    break;

      case FSA_END: //是否为帧尾
                    if ((c == FRAME_ETX) && (msg_buf_valid == 0))
                    { //已经收到完整一帧并且以前收到的消息已经被处理了
                      memcpy(&msg_buf[0], &recv_buf[0], 16);	 					  						  
                      msg_buf_valid = 1;
                    }										
      default:      //复位
                    recv_state = FSA_INIT;
                    break;                        	
    }//end switch         	    
  }//end if	
}
