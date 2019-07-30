LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY dff_width IS
   PORT(
        width:IN STD_LOGIC_VECTOR(11 DOWNTO 0);
        clk:IN STD_LOGIC;
        width1:OUT STD_LOGIC_VECTOR(11 DOWNTO 0));
END dff_width;

ARCHITECTURE rtl OF dff_width IS
BEGIN
   PROCESS(clk)
      BEGIN
         IF(clk'EVENT AND clk='0')THEN
            width1<=width;
         END IF;
   END PROCESS;
END rtl;