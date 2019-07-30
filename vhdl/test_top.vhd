
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY test IS

   PORT(
		ad_busy,inclk0,FSMC_CLK,NADV,nWR,nRD,NE1: IN STD_LOGIC  ;
        go:OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
        DAC_D:OUT STD_LOGIC_VECTOR(11 DOWNTO 0);
		  ad_data:IN STD_LOGIC_VECTOR(13 DOWNTO 0);
		ad_cs,nDAC_WR,nDAC_CS,ad_wr,conv,NWAIT,nSRAM_CE,nSRAM_OE,sig_ARM,nSRAM_WE,finish:OUT STD_LOGIC;
		ADDRH:INOUT STD_LOGIC_VECTOR(9 DOWNTO 0);
       AD:INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
        SRAM_A:OUT STD_LOGIC_VECTOR(19 DOWNTO 0);
        SRAM_D:INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		  LED,samp_complete,AC573_LE,FPGA_F5,FPGA_J6,FPGA_P8,FPGA_M7,FPGA_N2:OUT STD_LOGIC;
		  
		  clk_50M:OUT STD_LOGIC
		) ;
   
   
END test;


ARCHITECTURE rtl OF test IS

COMPONENT FSMW IS
	PORT(samp,wr ,RD   ,NE1    : IN STD_LOGIC ;
		clock_FSM   : IN STD_LOGIC ;
		clr		   : IN STD_LOGIC;
		final     	: OUT STD_LOGIC ;
		cs_adc		: OUT STD_LOGIC ;
		CNVST_RD_adc: OUT STD_LOGIC ;
		data        : IN STD_LOGIC_VECTOR (13 DOWNTO 0);
		data_out_adc: INOUT STD_LOGIC_VECTOR (15 DOWNTO 0);
		address		: OUT STD_LOGIC_VECTOR (19 DOWNTO 0);
		cs_ram      : OUT STD_LOGIC;
		we_ram		: OUT STD_LOGIC);
   END COMPONENT;
	  
Component fre_div1 Is
   PORT(inclk0,clr:IN STD_LOGIC;
        clk_wid:OUT STD_LOGIC);
END Component;

Component wr_ARM IS
   PORT(clk,FSMC_CLK,NADV,WR,RD,NE1,wr_FSNW,wr_ra:IN STD_LOGIC;
   SRAM_DATA:IN STD_LOGIC_VECTOR(15 DOWNTO 0);
   ad1:INOUT STD_LOGIC_VECTOR(9 DOWNTO 0);
   ad2:INOUT STD_LOGIC_VECTOR(15 DOWNTO 0);
   NWAIT,SRAM_OE,sig:OUT STD_LOGIC;
   vol,width,fre:OUT STD_LOGIC_VECTOR(11 DOWNTO 0));
END Component;

Component fre_div Is
   PORT(inclk0,clr:IN STD_LOGIC;
        clk_gate:OUT STD_LOGIC);
END Component;

Component clr_gen IS
   PORT(sys_rst,clk,sig:IN STD_LOGIC;
   clr,sig_ARM,wr_ram:OUT STD_LOGIC );
END Component;

  Component pll1 IS
	PORT
	(
		inclk0		: IN STD_LOGIC  ;
		c0		: OUT STD_LOGIC ;
		locked		: OUT STD_LOGIC 
	);
END Component;
 
   Component open1 IS
   PORT(clk1,qa,qb,qc,qd,STOP:IN STD_LOGIC;
        qa_1,qb_1,qc_1,qd_1:OUT STD_LOGIC);
END Component;

Component go_in IS
   PORT(clk1,clr,clk0,qa_1,qb_1,qc_1,qd_1:IN STD_LOGIC;
   go:OUT STD_LOGIC_VECTOR(9 DOWNTO 0));
END Component;

Component counttime Is
   PORT(time,width:IN STD_LOGIC_VECTOR(15 DOWNTO 0);
        clk_5,clr:IN STD_LOGIC;
        clk1,stop:OUT STD_LOGIC);
END Component;


Component count10en IS
   PORT(dr,clk1,clr,en:IN STD_LOGIC;
        qa,qb,qc,qd:OUT STD_LOGIC);
END Component;

Component IntF IS
     PORT
     (	 c0,wr:		IN  STD_LOGIC;        		 
          		
	     ADDRH      : IN STD_LOGIC_VECTOR(25 downto 16);
	     
	     AD         : INOUT STD_LOGIC_VECTOR(15 downto 0);
	     
	     nADV,FSMW_ce  : IN  STD_LOGIC;              

         nE1        : IN  STD_LOGIC;                 
         nWR        : IN  STD_LOGIC;                  
         nRD        : IN  STD_LOGIC;   
         nWAIT,sig,rst      : OUT STD_LOGIC;                            

         -- for DAC
		 DAC_D      : OUT STD_LOGIC_VECTOR(11 downto 0);
		 nDAC_CS    : OUT STD_LOGIC;
         nDAC_WR    : OUT STD_LOGIC;		 
		 
		 -- for info ARM
		 samp_complete : INOUT STD_LOGIC;
		 
		 -- for SRAM
		 SRAM_D     : INOUT STD_LOGIC_VECTOR(15 downto 0);
		 SRAM_A     : OUT STD_LOGIC_VECTOR(19 downto 0);
		 nSRAM_CE   : OUT STD_LOGIC;
		 nSRAM_WE   : OUT STD_LOGIC;
		 nSRAM_OE   : OUT STD_LOGIC;		 		 		 
		 
		 --used for debug
		 AC573_LE   : OUT STD_LOGIC;		 
		 
		 --used for debug
		 FPGA_F5    : OUT STD_LOGIC;
		 FPGA_J6    : OUT STD_LOGIC;
		 FPGA_P8    : OUT STD_LOGIC;
		 FPGA_M7    : OUT STD_LOGIC;
		 FPGA_N2    : OUT STD_LOGIC;
       fre,width: OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		 fin       : OUT STD_LOGIC;
		clr		   : IN STD_LOGIC;
		CNVST_RD_adc,cs_adc: OUT STD_LOGIC ; 
		data        : IN STD_LOGIC_VECTOR (13 DOWNTO 0) 
     );
END Component;

SIGNAL dr,en1:STD_LOGIC;
SIGNAL clk_gate,clr,locked,clk_vol,clk_time,q1,q2,q3,q4,clk_wid,final:STD_LOGIC;
SIGNAL stop,c1,c0,qa,qb,qc,qd,sig,wr_ram,CNVST_RD_adc,wr_ra,FSMW_ce,samp,rst:STD_LOGIC;
SIGNAL fre,width:STD_LOGIC_VECTOR(15 DOWNTO 0);

BEGIN
dr<='1';
en1<='1';
u0:pll1 PORT MAP(inclk0,c0,locked);
--c0:50M
--u1:FSMW PORT MAP(samp,wr_ram,nRD,NE1,c0,clr,final,ad_cs,CNVST_RD_adc,ad_data,SRAM_D,SRAM_A,FSMW_ce,nSRAM_WE);
u2:count10en PORT MAP(dr,c1,clr,en1,qa,qb,qc,qd);
u3:open1 PORT MAP(clk_wid,qa,qb,qc,qd,STOP,q1,q2,q3,q4);
u4:go_in PORT MAP(c1,clr,c0,q1,q2,q3,q4,go);
u5:counttime PORT MAP(fre,width,clk_wid,clr,c1,stop);
u6:clr_gen PORT MAP(rst,c0,sig,clr,sig_ARM,wr_ram);
u7:fre_div PORT MAP(inclk0,clr,clk_gate);
u9:fre_div1 PORT MAP(c0,clr,clk_wid);
u10:IntF PORT MAP(	        		 
	     c0,wr_ram,ADDRH,AD,nADV,FSMW_ce,nE1,nWR,nRD,nWAIT,sig,rst,DAC_D,nDAC_CS,nDAC_WR,samp,
		  SRAM_D,SRAM_A,nSRAM_CE,nSRAM_WE,nSRAM_OE,
		 AC573_LE,FPGA_F5,FPGA_J6,FPGA_P8,FPGA_M7,FPGA_N2,fre,width,finish,
		clr,CNVST_RD_adc,ad_cs,ad_data
     );

ad_wr<=CNVST_RD_adc;
conv<=CNVST_RD_adc;
LED<='1';
clk_50M<=c1;


end rtl;