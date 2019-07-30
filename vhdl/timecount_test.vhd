LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY counttime Is
   PORT(time,width:IN STD_LOGIC_VECTOR(15 DOWNTO 0);
        clk_5,clr:IN STD_LOGIC;
        clk1,stop:OUT STD_LOGIC);
END counttime;

ARCHITECTURE rtl of counttime IS
SIGNAL count,time2:STD_LOGIC_VECTOR(15 DOWNTO 0);
SIGNAL time1,time10:integer;
BEGIN
        time1<=CONV_INTEGER(time);
        time10<=time1*10;
        time2<=CONV_STD_LOGIC_VECTOR(time10,15)-width-'1';
     PROCESS(clk_5)
       BEGIN
        IF(clr='0')THEN
            count<="0000000000000000";
            stop<='0';
        ELSIF(clk_5'EVENT AND clk_5='1')THEN
           IF(count=time2+width)THEN
                count<="0000000000000000";
           ELSIF(count>time2 AND count<time2+width)THEN
			   clk1<='1';
			   stop<='1';
			   count<=count+'1';
			ELSE
                count<=count+'1';
                clk1<='0';
           END IF;
          END IF;
       END PROCESS;
 END rtl;