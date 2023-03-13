-- Autor reseni: MATEJ OTCENAS, xotcen01

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
    RESET: in std_logic;
    SMCLK: in std_logic;
    LED: out std_logic_vector(7 downto 0);
    ROW: out std_logic_vector(0 to 7)
);
end ledc8x8;

architecture main of ledc8x8 is
-------------- SIGNALS --------------
    signal enable :  std_logic;
    signal half_sec : std_logic_vector(21 downto 0) := (others => '0');
    signal rows : std_logic_vector(7 downto 0);
    signal change_state_en : std_logic_vector(1 downto 0) := (others => '0');
    signal freq_divider : std_logic_vector(7 downto 0) := (others => '0');
-------------------------------------

    -- Sem doplnte definice vnitrnich signalu.

-- FREQUENTION = 7.3728 MHz = 7372800 Hz (ticks per second)
-- ROWS activation = log. 1
-- LED in active row are enabled with log. 0
begin
    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.

    --------------------- DIVIDER ------------------------
    clk_en_generator: process(RESET, SMCLK)
    begin
        if RESET = '1' then
            half_sec <= (others => '0'); -- always reset to 0
            freq_divider <= (others => '0');
        elsif rising_edge(SMCLK) then
            if half_sec = "1110000100000000000000" then -- 0.5 sec
                if change_state_en /= "10" then -- 3.state skip incrementation
                    change_state_en <= change_state_en + 1; -- increment to next state
                end if ;
                half_sec <= (others => '0');
            end if;
            freq_divider <= freq_divider + 1;
            half_sec <= half_sec + 1; 
        end if;
    end process clk_en_generator;

    enable <= '1' when freq_divider = "11111111" else '0';

    register_rotation: process(RESET, SMCLK, enable, rows)
    begin
        if (RESET = '1') then
            rows <= "10000000";
        elsif rising_edge(SMCLK) and enable = '1' then
            case rows is
                when "10000000" => rows <= "01000000";
                when "01000000" => rows <= "00100000";
                when "00100000" => rows <= "00010000";
                when "00010000" => rows <= "00001000";
                when "00001000" => rows <= "00000100";
                when "00000100" => rows <= "00000010";
                when "00000010" => rows <= "00000001";
                when "00000001" => rows <= "10000000";
                when others => null;
            end case;
        end if;
        ROW <= rows;
    end process;

    display: process(rows, change_state_en)
    begin
        if change_state_en = "00" then -- first state is ok
            case rows is
                when "10000000" => LED <= "11101110";
                when "01000000" => LED <= "11100100";
                when "00100000" => LED <= "11101010";
                when "00010000" => LED <= "10001110";
                when "00001000" => LED <= "01101110";
                when "00000100" => LED <= "01101111";
                when "00000010" => LED <= "01101111";
                when "00000001" => LED <= "10011111";
                when others => LED <= "11111111";
            end case;
                
        elsif change_state_en = "01" then  -- second state is shut down
            LED <= "11111111";
        else -- third and also the last state is ok too
            case rows is
                when "10000000" => LED <= "11101110";
                when "01000000" => LED <= "11100100";
                when "00100000" => LED <= "11101010";
                when "00010000" => LED <= "10001110";
                when "00001000" => LED <= "01101110";
                when "00000100" => LED <= "01101111";
                when "00000010" => LED <= "01101111";
                when "00000001" => LED <= "10011111";
                when others => LED <= "11111111";
            end case;
        end if;   
	end process display;
end main;
-- ISID: 75579
