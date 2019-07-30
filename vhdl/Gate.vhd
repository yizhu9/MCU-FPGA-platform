LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY count10en IS
   PORT(dr,clk1,clr,en:IN STD_LOGIC;
        qa,qb,qc,qd:OUT STD_LOGIC);
END count10en;

ARCHITECTURE rtl OF count10en IS
SIGNAL count_4:STD_LOGIC_VECTOR (3 DOWNTO 0);
BEGIN
   qa <=count_4(0);
   qb <=count_4(1);
   qc <=count_4(2);
   qd <=count_4(3);
  PROCESS(clk1,clr)
     BEGIN
        IF(clr ='0')THEN
           count_4<="0000";
        ELSIF(clk1'EVENT AND clk1='1')THEN
           IF(en='1')THEN
             IF(count_4="1010")THEN
                count_4<="0001";
             ELSE
                count_4<=count_4+'1';
             END IF;
           END IF;
        END IF;
  END PROCESS;
END rtl;