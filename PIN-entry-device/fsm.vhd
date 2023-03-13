-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (START, TEST1_1, TEST2_1, TEST3_1, TEST4_1, TEST5_1, TEST6_1, TEST7_1, TEST8_1,
   TEST9_1, TEST10_1, TEST11_1, INCORRECT, CORRECT, PRINT_MESSAGE, PRINT_MESSAGE_OK, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= START;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------

--Access code(1) = 14081043184 
--Access code(2) = 1408104318

-- -------------------------------------------------------
-- ------------------- NEXT STATE LOGIC ------------------
-- -------------------------------------------------------

next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
      
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- Deciding between two options of correct input code

   when START =>
      next_state <= START;
      if(KEY(1) = '1') then   -- option 1
         next_state <= TEST2_1;
      elsif (KEY(15) = '1') then -- wrong state
         next_state <= PRINT_MESSAGE; 
      elsif(KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST2_1 =>
      next_state <= TEST2_1;
      if (KEY(4) = '1') then
         next_state <= TEST3_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST3_1 =>
      next_state <= TEST3_1;
      if (KEY(0) = '1') then
         next_state <= TEST4_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST4_1 =>
      next_state <= TEST4_1;
      if (KEY(8) = '1') then
         next_state <= TEST5_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;
      
   when TEST5_1 =>
      next_state <= TEST5_1;
      if (KEY(1) = '1') then
         next_state <= TEST6_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST6_1 =>
      next_state <= TEST6_1;
      if (KEY(0) = '1') then
         next_state <= TEST7_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST7_1 =>
      next_state <= TEST7_1;
      if (KEY(4) = '1') then
         next_state <= TEST8_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST8_1 =>
      next_state <= TEST8_1;
      if (KEY(3) = '1') then
         next_state <= TEST9_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST9_1 =>
      next_state <= TEST9_1;
      if (KEY(1) = '1') then
         next_state <= TEST10_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

   when TEST10_1 =>
      next_state <= TEST10_1;
      if (KEY(8) = '1') then
         next_state <= TEST11_1;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;
   
   when TEST11_1 =>
      next_state <= TEST11_1;
      if (KEY(4) = '1') then -- if 4 is pressed then continue to correct
         next_state <= CORRECT;
      elsif (KEY(15) = '1') then -- anuways if '#' is pressed the second option for input password was given correct and ok message can be printed
         next_state <= PRINT_MESSAGE_OK; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= INCORRECT;
      end if;

-- ------------------------------------------------------------------------------------------
-- -------------------------------- DECIDING STATEMENTS -------------------------------------
-- ------------------------------------------------------------------------------------------

   -- - - - - - - - - - - - - - - - - - - - - - -
   when CORRECT => -- if this state becomes and the key '#'(position 15) is pressed then the next state is to print accept 
      next_state <= CORRECT;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_OK;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= INCORRECT;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when INCORRECT => -- if this state becomes (something went wrong) then the next state is the error message
      next_state <= INCORRECT;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -    
   when PRINT_MESSAGE_OK => -- approval state message
      next_state <= PRINT_MESSAGE_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
   end if;

   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE => -- fail message
      next_state <= PRINT_MESSAGE;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH => -- after the states of error or approval messages are shown, finish state was reached
      next_state <= FINISH;
      if (KEY(15) = '1') then -- the magic key '#' was pressed
         next_state <= START;  -- jump at the beginning of program for secret key ID giving again
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= START;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
-- ----------------------- OUTPUT LOGIC ------------------
-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -

   when PRINT_MESSAGE_OK =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
      FSM_MX_MEM     <= '1'; --print message to pass 
   -- - - - - - - - - - - - - - - - - - - - - - -         
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

