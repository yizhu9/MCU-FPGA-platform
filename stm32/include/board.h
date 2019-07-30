/**
  ******************************************************************************
  * @file    board.h
  * @author  
  * @version V1.0.0
  * @date    2017-04-03
  * @brief   Header file for MOS-Tester
  ******************************************************************************  
  */ 
  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef __cplusplus
 extern "C" {
#endif 

	 
/* Includes ------------------------------------------------------------------*/	 
#include <stdint.h>
	 
	 
/* Scheduler Tick */
#define SCHEDULER_TICK       10    //单位: ms	(必须是ms,以配合SysTick_Config())

/* UART */
#define	FRAME_STX          0x02	   // Frame header = \STX
#define FRAME_ETX          0x03    // Frame end 	 = \ETX
//#define UART_BAUD_RATE   115200    // UART baud rate
//#define UART_BAUD_RATE 460800    // UART baud rate	 
#define UART_BAUD_RATE 921600    // UART baud rate	 
#define	MAX_RecvFrame        30    // 接收缓存区大小
#define	MAX_TransFrame       30    // 发送缓存区大小
#define RECV_TIMEOUT          5    // 接收时允许的字节间最大时间间隔, 单位: tick                                

/* Uart Queue */
typedef struct strUART_Q
{
  uint32_t  flag;                   //状态：0 - 空闲; 1 - 等待发送; 2 - 正在发送; 3 - 已发送,等待应答
  uint8_t   tdata[MAX_TransFrame];  //数据包
  uint32_t  len;					          //数据包有效长度(含效验字节)
  uint32_t  need_wait_ack;          //是否需要等待目的机应答： 0 - 否； 1 - 是
  uint32_t  wait_ack_time;		      //发送后等待目的机应答所剩余的时间，单位：tick 
                                    //  当flag=3,且剩余时间减为0时，重发                                    
}sUART_Q;

#define UART_WAIT_ACK   (300/SCHEDULER_TICK)   //发送后等待目的机应答所允许的最大时间, 单位：tick
#define UART_QUEUE_NUM   5                     //发送队列数

/* uart state constant(仅用于接收) */
#define FSA_INIT      0      //等待帧头 \STX
#define FSA_DATA      1      //等待命令串
#define FSA_END       2      //等待帧尾 \ETX 

/* WDT */		 
//#define Enable_WDT

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_H_ */  


/******************* (C) COPYRIGHT 2017 *****END OF FILE****/
