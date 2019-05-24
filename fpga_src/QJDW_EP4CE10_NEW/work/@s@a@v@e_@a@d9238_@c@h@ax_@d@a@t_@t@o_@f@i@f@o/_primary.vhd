library verilog;
use verilog.vl_types.all;
entity SAVE_AD9238_CHAx_DAT_TO_FIFO is
    generic(
        WRITE_BREAKOUT_V_DAT_CNT: integer := 4096;
        IDLE            : vl_logic_vector(0 to 5) := (Hi0, Hi0, Hi0, Hi0, Hi0, Hi1);
        START           : vl_logic_vector(0 to 5) := (Hi0, Hi0, Hi0, Hi0, Hi1, Hi0);
        PREPARE_DAT_AND_ADDR: vl_logic_vector(0 to 5) := (Hi0, Hi0, Hi0, Hi1, Hi0, Hi0);
        WRITE_EN        : vl_logic_vector(0 to 5) := (Hi0, Hi0, Hi1, Hi0, Hi0, Hi0);
        WAIT_WRITE_SUCCESS: vl_logic_vector(0 to 5) := (Hi0, Hi1, Hi0, Hi0, Hi0, Hi0);
        WRITE_SUCCESS   : vl_logic_vector(0 to 5) := (Hi1, Hi0, Hi0, Hi0, Hi0, Hi0);
        BURST_CNT       : vl_logic_vector(0 to 4) := (Hi1, Hi0, Hi0, Hi0, Hi0)
    );
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        ad9238_new_ad_dat_avalible: in     vl_logic;
        ad9238_chax_dat : in     vl_logic_vector(11 downto 0);
        ad_dat_thr      : in     vl_logic_vector(10 downto 0);
        change_rate_thr : in     vl_logic_vector(15 downto 0);
        write_fifo_en   : out    vl_logic;
        write_fifo_data : out    vl_logic_vector(11 downto 0);
        fifo_idle       : in     vl_logic;
        save_dat_to_fifo_flag: out    vl_logic
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of WRITE_BREAKOUT_V_DAT_CNT : constant is 1;
    attribute mti_svvh_generic_type of IDLE : constant is 1;
    attribute mti_svvh_generic_type of START : constant is 1;
    attribute mti_svvh_generic_type of PREPARE_DAT_AND_ADDR : constant is 1;
    attribute mti_svvh_generic_type of WRITE_EN : constant is 1;
    attribute mti_svvh_generic_type of WAIT_WRITE_SUCCESS : constant is 1;
    attribute mti_svvh_generic_type of WRITE_SUCCESS : constant is 1;
    attribute mti_svvh_generic_type of BURST_CNT : constant is 1;
end SAVE_AD9238_CHAx_DAT_TO_FIFO;
