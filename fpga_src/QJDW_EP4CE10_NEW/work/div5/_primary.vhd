library verilog;
use verilog.vl_types.all;
entity div5 is
    port(
        reset_n         : in     vl_logic;
        div_enable      : in     vl_logic;
        clkin           : in     vl_logic;
        clkout          : out    vl_logic
    );
end div5;
