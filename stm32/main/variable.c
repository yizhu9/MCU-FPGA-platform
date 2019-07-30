/*C**************************************************************************
* NAME:   variable.c
*----------------------------------------------------------------------------
* Copyright (c) 2017.
*----------------------------------------------------------------------------
* RELEASE:      2017.04.03
* REVISION:     1.0 
*----------------------------------------------------------------------------
* PURPOSE:
*   This file contains the definition of the global variables.
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include "board.h"           /* system config definition */


/*_____ D E F I N I T I O N ________________________________________________*/
/* 系统计时 */
uint32_t  gl_sys_tick = 0;	 /* 系统计时,增 tick */                    
uint32_t  gl_dly_tick;	     /* 应答延时计时,减 tick */  

/* USART */
char      msg_buf[MAX_RecvFrame];     // received message, used for proceding
uint32_t  msg_buf_valid;	            // received valid flag
char      recv_buf[MAX_RecvFrame];    // receiving buffer               
uint32_t  recv_state;                 // receive state
uint32_t  recv_timer;                 // receive time-out, 用于字节间超时判定
//uint8_t   recv_chksum;              // computed checksum
uint32_t  recv_ctr;                   // reveiving pointer 

/*
uint8_t   trans_buf[MAX_TransFrame];  // uart transfer buffer（发送前需要填充所有字节，除校验字节外）
uint32_t  trans_ctr;                  // transfering pointer
uint32_t  trans_size;                 // transfered bytes number（含所有字节，即含帧头和校验）
uint8_t   trans_chksum;               // check-sum （边发送边计算）
uint32_t  trans_occupy;               // 发送器被占用标志，1-被占用, 0-空闲                      

uint8_t   uart_q_index;               // 正在发送某队列项的序号：若为0xFF, 表示没有任何项在发送
sUART_Q   uart_q[UART_QUEUE_NUM];	    // 串口队列 					
*/

/* LED */
uint32_t  led_flash_tick;      //all leds 闪烁计时tick  
uint32_t  led_flash_pn;        //闪烁LED的亮灭时间段标志: 0 - 灭; 1 - 亮
uint32_t  led_light_mode[3];   //每个led点亮方式:  0 - 灭; 1 - 恒亮; 2 - 闪烁

/*  */
uint16_t  gVoltage_Ampl;       //脉冲幅度
uint16_t  gPulse_width;        //脉冲宽度
uint16_t  gPulse_period;       //脉冲周期
uint32_t  gSamples_K;          //采样点数(以K为单位, 1K=1024点)
uint32_t  gExec_flag;          //执行标志: 
                               //    0 - 正等待主机命令,无需查询FPGA并发送采样数据, 
															 //        或已经完成一次接收到主机命令并发送采样数据过程
															 //    1 - 已收到主机命令,并送达到FPGA,正查询FPGA是否已准备好所需数据
															 //    2 - FPGA已准备好数据,ARM正从FPGA读采样数据


/*_____ D E C L A R A T I O N ______________________________________________*/
