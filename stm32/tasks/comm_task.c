/*C**************************************************************************
* NAME:  comm_task.c
*----------------------------------------------------------------------------
* Copyright (c) 2017.
*----------------------------------------------------------------------------
* RELEASE:      2017.04.03
* REVISION:     1.0     
*----------------------------------------------------------------------------
* PURPOSE:          
*   This file contains the communication task and attached routines
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include "usart.h"
#include "led.h"
#include "fsmc.h"
#include "fpga_flag.h"
#include "comm_task.h"            /* comm task definition */


/*_____ D E F I N I T I O N ________________________________________________*/


/*_____ D E C L A R A T I O N ______________________________________________*/      
/* 系统计时 */
extern   uint32_t  gl_sys_tick;	  /* 系统计时,增 tick */                    
extern   uint32_t  gl_dly_tick;	  /* 应答延时计时,减 tick */ 

/* LED */
extern  uint32_t  led_flash_tick;      //all leds 闪烁计时tick  
extern  uint32_t  led_flash_pn;        //闪烁LED的亮灭时间段标志: 0 - 灭; 1 - 亮
extern  uint32_t  led_light_mode[3];   //每个led点亮方式:  0 - 灭; 1 - 恒亮; 2 - 闪烁

/* USART */			
extern  char      msg_buf[MAX_RecvFrame];     // received message, used for proceding
extern  uint32_t  msg_buf_valid;	            // received valid flag
extern  char      recv_buf[MAX_RecvFrame];    // receiving buffer               
extern  uint32_t  recv_state;                 // receive state
extern  uint32_t  recv_timer;                 // receive time-out, 用于字节间超时判定
//extern  uint8_t   recv_chksum;                // computed checksum
extern  uint32_t  recv_ctr;                   // reveiving pointer 

/*
extern  uint8_t   trans_buf[MAX_TransFrame];  // uart transfer buffer（发送前需要填充所有字节，除校验字节外）
extern  uint32_t  trans_ctr;                  // transfering pointer
extern  uint32_t  trans_size;                 // transfered bytes number（含所有字节，即含帧头和校验）
extern  uint8_t   trans_chksum;               // check-sum （边发送边计算）
extern  uint32_t  trans_occupy;               // 发送器被占用标志，1-被占用, 0-空闲                      

extern  uint8_t  uart_q_index;                // 正在发送某队列项的序号：若为0xFF, 表示没有任何项在发送
extern  sUART_Q  uart_q[UART_QUEUE_NUM];	    // 串口队列
*/		

extern  uint16_t  gVoltage_Ampl;   //脉冲幅度
extern  uint16_t  gPulse_width;    //脉冲宽度
extern  uint16_t  gPulse_period;   //脉冲周期
extern  uint32_t  gSamples_K;      //采样点数(以K为单位, 1K=1024点)
extern  uint32_t  gExec_flag;      //执行标志: 
                                   //    0 - 正等待主机命令,无需查询FPGA并发送采样数据, 
															     //        或已经完成一次接收到主机命令并发送采样数据过程
															     //    1 - 已收到主机命令,并送达到FPGA,正查询FPGA是否已准备好所需数据
															     //    2 - FPGA已准备好数据,ARM正从FPGA读采样数据

/* UART */ 
void uart_init(void);
//void uart_start_trans(void);    



/*F**************************************************************************
* NAME: uart_init
*----------------------------------------------------------------------------
* PARAMS:
* return:
*----------------------------------------------------------------------------
* PURPOSE: uart device initialization
*----------------------------------------------------------------------------
* REQUIREMENTS:
*****************************************************************************/
void uart_init(void)
{	
  //复位变量	
	recv_state = FSA_INIT;
	recv_timer = 0;
	recv_ctr = 0;
	//trans_ctr = 0;
	//trans_size = 0;	
	//trans_occupy = 0;   //空闲
  
  //初始化USART硬件
  //已在main()的 usart4.initialize(UART_BAUD_RATE) 中执行
}


/*F**************************************************************************
* NAME: comm_task_init
*----------------------------------------------------------------------------
* PARAMS:
* return:
*----------------------------------------------------------------------------
* PURPOSE: Communication task initialization
*----------------------------------------------------------------------------
* REQUIREMENTS:
*****************************************************************************/
void comm_task_init(void)
{ 
  //uint8_t i;	

  //1. USART变量初始化	
	msg_buf_valid = 0;
	
/*	
	uart_q_index  = 0xFF;   //无队列项进入发送流程
	for (i=0; i<UART_QUEUE_NUM; i++)
	{
		uart_q[i].flag = 0;                 //均空闲
		//uart_q[i].tdata[0] = FRAME_STX;	  //帧头
		uart_q[i].need_wait_ack = 0;        //不需应答 
	}/
*/
  
  //2. UART硬件初始化
  uart_init(); 
	
	//3. 打印提示
	usart4.printf("hello! I am MOS-Tester!\r\n");
	
	//4. 系统状态
	gExec_flag = 0;    //等待主机命令
}
					

/*F**************************************************************************
* NAME: comm_task
*----------------------------------------------------------------------------
* PARAMS:
* return:
*----------------------------------------------------------------------------
* PURPOSE: communication task
*----------------------------------------------------------------------------
* REQUIREMENTS:
*****************************************************************************/
void comm_task(void)
{   
	char str_val[5];	
	static uint32_t samples_K_remain;
	uint32_t i, j, k;
	uint16_t fsmc_val;
	uint32_t fsmc_addr_base;   //每K数据的基地址
		 		
  //1. 是否有新接收到的命令包?
  if (msg_buf_valid)
  {	//有来自主机的命令包
    //a. 提取各个参数
		//计算脉冲幅度
		memcpy(&str_val[0], &msg_buf[0], 4);
		str_val[4] = 0;
		gVoltage_Ampl = atoi(str_val);       
		//计算脉冲宽度
		memcpy(&str_val[0], &msg_buf[4], 4);
		//str_val[4] = 0;				
    gPulse_width  = atoi(str_val);
		//计算脉冲周期
		memcpy(&str_val[0], &msg_buf[8], 4);
		//str_val[4] = 0;				
    gPulse_period = atoi(str_val);
    //计算采样点数(K为单位)
		memcpy(&str_val[0], &msg_buf[12], 4);
		//str_val[4] = 0;			
    gSamples_K  = atoi(str_val);    //K为单位,每K为1024个点
    samples_K_remain = gSamples_K;			
		
		//b. 置标志
    if (gExec_flag == 2)
		{ //正在读上次触发,停止读, 停止绿色灯闪烁指示
      led_light_mode[1] = 0;    //灭绿色灯
		  LED_GREEN_OFF;
		}			
		gExec_flag = 1;
		//设置指示灯(蓝色灯长亮)							
    led_light_mode[0] = 0;    //灭红色灯
		LED_RED_OFF;
		led_light_mode[2] = 1;    //长亮蓝色灯
    LED_BLUE_ON;
		
		//c. 将三个参数写入FPGA
		//先产生复位
    fpga_write((uint32_t)0x08, 0x00);   
/*		
		//测试复位后的脉冲宽度寄存器是否正确?
		fsmc_val = fpga_read(0x02);		
    usart4.send_byte((fsmc_val >> 8) & 0xFF);	   //发送高字节		
    usart4.send_byte(fsmc_val & 0xFF);	         //发送低字节
		//测试复位后的脉冲周期寄存器是否正确?
		fsmc_val = fpga_read(0x04);
    usart4.send_byte((fsmc_val >> 8) & 0xFF);	   //发送高字节		
    usart4.send_byte(fsmc_val & 0xFF);	         //发送低字节
*/
    //开始写入命令值
		fpga_write((uint32_t)0x01, gVoltage_Ampl);   //脉冲电压幅值	
		fpga_write((uint32_t)0x02, gPulse_width);    //脉冲电压宽度						
		fpga_write((uint32_t)0x04, gPulse_period);   //脉冲电压周期		
		
		//测试写入后的脉冲宽度寄存器是否正确?
		fsmc_val = fpga_read(0x02);		
    usart4.send_byte((fsmc_val >> 8) & 0xFF);	   //发送高字节		
    usart4.send_byte(fsmc_val & 0xFF);	         //发送低字节
		//测试写入后的脉冲周期寄存器是否正确?
		fsmc_val = fpga_read(0x04);
    usart4.send_byte((fsmc_val >> 8) & 0xFF);	   //发送高字节		
    usart4.send_byte(fsmc_val & 0xFF);	         //发送低字节
	
		
		//ZZX: 测试SRAM 		
/*		
    //先写入SRAM - 系列值
		j = samples_K_remain;
    while (j)		
		{	
			//计算当前1K采样点的SRAM基地址
			fsmc_addr_base = gSamples_K - j;   //K 地址
			fsmc_addr_base <<= 10;             //16bit访问的1K基地址
			//每次写1024个地址
		  for (i=0; i<1024; i++)
			{ //写 SRAM
				k = fsmc_addr_base + i;
				k <<= 1;                   //<<1 表示转换为半字地址	
			  fpga_write_SRAM((uint32_t)k, (uint16_t)i); 							
			}			
			j--;
		}
*/
		
    //d. 新命令包已处理,复位标志
    msg_buf_valid = 0;	   
  }
    
  //2. 是否需要访问FPGA
  if (gExec_flag == 1)
  { //已收到主机命令,并送达到FPGA,正查询FPGA是否已准备好所需数据	
	  if (FPGA_FLAG == 0)
		{ //FPGA已准备好所有数据，可以去读取
      gExec_flag = 2;
			//设置指示灯
      led_light_mode[2] = 0;    //灭蓝色灯
		  LED_BLUE_OFF;
		  led_light_mode[1] = 2;    //闪烁绿色灯   
      LED_GREEN_ON;  		
		}					
	}
  else if (gExec_flag == 2)
  { //FPGA已准备好数据,ARM正从FPGA读采样数据
		//每一次主循环读1K个点并发送
    if (samples_K_remain == 0)
		{ //已全部发送完毕或无需发送
			//恢复指示
			gExec_flag = 0;
      led_light_mode[1] = 0;    //灭绿色灯
		  LED_GREEN_OFF;
		  led_light_mode[0] = 2;    //闪烁红色灯  			
		}	
    else
	 	{ //有需要发送					
			if (samples_K_remain == gSamples_K)
			{ //第一次发送, 先发送帧头
				usart4.send_byte(0x02);			
			}
			//计算当前1K采样点的SRAM基地址
			fsmc_addr_base = gSamples_K - samples_K_remain;
			fsmc_addr_base <<= 10;       //16bit访问的1K基地址
			//每次读并发送1K个采样点(1024个地址)
			for (i=0; i<1024; i++)
			{ 
				//读FPGA
				k = fsmc_addr_base + i;
				k <<= 1;                    //<<1 表示转换为半字地址					
				fsmc_val = fpga_read_SRAM(k);						  
        usart4.send_byte((fsmc_val >> 8) & 0xFF);	   //发送高字节			  
        usart4.send_byte(fsmc_val & 0xFF);					 //发送低字节
			}			
			//置余量
			samples_K_remain--;
			if (samples_K_remain == 0)
			{ //已全部发送完成,发送帧尾
				usart4.send_byte(0x03);	
			}								
		}			
	}		
  
	
  //2. USART 队列发送
/*	
  if (uart_q_index == 0xFF)         
  { //USART无进入发送流程的队列项.         
		for (i=0; i<UART_QUEUE_NUM; i++)
		{ 
			if (uart_q[i].flag == 1)
			{ //有等待发送的项，安排此项发送
				uart_q[i].flag = 2;
				uart_q_index = i;
				memcpy(&trans_buf[0], &uart_q[i].tdata[0], uart_q[i].len - 1);   
				trans_size = uart_q[i].len; 		  	  		 
				uart_start_trans();
				break;
			}
		}    
  }
*/	
}//end FUNC()


/*F**************************************************************************
* NAME: uart_start_trans
*----------------------------------------------------------------------------
* PARAMS:    
* return:  None
*----------------------------------------------------------------------------
* PURPOSE: 开始串口发送
*----------------------------------------------------------------------------
* REQUIREMENTS:	 使用顺序如下
*                a. while (trans?_occupy), 等待前一个发送完
*                b. 填trans?_buf[] 
*                c. 置trans?_size
*                d. 调用本函数
* Note： 若使用USART发送队列，则所有发送有comm_task()处理。
*****************************************************************************/
/*
void uart_start_trans(void)
{   
  trans_occupy = 1;       
  trans_chksum = 0;  
  trans_ctr = 0;   
  //USART_SendData(, trans_buf[0]);           //自动清上次的TXE/TC 
  //USART_ITConfig(, USART_IT_TC, DISABLE);   //ZZX: need ? 
  //USART_ITConfig(, USART_IT_TXE, ENABLE);   //仅允许TXE中断  
}
*/
