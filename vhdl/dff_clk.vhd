LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY dff_clk IS
   PORT(en:IN STD_LOGIC;
        dr:IN STD_LOGIC;
        clk_pub:IN STD_LOGIC;
        clk_vol,clk_time:OUT STD_LOGIC);
END dff_clk;

ARCHITECTURE rtl OF dff_clk IS
BEGIN
  clk_vol <= '0' when dr = '0' else (clk_pub AND (NOT dr) AND (NOT en) );
  clk_time <= '0' when dr = '0' else clk_pub AND dr AND (NOT en);
END architecture;

     
 -- clk_pub AND (NOT dr) AND (NOT en)   
 -- clk_pub AND dr AND (NOT en)
 --         clk_time<='0' when dr='1' else (others => '0');         clk_time <= '0' when dr = 'Z
