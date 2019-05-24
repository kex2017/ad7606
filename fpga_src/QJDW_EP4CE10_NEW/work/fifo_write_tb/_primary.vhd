library verilog;
use verilog.vl_types.all;
entity fifo_write_tb is
    generic(
        IDLE            : vl_logic_vector(0 to 4) := (Hi0, Hi0, Hi0, Hi0, Hi0);
        SET_RQ_SIG      : vl_logic_vector(0 to 4) := (Hi0, Hi0, Hi0, Hi0, Hi1);
        SEND_BURST_FINISH: vl_logic_vector(0 to 4) := (Hi0, Hi0, Hi0, Hi1, Hi0)
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of IDLE : constant is 1;
    attribute mti_svvh_generic_type of SET_RQ_SIG : constant is 1;
    attribute mti_svvh_generic_type of SEND_BURST_FINISH : constant is 1;
end fifo_write_tb;
