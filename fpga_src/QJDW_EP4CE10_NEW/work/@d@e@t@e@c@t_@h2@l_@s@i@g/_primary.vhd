library verilog;
use verilog.vl_types.all;
entity DETECT_H2L_SIG is
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        sig             : in     vl_logic;
        detect_sig_h2l  : out    vl_logic
    );
end DETECT_H2L_SIG;
