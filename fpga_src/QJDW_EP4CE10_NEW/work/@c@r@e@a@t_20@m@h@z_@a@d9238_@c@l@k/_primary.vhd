library verilog;
use verilog.vl_types.all;
entity CREAT_20MHZ_AD9238_CLK is
    generic(
        AD9238_ONE_SAMPLE_DAT_NUM: integer := 20000000
    );
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        ad9238_start_one_sample_flag: in     vl_logic;
        ad9238_clk      : out    vl_logic;
        ad9238_one_sample_over: out    vl_logic
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of AD9238_ONE_SAMPLE_DAT_NUM : constant is 1;
end CREAT_20MHZ_AD9238_CLK;
