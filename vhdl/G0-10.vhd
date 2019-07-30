
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY open1 IS
   PORT(clk1,qa,qb,qc,qd,STOP:IN STD_LOGIC;
        qa_1,qb_1,qc_1,qd_1:OUT STD_LOGIC);
END open1;

ARCHITECTURE rtl OF open1 IS
  BEGIN
  qa_1<=qa AND clk1 AND  STOP;
  qb_1<=qb AND clk1 AND  STOP;
  qc_1<=qc AND clk1 AND  STOP;
  qd_1<=qd AND clk1 AND  STOP;
END rtl;
           