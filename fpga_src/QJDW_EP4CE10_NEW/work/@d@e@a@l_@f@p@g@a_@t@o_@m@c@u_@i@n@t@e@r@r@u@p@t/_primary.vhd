library verilog;
use verilog.vl_types.all;
entity DEAL_FPGA_TO_MCU_INTERRUPT is
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        interrupt_sig_in: in     vl_logic;
        one_second_clk_in: in     vl_logic;
        interrupt_sig_out: out    vl_logic
    );
end DEAL_FPGA_TO_MCU_INTERRUPT;
