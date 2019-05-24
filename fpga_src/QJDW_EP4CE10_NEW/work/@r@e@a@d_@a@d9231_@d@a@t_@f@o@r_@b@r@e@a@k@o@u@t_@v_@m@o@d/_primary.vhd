library verilog;
use verilog.vl_types.all;
entity READ_AD9231_DAT_FOR_BREAKOUT_V_MOD is
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        AD9238A_DAT_IN  : in     vl_logic_vector(11 downto 0);
        AD9238B_DAT_IN  : in     vl_logic_vector(11 downto 0);
        AD9238_CHIP_A_DCLK: in     vl_logic;
        AD9238_CHIP_B_DCLK: in     vl_logic;
        AD9238_CHA0_DAT_OUT: out    vl_logic_vector(11 downto 0);
        AD9238_CHA1_DAT_OUT: out    vl_logic_vector(11 downto 0);
        AD9238_CHA2_DAT_OUT: out    vl_logic_vector(11 downto 0);
        AD9238_CHA3_DAT_OUT: out    vl_logic_vector(11 downto 0);
        ad9238_new_ad_dat_avalible: out    vl_logic
    );
end READ_AD9231_DAT_FOR_BREAKOUT_V_MOD;
