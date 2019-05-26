library verilog;
use verilog.vl_types.all;
entity DETECT_L2H_SIG is
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        sig             : in     vl_logic;
        detect_sig_l2h  : out    vl_logic
    );
end DETECT_L2H_SIG;
