module READ_CHA_DAT_FROM_SDRAM(
	//input
	clk_100m,
	reset_n,
	clear_count,
	cha,rd,
	fsmc_addr,
	dat_out,
	cha_rdat,
	cha_raddr,
	cha_rd,
	data_avalid,
	sdram_idle);

	input clk_100m;
	input reset_n;

	input clear_count;
	input[3:0]	cha;
	input rd;
	input[2:0] fsmc_addr;


	output[15:0] dat_out;
	input[15:0] cha_rdat;
	output [31:0]cha_raddr;
	output cha_rd;
	input data_avalid;
	input sdram_idle;


	wire clk_100m;
	wire reset_n;


	reg[15:0] dat_out;
	wire[15:0] dat_out_next;

	reg[31:0]cha_dat_count;
	wire [31:0] cha_dat_count_next;
	wire rd;
	wire wr;
	reg clear;
	reg [15:0] indata;
	reg [31:0]cha_raddr;
	wire cha_rd_tmp;
	reg cha_rd;

	parameter CH0_SDRAM_ADDR_BASE    =	32'h000000;
	parameter CH1_SDRAM_ADDR_BASE    =	32'h200000;
	parameter CH2_SDRAM_ADDR_BASE    =	32'h400000;
	parameter CH3_SDRAM_ADDR_BASE    =	32'h600000;



	//检测ARM 读数据的读信号
	DETECT_FSMC_READ_DAT_SIG detect_fsmc_read_dat_sig_t
	(
		//inputs;
		.clk_100m (clk_100m),
		.reset_n (reset_n),
		.fsmc_read (rd),
		.fsmc_addr (fsmc_addr),
		//outpus:
        .read_detect(cha_rd_tmp)
	);
	//assign cha_rd = (fsmc_addr==3'b111) ? cha_rd_tmp:1'b0;
	reg cha_rd_tmp_reg;
	wire cha_rd_tmp_reg_next;
	always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			cha_rd_tmp_reg <= 1'b0;
		else if(cha_rd_tmp)
			cha_rd_tmp_reg <= 1'b1;
		else if(cha_rd)
			cha_rd_tmp_reg <= 1'b0;
		else
			cha_rd_tmp_reg <= cha_rd_tmp_reg_next;
	end
	assign cha_rd_tmp_reg_next = cha_rd_tmp_reg;
	//assign cha_rd = (cha_rd_tmp_reg&(sdram_idle));
	always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			cha_rd <= 1'b0;
		else
		begin
			cha_rd <= (cha_rd_tmp_reg&(sdram_idle));
		end
	end

	/*always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			cha_rd <= 1'h0;
		else
		begin
			if(fsmc_addr==3'b111)
				cha_rd <= cha_rd_tmp;
			else
				cha_rd <= 1'b0;
		end
	end*/

	always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			dat_out <= 15'h0;
		else if(data_avalid)
			dat_out <= cha_rdat;
		else
			dat_out <= dat_out_next;
	end

	assign dat_out_next = dat_out;


	always @( * )
	begin
		case(cha)
		4'b0000://通道0
			cha_raddr = CH0_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		4'b0001://通道1
			cha_raddr = CH1_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		4'b0010://通道2
			cha_raddr = CH2_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		4'b0011://通道3
			cha_raddr = CH3_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		default:
			cha_raddr = CH0_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		endcase
	end


	//每来一个读信号(读reg7)，则将地址往后推1个
	always @( posedge cha_rd or posedge clear_count)
	begin
		if(clear_count)
		begin
			cha_dat_count <= 32'h0;
		end
		else
		begin
			if( (cha_rd)&&(fsmc_addr==3'b111) )
				cha_dat_count <= cha_dat_count_next + 1'b1;
			else
				cha_dat_count <= cha_dat_count_next;
		end
	end

	assign cha_dat_count_next = cha_dat_count;

endmodule


module spi_slave_for_stm32
			(
						//clk,
						clk_200m,
						rst_n,

						ad9238_chip_a_clk,ad9238_chip_a_dclk,
						ad9238_chip_a_data,ad9238_chip_a_cha1_otr,ad9238_chip_a_cha2_otr,
						ad9238_chip_b_clk,ad9238_chip_b_dclk,
						ad9238_chip_b_data,ad9238_chip_b_cha1_otr,ad9238_chip_b_cha2_otr,

						spi_sclk_form_stm32,
						spi_csn_form_stm32,
						spi_mosi_form_stm32,
						spi_miso_to_stm32,
						sdram_buf0_wdat,sdram_buf0_waddr,sdram_buf0_wr,sdram_buf0_idle,
						sdram_buf1_wdat,sdram_buf1_waddr,sdram_buf1_wr,sdram_buf1_idle,
						sdram_buf2_wdat,sdram_buf2_waddr,sdram_buf2_wr,sdram_buf2_idle,
						sdram_buf_rdat,sdram_buf_raddr,sdram_buf_rd,sdram_buf_data_avalid,sdram_idle
		)/* synthesis keep */;
	parameter AD_BIT_WIDTH=12;
	
	input clk_200m;
	input rst_n;
	input spi_sclk_form_stm32;
	input spi_csn_form_stm32;
	input spi_mosi_form_stm32;
	output spi_miso_to_stm32;
	
	output ad9238_chip_a_clk;
	output ad9238_chip_b_clk;
	input ad9238_chip_a_dclk;
	input ad9238_chip_b_dclk;
	input[AD_BIT_WIDTH-1:0] ad9238_chip_a_data;
	input[AD_BIT_WIDTH-1:0] ad9238_chip_b_data;
	input ad9238_chip_a_cha1_otr;
	input ad9238_chip_a_cha2_otr;
	input ad9238_chip_b_cha1_otr;
	input ad9238_chip_b_cha2_otr;
	
	output [15:0]sdram_buf0_wdat;
	output [31:0]sdram_buf0_waddr;
	output sdram_buf0_wr;
	input  sdram_buf0_idle;
	output [15:0]sdram_buf1_wdat;
	output [31:0]sdram_buf1_waddr;
	output sdram_buf1_wr;
	input  sdram_buf1_idle;
	output [15:0]sdram_buf2_wdat;
	output [31:0]sdram_buf2_waddr;
	output sdram_buf2_wr;
	input  sdram_buf2_idle;
	
	input[15:0] sdram_buf_rdat;
	output[31:0] sdram_buf_raddr;
	output sdram_buf_rd;
	input sdram_buf_data_avalid;
	input sdram_idle;
	
	
	wire clk_200m;
	wire rst_n;
	wire spi_sclk_form_stm32;
	wire spi_csn_form_stm32;
	wire spi_mosi_form_stm32;
	reg spi_miso_to_stm32;
	
	wire ad9238_chip_a_clk;
	wire ad9238_chip_b_clk;
	wire ad9238_chip_a_dclk;
	wire ad9238_chip_b_dclk;
	wire[AD_BIT_WIDTH-1:0] ad9238_chip_a_data;
	wire[AD_BIT_WIDTH-1:0] ad9238_chip_b_data;
	wire ad9238_chip_a_cha1_otr;
	wire ad9238_chip_a_cha2_otr;
	wire ad9238_chip_b_cha1_otr;
	wire ad9238_chip_b_cha2_otr;
	
	wire  [15:0]sdram_buf0_wdat;
	wire  [31:0]sdram_buf0_waddr;
	wire  sdram_buf0_wr;
	wire  sdram_buf0_idle;
	wire  [15:0]sdram_buf1_wdat;
	wire  [31:0]sdram_buf1_waddr;
	wire  sdram_buf1_wr;
	wire  sdram_buf1_idle;
	wire  [15:0]sdram_buf2_wdat;
	wire  [31:0]sdram_buf2_waddr;
	wire  sdram_buf2_wr;
	wire  sdram_buf2_idle;
	
	wire[15:0] sdram_buf_rdat;
	wire[31:0] sdram_buf_raddr;
	wire sdram_buf_rd;
	wire sdram_buf_data_avalid;
	wire sdram_idle;
	
	parameter CHANNEL_NUM=3;
	 
	//reg [15:0] OP_CHA_REG;
	reg [15:0] CHA_STA_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] DAT_LEN_H[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] DAT_LEN_L[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] DAT_MAX_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] DAT_AVR_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	//reg [15:0] SYN_FRE_REG[CHANNEL_NUM-1:0];
	reg [15:0] CHA_DAT_ITR[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	 
	reg [15:0] CHA_RUN_REG;// 
	reg [15:0] DAT_THR_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] SAM_CYC_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] SAM_CTR_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] ONE_PHA_DATNUM_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	reg [15:0] AD_BPS_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	
	wire save_breakout_v_dat_to_sdram_flag[3];

	(*preserve*) reg[3:0] cmd_from_stm32/*synthesis noprune*/;
	wire[3:0]cmd_from_stm32_next;
	(*preserve*)reg[3:0] cha_from_stm32/*synthesis noprune*/;
	wire[3:0]cha_from_stm32_next;
	(*preserve*)reg[31:0] dat_receive_from_stm32/*synthesis noprune*/;
	wire[31:0] dat_receive_from_stm32_next;
	(*preserve*)reg[15:0] dat_write_to_stm32/*synthesis noprune*/;
	wire[15:0] dat_write_to_stm32_next;
	
	(*preserve*)reg[7:0] reg_addr_receive_from_stm32/*synthesis noprune*/;
	wire[7:0] reg_addr_receive_from_stm32_next;
	
	(*preserve*)reg  stm32_read_one_new_dat_flag/*synthesis noprune*/;
	
	parameter READ_DAT    					= 4'b0110;
	parameter READ_REG    					= 4'b0101;
	parameter WRITE_REG    					= 4'b0001;
	
	
	parameter IDLE    					= 6'b000001;
	parameter RW_BIT_DAT   				= 6'b000010;
	parameter RW_SUCCESS    			= 6'b000100;
	

	reg[5:0] SPI_SLAVE_CS;
	reg[5:0] SPI_SLAVE_NS;

	wire spi_cs_h2l;
	wire spi_cs_12h;
	wire spi_sclk_l2h;
	wire spi_sclk_h2l;
	wire[15:0]q_read_from_fifo;
	
	always @( * )
	begin
		case(SPI_SLAVE_CS)
		IDLE://????
		begin
			if( spi_cs_h2l )
				SPI_SLAVE_NS <= RW_BIT_DAT;
			else
				SPI_SLAVE_NS <= SPI_SLAVE_CS;
		end
		RW_BIT_DAT://??????????
			if(spi_cs_12h)
				SPI_SLAVE_NS <= RW_SUCCESS;
			else
				SPI_SLAVE_NS <= SPI_SLAVE_CS;
		RW_SUCCESS://????? ???????????????
				SPI_SLAVE_NS <= IDLE;
		default:
			SPI_SLAVE_NS <= IDLE;
		endcase
	end
	
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			SPI_SLAVE_CS <= IDLE;
		else
			SPI_SLAVE_CS <= SPI_SLAVE_NS;
	end
	

	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			dat_write_to_stm32 <= 0;
		else
		begin
			if( (spi_clk_cnt==16)&&(cmd_from_stm32==READ_REG)&&(spi_sclk_l2h) )
			begin
				case(reg_addr_receive_from_stm32)
					3'b000:dat_write_to_stm32 <=  CHA_RUN_REG;
					3'b001:dat_write_to_stm32 <=  CHA_STA_REG[cha_from_stm32];
					3'b010:dat_write_to_stm32 <=  DAT_LEN_H[cha_from_stm32];
					3'b011:dat_write_to_stm32 <=  DAT_LEN_L[cha_from_stm32];
					3'b100:dat_write_to_stm32 <=  DAT_MAX_REG[cha_from_stm32];
					3'b101:dat_write_to_stm32 <=  DAT_AVR_REG[cha_from_stm32];
				/*	3'b110:dat_write_to_stm32 <=  sync_clk_fre;
					3'b111:dat_write_to_stm32 <=  data_read_from_sdram;*/
					default:;
				endcase
			end
			else if( (spi_clk_cnt==16)&&(cmd_from_stm32==WRITE_REG)&&(spi_sclk_l2h) )
			begin
				case (reg_addr_receive_from_stm32)
					3'b000:CHA_RUN_REG <=  dat_receive_from_stm32[15:0];
					3'b001:DAT_THR_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b010:SAM_CYC_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b011:SAM_CTR_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b100:ONE_PHA_DATNUM_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b101:AD_BPS_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					default:;
				endcase
			end
			else if( stm32_read_one_new_dat_flag )
				dat_write_to_stm32 <= q_read_from_fifo;
			else if( (SPI_SLAVE_CS == RW_BIT_DAT )&&(spi_sclk_h2l) )
				dat_write_to_stm32 <= {dat_write_to_stm32_next[14:0],1'b0};
			else if( SPI_SLAVE_CS == RW_SUCCESS )
				dat_write_to_stm32 <= dat_receive_from_stm32;
			else
				dat_write_to_stm32 <= dat_write_to_stm32_next;
		end
	end
	assign dat_write_to_stm32_next = dat_write_to_stm32;
	
	
	always @(posedge clk_200m)
		spi_miso_to_stm32 <= dat_write_to_stm32[15];
	
	
	reg new_spi_data_avalible;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			new_spi_data_avalible <= 1'b0;
		else
		begin
			 if( SPI_SLAVE_CS == RW_SUCCESS )
			begin
				new_spi_data_avalible <= 1'b1;
			end
			else
				new_spi_data_avalible <= 1'b0;
		end
	end
	
	
	
		
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			dat_receive_from_stm32 <= 0;
		else
		begin
			if( (SPI_SLAVE_CS == RW_BIT_DAT )&&(spi_sclk_l2h))
				dat_receive_from_stm32 <= {dat_receive_from_stm32_next[30:0],spi_mosi_form_stm32};
			else
				dat_receive_from_stm32 <= dat_receive_from_stm32_next;
		end
	end
	assign dat_receive_from_stm32_next = dat_receive_from_stm32;
	
	


	reg[31:0] spi_clk_cnt/*synthesis noprune*/;
	wire[31:0]spi_clk_cnt_next;
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			spi_clk_cnt <= 0;
		else
		begin
			if( (SPI_SLAVE_CS == RW_BIT_DAT )&&(spi_sclk_l2h))
				spi_clk_cnt <= spi_clk_cnt_next + 1'b1;
			else if( (SPI_SLAVE_CS == IDLE)||(SPI_SLAVE_CS == RW_SUCCESS) )
				spi_clk_cnt <= 0;
		end
	end
	assign spi_clk_cnt_next = spi_clk_cnt;
	
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			cmd_from_stm32 <= 1;
		else
		begin
			if( ( spi_sclk_l2h )&&(spi_clk_cnt==4))
				cmd_from_stm32 <= dat_receive_from_stm32[3:0];
			else
				cmd_from_stm32 <= cmd_from_stm32_next;
		end
	end
	assign cmd_from_stm32_next = cmd_from_stm32;
	
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			cha_from_stm32 <= 0;
		else
		begin
			if( ( spi_sclk_l2h )&&(spi_clk_cnt==8))
				cha_from_stm32 <= dat_receive_from_stm32[3:0];
			else
				cha_from_stm32 <= cha_from_stm32_next;
		end
	end
	assign cha_from_stm32_next = cha_from_stm32;
	
	reg[23:0] addr_from_stm32;
	wire[23:0]addr_from_stm32_next;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			addr_from_stm32 <= 0;
		else
		begin
			if( ( spi_sclk_l2h )&&(spi_clk_cnt==32))
				addr_from_stm32 <= dat_receive_from_stm32[23:0];
			else
				addr_from_stm32 <= addr_from_stm32_next;
		end
	end
	assign addr_from_stm32_next = addr_from_stm32;
	

	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			reg_addr_receive_from_stm32 <= 0;
		else
		begin
			if( /*( spi_sclk_l2h )&&*/(spi_clk_cnt==16)&&(cmd_from_stm32[0]) )
				reg_addr_receive_from_stm32 <= dat_receive_from_stm32[7:0];
			else
				reg_addr_receive_from_stm32 <= reg_addr_receive_from_stm32_next;
		end
	end
	assign reg_addr_receive_from_stm32_next = reg_addr_receive_from_stm32;
	
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			stm32_read_one_new_dat_flag <= 1'b0;
		else
		begin
			if( ( spi_sclk_l2h )&&(spi_clk_cnt>31)&&(spi_clk_cnt[3:0]==0)&&(cmd_from_stm32==READ_DAT) )
				stm32_read_one_new_dat_flag <= 1'b1;
			else
				stm32_read_one_new_dat_flag <= 1'b0;
		end
	end
	
	reg[15:0] test_code;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			test_code <= 0;
		else
		begin
			if( stm32_read_one_new_dat_flag )
				test_code <= test_code + 2;
			else
				test_code <= test_code;
		end
	end
	

	DETECT_L2H_SIG2 detect_spi_sclk_l2h
	(	
		.clk_100m(clk_200m),
		.reset_n(rst_n),
		.sig(spi_sclk_form_stm32),
		.detect_sig_l2h(spi_sclk_l2h)
	);
	
	DETECT_H2L_SIG detect_spi_sclk_h2l
	(	
		.clk_100m(clk_200m),
		.reset_n(rst_n),
		.sig(spi_sclk_form_stm32),
		.detect_sig_h2l(spi_sclk_h2l)
	);
	
	
	DETECT_L2H_SIG2 detect_spi_cs_l2h
	(	
		.clk_100m(clk_200m),
		.reset_n(rst_n),
		.sig(spi_csn_form_stm32),
		.detect_sig_l2h(spi_cs_12h)
	);
	
	
	DETECT_H2L_SIG detect_spi_cs_h2l
	(	
		.clk_100m(clk_200m),
		.reset_n(rst_n),
		.sig(spi_csn_form_stm32),
		.detect_sig_h2l(spi_cs_h2l)
	);
	
	
	wire[15:0] data_read_from_sdram;
	/*reg[15:0] data_read_from_sdram;
	reg write_ad_dat_to_spi_read_fifo_en;
	wire[2:0]spi_read_fifo_usedw;
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			write_ad_dat_to_spi_read_fifo_en <= 1'b0;
			data_read_from_sdram <= 0;
		end
		else
		begin
			if( spi_read_fifo_usedw < 6 )
			begin
				write_ad_dat_to_spi_read_fifo_en <= 1'b1;
				data_read_from_sdram <= data_read_from_sdram + 3;
			end
			else
			begin
				write_ad_dat_to_spi_read_fifo_en <= 1'b0;
				data_read_from_sdram <= data_read_from_sdram;
			end
		end
	end
	
	sc_fifo_16w_8d spi_read_debug_dat_fifo (
		.clock (clk_200m),//
		.data (data_read_from_sdram),//
		.q (q_read_from_fifo),
		.rdreq (stm32_read_one_new_dat_flag),//
		.wrreq (write_ad_dat_to_spi_read_fifo_en),//
		.usedw (spi_read_fifo_usedw),//
	);*/
	
	wire[15:0]cha0_data_read_from_sdram;
	wire[15:0]cha0_dat_read_from_fifo;
	reg stm32_read_cha0_one_new_dat_flag;
	wire write_cha0_dat_to_spi_read_fifo_en;
	wire[2:0]spi_read_cha0_dat_fifo_usedw;
	
	
	
	sc_fifo_16w_8d spi_read_cha0_dat_fifo (
		.clock (clk_200m),//
		.data (cha0_data_read_from_sdram),//
		.q (cha0_dat_read_from_fifo),
		.rdreq (stm32_read_cha0_one_new_dat_flag),//
		.wrreq (write_cha0_dat_to_spi_read_fifo_en),//
		.usedw (spi_read_cha0_dat_fifo_usedw),//
	);
	
	
	wire[15:0]cha1_data_read_from_sdram;
	wire[15:0]cha1_dat_read_from_fifo;
	reg stm32_read_cha1_one_new_dat_flag;
	wire write_cha1_dat_to_spi_read_fifo_en;
	wire[2:0]spi_read_cha1_dat_fifo_usedw;
	sc_fifo_16w_8d spi_read_cha1_dat_fifo (
		.clock (clk_200m),//
		.data (cha1_data_read_from_sdram),//
		.q (cha1_dat_read_from_fifo),
		.rdreq (stm32_read_cha1_one_new_dat_flag),//
		.wrreq (write_cha1_dat_to_spi_read_fifo_en),//
		.usedw (spi_read_cha1_dat_fifo_usedw),//
	);
	
	
	wire[15:0]cha2_data_read_from_sdram;
	wire[15:0]cha2_dat_read_from_fifo;
	reg stm32_read_cha2_one_new_dat_flag;
	wire write_cha2_dat_to_spi_read_fifo_en;
	wire[2:0]spi_read_cha2_dat_fifo_usedw;
	sc_fifo_16w_8d spi_read_cha2_dat_fifo (
		.clock (clk_200m),//
		.data (cha2_data_read_from_sdram),//
		.q (cha2_dat_read_from_fifo),
		.rdreq (stm32_read_cha2_one_new_dat_flag),//
		.wrreq (write_cha2_dat_to_spi_read_fifo_en),//
		.usedw (spi_read_cha2_dat_fifo_usedw),//
	);
	
	
	reg[15:0]chax_dat_read_from_fifo;
	always @( * )
	begin
		case(cha_from_stm32)
		4'b0000://閫氶亾0
		begin
			chax_dat_read_from_fifo <= cha0_dat_read_from_fifo;
			stm32_read_cha0_one_new_dat_flag <= stm32_read_one_new_dat_flag;	
			stm32_read_cha1_one_new_dat_flag <= 0;
			stm32_read_cha2_one_new_dat_flag <= 0;		
		end
		4'b0001://閫氶亾1
		begin
			chax_dat_read_from_fifo <= cha1_dat_read_from_fifo;
			stm32_read_cha1_one_new_dat_flag <= stm32_read_one_new_dat_flag;
			stm32_read_cha0_one_new_dat_flag <= 0;
			stm32_read_cha2_one_new_dat_flag <= 0;				
		end
		4'b0010://閫氶亾2
		begin
			chax_dat_read_from_fifo <= cha2_dat_read_from_fifo;
			stm32_read_cha2_one_new_dat_flag <= stm32_read_one_new_dat_flag;	
			stm32_read_cha0_one_new_dat_flag <= 0;
			stm32_read_cha1_one_new_dat_flag <= 0;			
		end
		default:
		begin
			chax_dat_read_from_fifo <= cha0_dat_read_from_fifo;
			stm32_read_cha0_one_new_dat_flag <= stm32_read_one_new_dat_flag;	
			stm32_read_cha1_one_new_dat_flag <= 0;
			stm32_read_cha2_one_new_dat_flag <= 0;			
		end
		endcase
	end
	
	CREAT_20MHZ_AD9238_CLK creat_20m_ad9238_clk
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.ad9238_start_one_sample_flag(1'b1/* ad9238_start_one_sample_flag */),
		.ad9238_clk(ad9238_clk),
		.ad9238_one_sample_over(ad9238_one_sample_over)
	);
	
	
	wire ad9238_new_ad_dat_avalible;
	wire[11:0]ad9238_cha0_dat_out;
	wire[11:0]ad9238_cha1_dat_out;
	wire[11:0]ad9238_cha2_dat_out;
	wire[11:0]ad9238_cha3_dat_out;
	READ_AD9238_DAT_FOR_BREAKOUT_V_MOD read_ad9238_dat_for_breakout_v_mod
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.AD9238A_DAT_IN( ad9238_chip_a_data ),
		.AD9238B_DAT_IN( ad9238_chip_b_data ),
		.AD9238_CLK( ad9238_clk ),
		.AD9238_CHA0_DAT_OUT( ad9238_cha0_dat_out ),
		.AD9238_CHA1_DAT_OUT( ad9238_cha1_dat_out ),
		.AD9238_CHA2_DAT_OUT( ad9238_cha2_dat_out ),
		.AD9238_CHA3_DAT_OUT( ad9238_cha3_dat_out ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible )
	);


	parameter AD9238_CHA0    	= 4'h0;
	parameter AD9238_CHA1    	= 4'h1;
	parameter AD9238_CHA2    	= 4'h2;

	
	wire fpga_interrupt_flag[CHANNEL_NUM];
	wire[15:0] breakout_v_dat_max[CHANNEL_NUM-1:0];
	wire[15:0] breakout_v_dat_min[CHANNEL_NUM-1:0];


	WRITE_BREAKOUT_V_DAT_TO_SDRAM write_cha0_breakout_v_dat_to_sdram
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.cha( AD9238_CHA0 ),
		.ad9238_chax_dat( ad9238_cha0_dat_out ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible ),
		.ad9238_chax_thr( DAT_THR_REG[0] ),
		.clear_flag_from_mcu( CHA_RUN_REG[4] ),
		.save_breakout_v_dat_to_sdram_flag( save_breakout_v_dat_to_sdram_flag[0]  ),
		.fpga_interrupt_flag( fpga_interrupt_flag[0] ),
		.breakout_v_dat_max( breakout_v_dat_max[0] ),
		.breakout_v_dat_min( breakout_v_dat_min[0] ),
		.sdram_write_en( sdram_buf0_wr ),
		.sdram_data( sdram_buf0_wdat ),
		.sdram_addr( sdram_buf0_waddr ),
		.sdram_dev_idle(sdram_buf0_idle)
	);

	WRITE_BREAKOUT_V_DAT_TO_SDRAM write_cha1_breakout_v_dat_to_sdram
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.cha( AD9238_CHA1 ),
		.ad9238_chax_dat( ad9238_cha1_dat_out ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible ),
		.ad9238_chax_thr( DAT_THR_REG[1] ),
		.clear_flag_from_mcu( CHA_RUN_REG[5] ),
		.save_breakout_v_dat_to_sdram_flag( save_breakout_v_dat_to_sdram_flag[1]  ),
		.fpga_interrupt_flag( fpga_interrupt_flag[1] ),
		.breakout_v_dat_max( breakout_v_dat_max[1] ),
		.breakout_v_dat_min( breakout_v_dat_min[1] ),
		.sdram_write_en( sdram_buf1_wr ),
		.sdram_data( sdram_buf1_wdat ),
		.sdram_addr( sdram_buf1_waddr ),
		.sdram_dev_idle(sdram_buf1_idle)
	);

	WRITE_BREAKOUT_V_DAT_TO_SDRAM write_cha2_breakout_v_dat_to_sdram
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.cha( AD9238_CHA2 ),
		.ad9238_chax_dat( ad9238_cha2_dat_out ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible ),
		.ad9238_chax_thr( DAT_THR_REG[2] ),
		.clear_flag_from_mcu( CHA_RUN_REG[6] ),
		.save_breakout_v_dat_to_sdram_flag( save_breakout_v_dat_to_sdram_flag[2]  ),
		.fpga_interrupt_flag( fpga_interrupt_flag[2] ),
		.breakout_v_dat_max( breakout_v_dat_max[2] ),
		.breakout_v_dat_min( breakout_v_dat_min[2] ),
		.sdram_write_en( sdram_buf2_wr ),
		.sdram_data( sdram_buf2_wdat ),
		.sdram_addr( sdram_buf2_waddr ),
		.sdram_dev_idle(sdram_buf2_idle)
	);
	
	//从SDRAM中读取各个通道的数据
	
	reg clear_read_cnt;
	reg read_chax_dat_from_sdram_en;
	wire[2:0] fsmc_addr;
	READ_CHA_DAT_FROM_SDRAM read_chax_dat_from_sdram
	(
		//inputs;
		.clk_100m (clk_100m),
		.reset_n (reset_n),
		.rd (read_chax_dat_from_sdram_en),
		.fsmc_addr(fsmc_addr),
		.clear_count(clear_read_cnt),
        .cha(cha_from_stm32),
		//outpus:

        .dat_out(data_read_from_sdram),
        .cha_rdat(sdram_buf_rdat),
        .cha_raddr(sdram_buf_raddr),
        .cha_rd(sdram_buf_rd),
        .data_avalid(sdram_buf_data_avalid),
        .sdram_idle(sdram_idle)
	);
	

	
endmodule 