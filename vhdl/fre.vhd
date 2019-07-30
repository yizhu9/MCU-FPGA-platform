
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
Use ieee.std_logic_arith.all; 

ENTITY fre_div Is
   PORT(inclk0,clr:IN STD_LOGIC;
        clk_gate:OUT STD_LOGIC);
END fre_div;

ARCHITECTURE rtl OF fre_div IS
   signal data:integer;
   signal q:STD_LOGIC;
begin
process(inclk0,clr)
  begin
   IF (clr='0')THEN
   data<=0;
   q<='0';
   ELSIf (inclk0'EVENT AND inclk0='1' )then
    if(data=125) then
       data<=0;
      q<=not q;
    else
       data<=data+1;
    end if;
   end if;
   clk_gate<=q;
end process;
end rtl;