LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY clr_gen IS
   PORT(sys_rst,clk,sig:IN STD_LOGIC;
   clr,sig_ARM,wr_ram:OUT STD_LOGIC);
END ENTITY;

ARCHITECTURE rtl OF clr_gen IS
SIGNAL count:STD_LOGIC_VECTOR(6 DOWNTO 0):= "0000000";
SIGNAL sig1:STD_LOGIC:= '1';
BEGIN
     PROCESS(clk)
       BEGIN
		  IF(sys_rst='0')then
		   count<="0000000";
        ELSIF(clk'EVENT AND clk='1' AND sig='0')THEN
           IF(count="1100000")THEN           
                clr<='1';
					 sig1<='0';
					 wr_ram<='0';
           ELSE
                count<=count+'1';
					 clr<='0';
					 wr_ram<='1';
           END IF;
          END IF;
          sig_ARM<=sig1;
       END PROCESS;
 END rtl;