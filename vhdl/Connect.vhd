-- Project
   -- Interface Logic
-- Device
   -- EP4CE10F17C8N
-- Date
   -- 2017/04/09 for PCB V1.0
-- Designer


LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;


ENTITY IntF IS
     PORT
     (  c0,wr:		IN  STD_LOGIC;   	
	     ADDRH      : IN STD_LOGIC_VECTOR(25 downto 16);
	     AD         : INOUT STD_LOGIC_VECTOR(15 downto 0);
	     nADV,FSMW_ce       : IN  STD_LOGIC;              

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
		 fin      : OUT STD_LOGIC;
	
--FSMW
       clr		   : IN STD_LOGIC;
		CNVST_RD_adc,cs_adc: OUT STD_LOGIC ; 
		data        : IN STD_LOGIC_VECTOR (13 DOWNTO 0)
     );
END IntF;



ARCHITECTURE Stru OF IntF IS 
   	SIGNAL time_count :STD_LOGIC_VECTOR (5 DOWNTO 0);
      SIGNAL address_coun :STD_LOGIC_VECTOR (19 DOWNTO 0);
      SIGNAL cs_ram,FSMW_ce1,time_rst:STD_LOGIC;
		SIGNAL data_tem :STD_LOGIC_VECTOR (13 DOWNTO 0);
		SIGNAL count:STD_LOGIC_VECTOR (15 DOWNTO 0);
		SIGNAL state:INTEGER;
   	
	-- signal declaration		
	-- for PLL
    signal pll_out : STD_LOGIC;    -- is 50MHz
    
    -- for low 16bit address latch
    signal ADDRL   : STD_LOGIC_VECTOR(15 downto 0);
    
    -- for system reset
    signal sys_rst : STD_LOGIC;    -- low active
    
    -- for SRAM
    signal sram_ce : STD_LOGIC;

    -- for     
    signal width_cs, fre_cs : STD_LOGIC;
	 signal sig_wid, sig_fre : STD_LOGIC;


-- Begin Structure description
BEGIN 	

   sig <= NOT (sig_wid OR sig_fre);
	-- latch low 16Bit address 		
	process (nADV, AD)
	begin
			if (nADV = '0') then
				ADDRL <= AD;
			end if;
	end process;

	-- generate system reset signal			
    sys_rst <= nE1 or nWR or ADDRH(24) or (not ADDRH(23)) or ADDRH(22) or ADDRH(21) or ADDRH(20);				
    rst<=sys_rst;
	-- write voltage amplitude
	DAC_D   <= AD(11 DOWNTO 0);
	nDAC_WR <= nWR;
    nDAC_CS <= nE1 or ADDRH(24) or ADDRH(23) or ADDRH(22) or ADDRH(21) or (not ADDRH(20));   
    
    -- write pulse width
    width_cs <= nE1 or ADDRH(24) or ADDRH(23) or ADDRH(22) or (not ADDRH(21)) or ADDRH(20);       
    process (nWR, sys_rst)
	begin
		if sys_rst = '0' then
			width <= (others => '0');
			sig_wid<='0';
			samp_complete<='1';
		elsif rising_edge(nWR) then
			if width_cs = '0' then
			    width <= AD;
				 sig_wid<='1';
			end if;
		end if;
	end process;
	--AD <= width when (width_cs = '0' and nRD = '0') else (others => 'Z');	 --read from width register
	
    -- write pulse period
    fre_cs <= nE1 or ADDRH(24) or ADDRH(23) or (not ADDRH(22)) or ADDRH(21) or ADDRH(20);       
    process (nWR, sys_rst)
	begin
		if sys_rst = '0' then
			fre <= (others => '0');
			sig_fre <= '0';
		elsif rising_edge(nWR) then
			if fre_cs = '0' then
			    fre <= AD;
				 sig_fre <= '1';
			end if;
		end if;
	end process;
	--AD <= fre when (fre_cs = '0' and nRD = '0') else (others => 'Z');	 --read from fre register	
    
                     									
--	PROCESS(nWR)
--	BEGIN
--    IF (nWR'event AND nWR='1' AND nE1='0') THEN  	    
--        IF (ADDRH(23 DOWNTO 20) = "0001") THEN 
--                vol <= AD(11 DOWNTO 0);
--				  sig_vol <= '0'; 
--        ELSIF (ADDRH(23 DOWNTO 20) = "0010") THEN 
--				  width <= AD(11 DOWNTO 0);
--				  sig_wid <= '0';
--        ELSIF (ADDRH(23 DOWNTO 20) = "0100") THEN 
--				  fre <= AD(11 DOWNTO 0);
--				  sig_fre <= '0';
--        END IF;
--	  END IF;
--	END PROCESS;	
--  DAC_D <= vol;		
		
	-- for SRAM
	
	  PROCESS(c0)
     BEGIN
     if(clr='0')then
	   fin<='1';
      time_count <="000000";
      address_coun <= "00000000000000000000";
		state<=0;
		time_rst<='1';
	  --elsif(time_count="110010")then time_count<="000000";
     elsif(c0'EVENT AND c0='1' AND address_coun < "11111111111111111111" AND wr='0')then
	     --if (data/="ZZZZZZZZZZZZZZZZ")  then data_tem<=data;end if;
       CASE time_count IS
        WHEN "000001" => cs_adc <='0';CNVST_RD_adc <='1'; SRAM_A<= "ZZZZZZZZZZZZZZZZZZZZ";SRAM_D <= "ZZZZZZZZZZZZZZZZ";cs_ram <='1';nSRAM_WE<='1';
        WHEN "000010" => SRAM_A <= address_coun;cs_ram <='0';nSRAM_WE<='0';CNVST_RD_adc <='0';SRAM_D(13 DOWNTO 0) <= data;SRAM_D(15 DOWNTO 14) <= "00";
        WHEN "001000" => nSRAM_WE<='1';CNVST_RD_adc <='1';
		  WHEN "001001" => cs_ram <= '1';SRAM_A <= "ZZZZZZZZZZZZZZZZZZZZ";SRAM_D <= "ZZZZZZZZZZZZZZZZ";
        WHEN "110010" => address_coun <= address_coun + '1';time_count<="000000";--time_rst <= '0';
        WHEN OTHERS => NULL;
		 end CASE; 
		  --if(time_rst='0')then
		    --time_rst<='1';
			 --time_count<="000000";
		  --end if;
		  --elsif (state<=8 AND state>2)then SRAM_A <= address_coun;cs_ram <='0';nSRAM_WE<='0';CNVST_RD_adc <='0';SRAM_D(13 DOWNTO 0) <= data;SRAM_D(15 DOWNTO 14) <= "00";
        --elsif (state<=9 AND state>8)then nSRAM_WE<='1';CNVST_RD_adc <='1';
		  --elsif (state<50 AND state>9)then cs_ram <= '1';SRAM_A <= "ZZZZZZZZZZZZZZZZZZZZ";SRAM_D <= "ZZZZZZZZZZZZZZZZ";
        --elsif (state=50) then state <= 1;--address_coun <= address_coun + '1';
       --end if;
		 time_count <= time_count + '1';
		 --state<=state + 1;
		 elsif(c0'EVENT AND c0 ='1' AND address_coun = "11111111111111111111")then
		  fin<='0';
		  --cs_ram <='0';
		  FSMW_ce1 <= '0';
		  --sram_ce  <= (nE1 or ADDRH(24) or ADDRH(23) or ADDRH(22) or ADDRH(21) or ADDRH(20)) AND FSMW_ce1 AND cs_ram; 
	    -- nSRAM_CE <= sram_ce;	
	     SRAM_A(19 downto 16) <= ADDRH(19 downto 16);
	     SRAM_A(15 downto  0) <= ADDRL(15 downto  0);
		  
		  
		end if;
    END PROCESS;
	sram_ce  <= (nE1 or ADDRH(24) or ADDRH(23) or ADDRH(22) or ADDRH(21) or ADDRH(20)) AND FSMW_ce1 AND cs_ram; 
	 nSRAM_CE <= sram_ce;	 nSRAM_OE <= nRD or FSMW_ce1;
		
	--SRAM_D <= AD when (sram_ce = '0' and nWR = '0') else (others => 'Z');	 --write to SRAM
    AD <= SRAM_D when (sram_ce = '0' and nRD = '0') else (others => 'Z');	 --read from SRAM	
				
	-- samp_complete <= '1';
					
   -- AC573_LE <= '1'; 
	PROCESS(nRD)
	begin
	if(sys_rst='0')then
	count<="0000000000000000";
	elsif(nRD'EVENT AND nRD='1' AND sram_ce='0')then
			count<=count+'1';
	end if;
	end PROCESS;
	
    --AD <= count when (sram_ce = '0' and nRD = '0') else (others => 'Z');
    -- nWAIT not used, must always drive to '1'
    nWAIT   <= '1';    

    -- out some signals using unusing FPGA pins
--    FPGA_F5  <= nE1;    
--    FPGA_J6  <= nWR;    
--    FPGA_P8  <= width(0);    
--    FPGA_M7  <= nADV;	
--    FPGA_N2  <= sig_vol or sig_wid or sig_fre;
    

END Stru;
    