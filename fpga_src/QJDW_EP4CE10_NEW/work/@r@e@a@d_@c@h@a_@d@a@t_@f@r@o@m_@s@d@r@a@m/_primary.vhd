library verilog;
use verilog.vl_types.all;
entity READ_CHA_DAT_FROM_SDRAM is
    generic(
        CH0_SDRAM_ADDR_BASE: integer := 0;
        CH1_SDRAM_ADDR_BASE: integer := 2097152;
        CH2_SDRAM_ADDR_BASE: integer := 4194304;
        CH3_SDRAM_ADDR_BASE: integer := 6291456
    );
    port(
        clk_100m        : in     vl_logic;
        reset_n         : in     vl_logic;
        clear_count     : in     vl_logic;
        cha             : in     vl_logic_vector(3 downto 0);
        rd              : in     vl_logic;
        fsmc_addr       : in     vl_logic_vector(2 downto 0);
        dat_out         : out    vl_logic_vector(15 downto 0);
        cha_rdat        : in     vl_logic_vector(15 downto 0);
        cha_raddr       : out    vl_logic_vector(31 downto 0);
        cha_rd          : out    vl_logic;
        data_avalid     : in     vl_logic;
        sdram_idle      : in     vl_logic
    );
    attribute mti_svvh_generic_type : integer;
    attribute mti_svvh_generic_type of CH0_SDRAM_ADDR_BASE : constant is 1;
    attribute mti_svvh_generic_type of CH1_SDRAM_ADDR_BASE : constant is 1;
    attribute mti_svvh_generic_type of CH2_SDRAM_ADDR_BASE : constant is 1;
    attribute mti_svvh_generic_type of CH3_SDRAM_ADDR_BASE : constant is 1;
end READ_CHA_DAT_FROM_SDRAM;
