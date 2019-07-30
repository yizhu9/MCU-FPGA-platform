LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY go_in IS
   PORT(clk1,clr,clk0,qa_1,qb_1,qc_1,qd_1:IN STD_LOGIC;
   go:OUT STD_LOGIC_VECTOR(9 DOWNTO 0) );
END ENTITY;

ARCHITECTURE rtl OF go_in IS  
SIGNAL q1: STD_LOGIC_VECTOR(3 DOWNTO 0)	;
  BEGIN
    q1(0)<=qa_1;
    q1(1)<=qb_1;
    q1(2)<=qc_1;
    q1(3)<=qd_1;
  PROCESS(clk0)
  BEGIN			 
  If (clr='0')THEN	 
	  go<="1111111111"	 ;
	  ELSIF (clk0'EVENT AND clk0='0')THEN
  CASE q1 IS
    WHEN "0001" => go(0)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "0010" => go(1)<=NOT clk1;go(0)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "0011" => go(2)<=NOT clk1;go(1)<='1';go(3)<='1';go(4)<='1';go(0)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "0100" => go(3)<=NOT clk1;go(1)<='1';go(2)<='1';go(4)<='1';go(0)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "0101" => go(4)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(0)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "0110" => go(5)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(0)<='1';go(6)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "0111" => go(6)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(0)<='1';go(5)<='1';go(7)<='1';go(8)<='1';go(9)<='1';
    WHEN "1000" => go(7)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(0)<='1';go(5)<='1';go(6)<='1';go(8)<='1';go(9)<='1';
    WHEN "1001" => go(8)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(0)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(9)<='1';
    WHEN "1010" => go(9)<=NOT clk1;go(1)<='1';go(2)<='1';go(3)<='1';go(4)<='1';go(0)<='1';go(5)<='1';go(6)<='1';go(7)<='1';go(8)<='1';
	 WHEN OTHERS => NULL;
  END CASE;	   
  END if;			  
  END PROCESS;
end rtl;
    
    
          