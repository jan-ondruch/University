-- INP project 1
-- Author: Jan Ondruch, xondru14

-- Library dec -- 
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- Entity ledc8x8 dec --
entity ledc8x8 is

	port (

		SMCLK:	in std_logic;
		RESET:	in std_logic;

		ROW:	out std_logic_vector (7 downto 0);
		LED:	out std_logic_vector (7 downto 0)

	);
end entity ledc8x8;

-- Architecture ledc8x8 --
architecture behavioral of ledc8x8 is
	
	-- Signals
	signal ce:		std_logic;	-- clock enable
	signal ce_d:	std_logic;	-- clock enable delay

	signal row_d:		std_logic_vector (7 downto 0) := "01111111";  -- rows
	signal cnt:		std_logic_vector (21 downto 0);	-- control counter
	signal name:	std_logic_vector (7 downto 0) := "01111111";  -- displayed char
	signal clk:		std_logic_vector (7 downto 0) := "00000000";

	begin

	-- counter implementation 
	edge: process (SMCLK, RESET)

		begin

			if (RESET = '1') then
				clk <= "00000000";

			-- is rising edge
			elsif (SMCLK'event) and (SMCLK = '1') then
				clk <= clk + 1;

			if clk (7 downto 0) = "11111111" then
				ce <= '1';

			else
				ce <= '0';
      
      end if;
      
			end if;

	end process;

	-- row rotation (ce and RESET on input) -- 
	process (ce, RESET, row_d)

		begin

			-- RESET in 1 -> activate the first row
			if (RESET = '1') then
				row_d <= "00000001";

			-- ce'event && ce in 1 -> shift 
			elsif (ce'event) and (ce = '1') then                     
				row_d <= row_d(0) & row_d (7 downto 1);

			end if;                        

	end process;

	-- enables LEDs = turns them on --
	led_enable: process (row_d, SMCLK, ce)

		begin

			if (SMCLK'event) and (SMCLK = '1') and (ce = '1') then
				ROW <= row_d;
				LED <= name;
			end if;

	end process led_enable;
  
  -- delays display of the second char by 0.5 sec --
	delay: process (SMCLK, RESET, cnt, ce_d)

		begin

			if (RESET = '1') then
				cnt <= "0000000000000000000000";
				ce_d <= '0';

			elsif (SMCLK'event) and (SMCLK = '1') then
				cnt <= cnt + 1;

			-- timer set to 40 MHZ
			if cnt (21 downto 0) = "1001100010010110100000" then
				ce_d <= '1';          

			end if;

			end if;

	end process delay;

	-- decoder (the main LED switching logic) --
	decoder: process (SMCLK, row_d, ce_d, RESET)

		begin

			-- reset on the start
			if (RESET = '1') then
				name <= "10000000";

			-- is rising edge
			elsif (SMCLK'event) and (SMCLK = '1') then

				-- ce_d signal - works as a kind of switch
				-- starts in "1" - first char is switched on
				-- after 0,5 sec - set to "0" and the second char is on	
				if (ce_d = '0') then

					case row_d is
						when "00000001" => name <= "11111111";	-- row 1
						when "00000010" => name <= "11111011";	-- row 2
						when "00000100" => name <= "11111011";	-- row 3
						when "00001000" => name <= "11111011";	-- row 4
						when "00010000" => name <= "11111010";	-- row 5
						when "00100000" => name <= "11111000";	-- row 6
						when "01000000" => name <= "11111111";	-- row 7
						when "10000000" => name <= "11111111";	-- row 8
						when others		=> name <= "11111111";	-- others
					end case;

				elsif (ce_d = '1') then

					case row_d is
						when "00000001" => name <= "00011111";	-- row 1
						when "00000010" => name <= "11111011";	-- row 2
						when "00000100" => name <= "11111011";	-- row 3
						when "00001000" => name <= "11111011";	-- row 4
						when "00010000" => name <= "00011010";	-- row 5
						when "00100000" => name <= "01011000";	-- row 6
						when "01000000" => name <= "01011111";	-- row 7
						when "10000000" => name <= "01011111";	-- row 8
						when others 	=> name <= "11111111";	-- others
					end case;

				end if;

			end if;

	end process decoder;

end behavioral;