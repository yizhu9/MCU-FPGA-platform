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
     (	 
		 CLK_25M    : IN STD_LOGIC;            		 
          		
	     ADDRH      : IN STD_LOGIC_VECTOR(25 downto 16);
	     
	     AD         : INOUT STD_LOGIC_VECTOR(15 downto 0);
	     
	     nADV       : IN  STD_LOGIC;              

         nE1        : IN  STD_LOGIC;                 
         nWR        : IN  STD_LOGIC;                  
         nRD        : IN  STD_LOGIC;   
         nWAIT      : OUT STD_LOGIC;                            

         -- for DAC
		 DAC_D      : OUT STD_LOGIC_VECTOR(11 downto 0);
		 nDAC_CS    : OUT STD_LOGIC;
         nDAC_WR    : OUT STD_LOGIC;		 
		 
		 -- for info ARM
		 samp_complete : OUT STD_LOGIC;
		 
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
		 FPGA_N2    : OUT STD_LOGIC		 
     );
END IntF;


ARCHITECTURE Stru OF IntF IS 
   	
   	-- component declaration	
    component pll is
	  port
	  (
		inclk0		: in  std_logic  := '0';
		c0	    	: out std_logic 
	  );
    end component;   	
   	
   	
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
    signal width, fre : STD_LOGIC_VECTOR(15 DOWNTO 0);
    signal width_cs, fre_cs : STD_LOGIC;
  --signal sig_vol, sig_wid, sig_fre : STD_LOGIC:='1';


-- Begin Structure description
BEGIN 	

    -- PLL for generate 50MHz clock
    PLL0 : pll 	                         
	       PORT MAP (
	    		    inclk0  => CLK_25M, 		           
		            c0      => pll_out		     
	       );
    	
	-- latch low 16Bit address 		
	process (nADV, AD)
	begin
			if (nADV = '0') then
				ADDRL <= AD;
			end if;
	end process;

	-- generate system reset signal			
    sys_rst <= nE1 or nWR or ADDRH(24) or (not ADDRH(23)) or ADDRH(22) or ADDRH(21) or ADDRH(20);				
  
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
		elsif rising_edge(nWR) then
			if width_cs = '0' then
			    width <= AD;
			end if;
		end if;
	end process;
	AD <= width when (width_cs = '0' and nRD = '0') else (others => 'Z');	 --read from width register
	
    -- write pulse period
    fre_cs <= nE1 or ADDRH(24) or ADDRH(23) or (not ADDRH(22)) or ADDRH(21) or ADDRH(20);       
    process (nWR, sys_rst)
	begin
		if sys_rst = '0' then
			fre <= (others => '0');
		elsif rising_edge(nWR) then
			if fre_cs = '0' then
			    fre <= AD;
			end if;
		end if;
	end process;
	AD <= fre when (fre_cs = '0' and nRD = '0') else (others => 'Z');	 --read from fre register	
    
                     									
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
	sram_ce  <= nE1 or ADDRH(24) or ADDRH(23) or ADDRH(22) or ADDRH(21) or ADDRH(20); 
	nSRAM_CE <= sram_ce;	
	nSRAM_WE <= nWR;
	nSRAM_OE <= nRD;
	
	SRAM_A(19 downto 16) <= ADDRH(19 downto 16);
	SRAM_A(15 downto  0) <= ADDRL(15 downto  0);
		
	SRAM_D <= AD when (sram_ce = '0' and nWR = '0') else (others => 'Z');	 --write to SRAM
    AD <= SRAM_D when (sram_ce = '0' and nRD = '0') else (others => 'Z');	 --read from SRAM	
				
	-- 
	samp_complete <= '1';
					
    AC573_LE <= '1'; 
    
    -- nWAIT not used, must always drive to '1'
    nWAIT   <= '1';    

    -- out some signals using unusing FPGA pins
--    FPGA_F5  <= nE1;    
--    FPGA_J6  <= nWR;    
--    FPGA_P8  <= width(0);    
--    FPGA_M7  <= nADV;	
--    FPGA_N2  <= sig_vol or sig_wid or sig_fre;
    

END Stru;
    