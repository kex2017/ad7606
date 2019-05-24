module CREAT_1S_CLK_FROM_50HZ(clk_100m,reset_n,clk_50hz,clk_1s);
	input clk_100m;
	input reset_n;
	input clk_50hz;
	output clk_1s;


	wire clk_50hz;
	reg clk_1s;

	reg[5:0] count1;
	wire[5:0] count1_next;



    always @ ( posedge clk_50hz )
    begin
		if( count1 >= 49 )
			count1 <= 0;
		else
			count1 <= count1_next + 1'b1;
	end

	assign count1_next = count1;


	always @ ( posedge clk_50hz  )
	begin
		if( count1 <= 24 )
			clk_1s <= 1'b1;
		else 
			clk_1s <= 1'b0;
	end
endmodule


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

	assign cha_rd_tmp = rd;


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
		4'b0000:cha_raddr = CH0_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		4'b0001:cha_raddr = CH1_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		4'b0010:cha_raddr = CH2_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		4'b0011:cha_raddr = CH3_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		default:
			cha_raddr = CH0_SDRAM_ADDR_BASE + {cha_dat_count[30:0],1'b0} - 2'b10;
		endcase
	end



	always @( posedge cha_rd or posedge clear_count)
	begin
		if(clear_count)
		begin
			cha_dat_count <= 32'h0;
		end
		else
		begin
			if( cha_rd )
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
						clk_100m,
						clk_20m,
						rst_n,

						ad9238_chip_a_clk,ad9238_chip_a_dclk,
						ad9238_chip_a_data,ad9238_chip_a_cha1_otr,


						spi_sclk_form_stm32,
						spi_csn_form_stm32,
						spi_mosi_form_stm32,
						spi_miso_to_stm32,
						sdram_buf0_wdat,sdram_buf0_waddr,sdram_buf0_wr,sdram_buf0_idle,
						sdram_buf1_wdat,sdram_buf1_waddr,sdram_buf1_wr,sdram_buf1_idle,
						sdram_buf_rdat,sdram_buf_raddr,sdram_buf_rd,sdram_buf_data_avalid,sdram_idle,
						fpga_interrupt,one_second_clk
		)/*synthesis noprune*/;
	parameter AD_BIT_WIDTH=12;
	
	input wire clk_100m;
	input wire clk_20m;
	input wire rst_n;
	input wire spi_sclk_form_stm32;
	input wire spi_csn_form_stm32;
	input wire spi_mosi_form_stm32;
	output reg spi_miso_to_stm32;
	
	output wire ad9238_chip_a_clk;
	input wire ad9238_chip_a_dclk;
	input wire [AD_BIT_WIDTH-1:0] ad9238_chip_a_data;
	input wire ad9238_chip_a_cha1_otr;

	
	output wire [15:0]sdram_buf0_wdat;
	output wire [31:0]sdram_buf0_waddr;
	output wire sdram_buf0_wr;
	input  wire sdram_buf0_idle;
	output wire [15:0]sdram_buf1_wdat;
	output wire [31:0]sdram_buf1_waddr;
	output wire sdram_buf1_wr;
	input  wire sdram_buf1_idle;
	
	input wire [15:0]sdram_buf_rdat;
	output wire [31:0]sdram_buf_raddr;
	output wire sdram_buf_rd;
	input wire sdram_buf_data_avalid;
	input wire sdram_idle;
	output reg fpga_interrupt;
	input wire one_second_clk;
	
	


	



	reg[15:0]chax_dat_read_from_fifo/*synthesis noprune*/;
	
	
	parameter CHANNEL_NUM=2;
	
	reg fifo_idle[CHANNEL_NUM-1:0];
	
	wire[15:0]cha0_data_read_from_sdram;
	wire[15:0]cha0_dat_read_from_fifo/*synthesis noprune*/;
	reg stm32_read_cha0_one_new_dat_flag;
	wire write_cha0_dat_to_spi_read_fifo_en;
	wire[11:0]spi_read_cha0_dat_fifo_usedw;
	
	wire[11:0] cha0_write_fifo_data;
	wire[11:0] cha1_write_fifo_data;

	wire cha0_save_dat_to_fifo_flag;
	wire cha1_save_dat_to_fifo_flag;
	wire cha0_save_dat_start_time_flag;//CHA0保存数据的开始时间
	wire cha1_save_dat_start_time_flag;//CHA1保存数据的开始时间

	
	reg[31:0] spi_clk_cnt/*synthesis noprune*/;
	wire[31:0]spi_clk_cnt_next;
	 
	//reg [15:0] OP_CHA_REG;
	(*preserve*)reg [15:0] CHA_STA_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)wire [15:0] CHA_STA_REG_next[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] DAT_LEN_H[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] DAT_LEN_L[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] DAT_MAX_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] DAT_AVR_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] CHANGE_RATE_THR[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	//reg [15:0] SYN_FRE_REG[CHANNEL_NUM-1:0];
	(*preserve*)reg [15:0] CHA_DAT_ITR[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	 
	(*preserve*)reg [15:0] CHA_RUN_REG/*synthesis noprune*/;// 
	(*preserve*)reg [15:0] DAT_THR_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] SAM_CYC_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] SAM_CTR_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] ONE_PHA_DATNUM_REG[CHANNEL_NUM-1:0]/*synthesis noprune*/;
	(*preserve*)reg [15:0] ONE_SECOND_CNT_H/*synthesis noprune*/;
	(*preserve*)reg [15:0] ONE_SECOND_CNT_L/*synthesis noprune*/;
	(*preserve*)reg [15:0] CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H/*synthesis noprune*/;
	(*preserve*)reg [15:0] CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L/*synthesis noprune*/;
	(*preserve*)reg [15:0] CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H/*synthesis noprune*/;
	(*preserve*)reg [15:0] CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L/*synthesis noprune*/;
	
	
	wire[15:0]cha1_data_read_from_sdram;
	wire[15:0]cha1_dat_read_from_fifo;
	reg stm32_read_cha1_one_new_dat_flag;
	wire write_cha1_dat_to_spi_read_fifo_en;
	wire[11:0]spi_read_cha1_dat_fifo_usedw;
	
	
	wire[15:0]cha2_data_read_from_sdram;
	wire[15:0]cha2_dat_read_from_fifo;
	reg stm32_read_cha2_one_new_dat_flag;
	wire write_cha2_dat_to_spi_read_fifo_en;
	wire[11:0]spi_read_cha2_dat_fifo_usedw;
	
	wire save_breakout_v_dat_to_sdram_flag[2:0];

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
	
	(*preserve*)reg[15:0]cha0_max/*synthesis noprune*/;
	(*preserve*)reg[15:0]cha1_max/*synthesis noprune*/;
	(*preserve*)reg[15:0]cha2_max/*synthesis noprune*/;
	(*preserve*)reg[15:0]cha3_max/*synthesis noprune*/;
	
	(*preserve*)reg[15:0]cha0_min/*synthesis noprune*/;
	(*preserve*)reg[15:0]cha1_min/*synthesis noprune*/;
	(*preserve*)reg[15:0]cha2_min/*synthesis noprune*/;
	(*preserve*)reg[15:0]cha3_min/*synthesis noprune*/;
	
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
	
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			SPI_SLAVE_CS <= IDLE;
		else
			SPI_SLAVE_CS <= SPI_SLAVE_NS;
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			fifo_idle[0] <= 1;
			fifo_idle[1] <= 1;
		end
		else
		begin
			fifo_idle[0] <= 1;
			fifo_idle[1] <= 1;
		end
	end
	reg[15:0] cha0_thr_reg/*synthesis noprune*/;
	reg[15:0] cha1_thr_reg/*synthesis noprune*/;
	reg[15:0] cha2_thr_reg/*synthesis noprune*/;
	always @(posedge clk_100m )
	begin
		cha0_thr_reg <= DAT_THR_REG[0];
		cha1_thr_reg <= DAT_THR_REG[1];
	end
	
	reg spi_clk_l2h_d3;
	reg spi_clk_l2h_d2;
	reg spi_clk_l2h_d1;
	always @(posedge clk_100m )
	begin
		spi_clk_l2h_d1 <= spi_sclk_l2h;
		spi_clk_l2h_d2 <= spi_clk_l2h_d1;
		spi_clk_l2h_d3 <= spi_clk_l2h_d2;
	end
	
	
	reg[15:0]FPGA_WR_TEST_REG;
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			dat_write_to_stm32 <= 0;
			DAT_THR_REG[0] <= 2047;
			DAT_THR_REG[1] <= 2047;
			CHANGE_RATE_THR[0] = 4095;
			CHANGE_RATE_THR[1] = 4095;

		end
		else
		begin
			if( (spi_clk_cnt==16)&&(cmd_from_stm32==READ_REG)&&(spi_sclk_l2h) )
			begin
				case(reg_addr_receive_from_stm32)
					4'b0000:dat_write_to_stm32 <=  CHA_RUN_REG;
					4'b0001:dat_write_to_stm32 <=  CHA_STA_REG[cha_from_stm32];
					4'b0010:dat_write_to_stm32 <=  DAT_LEN_H[cha_from_stm32];
					4'b0011:dat_write_to_stm32 <=  DAT_LEN_L[cha_from_stm32];
					4'b0100:dat_write_to_stm32 <=  DAT_MAX_REG[cha_from_stm32];
					4'b0101:dat_write_to_stm32 <=  DAT_AVR_REG[cha_from_stm32];
				/*	4'b0110:dat_write_to_stm32 <=  sync_clk_fre;
					4'b0111:dat_write_to_stm32 <=  data_read_from_sdram;*/
					4'b1000:dat_write_to_stm32 <=  ONE_SECOND_CNT_H;
					4'b1001:dat_write_to_stm32 <=  ONE_SECOND_CNT_L;
					4'b1010:dat_write_to_stm32 <=  CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H;
					4'b1011:dat_write_to_stm32 <=  CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L;
					4'b1100:dat_write_to_stm32 <=  CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H;
					4'b1101:dat_write_to_stm32 <=  CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L;
					4'b1110:dat_write_to_stm32 <=  16'h1357;//read test only
					default:;
				endcase
			end
			else if( (spi_clk_cnt==32)&&(cmd_from_stm32==WRITE_REG)&&(spi_clk_l2h_d3) )
			begin
				case (reg_addr_receive_from_stm32)
					3'b001:DAT_THR_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b010:SAM_CYC_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b011:SAM_CTR_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b100:ONE_PHA_DATNUM_REG[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					3'b101:FPGA_WR_TEST_REG <=  dat_receive_from_stm32[15:0];
					3'b110:CHANGE_RATE_THR[cha_from_stm32] <=  dat_receive_from_stm32[15:0];
					default:;
				endcase
			end
			else if( stm32_read_one_new_dat_flag )
				dat_write_to_stm32 <= chax_dat_read_from_fifo;
			else if( (SPI_SLAVE_CS == RW_BIT_DAT )&&(spi_sclk_h2l) )
				dat_write_to_stm32 <= {dat_write_to_stm32_next[14:0],1'b0};
			else if( SPI_SLAVE_CS == RW_SUCCESS )
				dat_write_to_stm32 <= dat_receive_from_stm32;
			else
				dat_write_to_stm32 <= dat_write_to_stm32_next;
		end
	end
	assign dat_write_to_stm32_next = dat_write_to_stm32;
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			CHA_RUN_REG <= 0;
		end
		else
		begin
			if( (spi_clk_cnt==32)&&(cmd_from_stm32==WRITE_REG)&&(spi_clk_l2h_d3) )
			begin
				case (reg_addr_receive_from_stm32)
					3'b000:CHA_RUN_REG <=  dat_receive_from_stm32[15:0];
				endcase
			end
			else
				CHA_RUN_REG[7:4] <= 0;
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			DAT_LEN_H[0] <= 16'h0000;
			DAT_LEN_L[0] <= 16'h0000;
		end
		else
		begin
			if( cha0_save_dat_to_fifo_flag )
			begin
				DAT_LEN_H[0] <= 16'h0000;
				DAT_LEN_L[0] <= 16'h1000;
			end
			else if(CHA_RUN_REG[4])
			begin
				DAT_LEN_H[0] <= 16'h0000;
				DAT_LEN_L[0] <= 16'h0000;
			end
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			DAT_LEN_H[1] <= 16'h0000;
			DAT_LEN_L[1] <= 16'h0000;
		end
		else
		begin
			if( cha1_save_dat_to_fifo_flag )
			begin
				DAT_LEN_H[1] <= 16'h0000;
				DAT_LEN_L[1] <= 16'h1000;
			end
			else if(CHA_RUN_REG[5])
			begin
				DAT_LEN_H[1] <= 16'h0000;
				DAT_LEN_L[1] <= 16'h0000;
			end
		end
	end
	

	
	
	
	always @(posedge clk_100m)
		spi_miso_to_stm32 <= dat_write_to_stm32[15];
	
	
	reg new_spi_data_avalible;
	always @(posedge clk_100m or negedge rst_n)
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
	
	
	
		
	always @(posedge clk_100m or negedge rst_n)
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
	
	


	
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			spi_clk_cnt <= 0;
		else
		begin
			if( (SPI_SLAVE_CS == RW_BIT_DAT )&&(spi_sclk_l2h))
				spi_clk_cnt <= spi_clk_cnt_next + 1'b1;
			else if( (SPI_SLAVE_CS == IDLE)||(SPI_SLAVE_CS == RW_SUCCESS) )
				spi_clk_cnt <= 0;
			else
				spi_clk_cnt <= spi_clk_cnt_next;
		end
	end
	assign spi_clk_cnt_next = spi_clk_cnt;
	
	
	always @(posedge clk_100m or negedge rst_n)
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
	
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			cha_from_stm32 <= 0;
		else
		begin
			if( ( spi_sclk_l2h )&&(spi_clk_cnt==8)&&(dat_receive_from_stm32[3:0]<3) )
				cha_from_stm32 <= dat_receive_from_stm32[3:0];
			else
				cha_from_stm32 <= cha_from_stm32_next;
		end
	end
	assign cha_from_stm32_next = cha_from_stm32;
	
	reg[23:0] addr_from_stm32;
	wire[23:0]addr_from_stm32_next;
	always @(posedge clk_100m or negedge rst_n)
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
	

	always @(posedge clk_100m or negedge rst_n)
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
	
	
	always @(posedge clk_100m or negedge rst_n)
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
	
	reg read_first_dat_from_sdram_for_stm32;
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			read_first_dat_from_sdram_for_stm32 <= 1'b0;
		else
		begin
			if( ( spi_sclk_l2h )&&(spi_clk_cnt==16)&&(cmd_from_stm32==READ_DAT) )
				read_first_dat_from_sdram_for_stm32 <= 1'b1;
			else
				read_first_dat_from_sdram_for_stm32 <= 1'b0;
		end
	end
	
	
	reg[15:0] test_code;
	always @(posedge clk_100m or negedge rst_n)
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
	
	wire one_second_clk_l2h;
	DETECT_L2H_SIG fsmc_one_second_clk_l2h 
	(
		.clk_100m( clk_100m ),
		.reset_n( rst_n ),
		.sig( one_second_clk ),
		.detect_sig_l2h( one_second_clk_l2h )
	);
	
	reg[31:0] one_second_clk_cnt_tmp;
	reg[31:0]one_second_clk_cnt;
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			one_second_clk_cnt_tmp <= 0;
		else
		begin
			if( one_second_clk_l2h )
				one_second_clk_cnt_tmp <= 0;
			else
				one_second_clk_cnt_tmp <= one_second_clk_cnt_tmp + 1'b1;
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			one_second_clk_cnt <= 100000000;
		else
		begin
			if( (one_second_clk_l2h )&&(one_second_clk_cnt_tmp<100010000)&&(one_second_clk_cnt_tmp>99990000))
				one_second_clk_cnt <= one_second_clk_cnt_tmp;
			else
				one_second_clk_cnt <= one_second_clk_cnt;
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			ONE_SECOND_CNT_H <= 0;
			ONE_SECOND_CNT_L <= 0;
		end
		else
		begin
			ONE_SECOND_CNT_H <= one_second_clk_cnt[31:16];
			ONE_SECOND_CNT_L <= one_second_clk_cnt[15:0];
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H <= 0;
			CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L <= 0;
		end
		else
		begin
			if(cha0_save_dat_start_time_flag)
			begin
				CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H <= one_second_clk_cnt[31:16];
				CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L <= one_second_clk_cnt[15:0];
			end
			else
			begin
				CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H <= CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_H;
				CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L <= CHA0_CLK_CNT_FROM_ONE_SECOND_PLUS_L;
			end	
		end
	end
	
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
		begin
			CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H <= 0;
			CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L <= 0;
		end
		else
		begin
			if(cha1_save_dat_start_time_flag)
			begin
				CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H <= one_second_clk_cnt[31:16];
				CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L <= one_second_clk_cnt[15:0];
			end
			else
			begin
				CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H <= CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_H;
				CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L <= CHA1_CLK_CNT_FROM_ONE_SECOND_PLUS_L;
			end	
		end
	end
	

	DETECT_L2H_SIG2 detect_spi_sclk_l2h
	(	
		.clk_100m(clk_100m),
		.reset_n(rst_n),
		.sig(spi_sclk_form_stm32),
		.detect_sig_l2h(spi_sclk_l2h)
	);
	
	DETECT_H2L_SIG detect_spi_sclk_h2l
	(	
		.clk_100m(clk_100m),
		.reset_n(rst_n),
		.sig(spi_sclk_form_stm32),
		.detect_sig_h2l(spi_sclk_h2l)
	);
	
	
	DETECT_L2H_SIG2 detect_spi_cs_l2h
	(	
		.clk_100m(clk_100m),
		.reset_n(rst_n),
		.sig(spi_csn_form_stm32),
		.detect_sig_l2h(spi_cs_12h)
	);
	
	
	DETECT_H2L_SIG detect_spi_cs_h2l
	(	
		.clk_100m(clk_100m),
		.reset_n(rst_n),
		.sig(spi_csn_form_stm32),
		.detect_sig_h2l(spi_cs_h2l)
	);
	

	
	wire[15:0]chax_data_read_from_sdram/*synthesis noprune*/;

	reg stm32_read_chax_one_new_dat_flag;
	wire write_chax_dat_to_spi_read_fifo_en;
	wire[2:0]spi_read_chax_dat_fifo_usedw;
	
	
	
	
	

	wire reset_n;
	
	assign reset_n = rst_n;
	
		
	CREAT_20MHZ_AD9238_CLK creat_20m_ad9238_clk
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.ad9238_start_one_sample_flag(1'b1/* ad9238_start_one_sample_flag */),
		.ad9238_clk(ad9238_chip_a_clk),
		.ad9238_one_sample_over(ad9238_one_sample_over)
	);
	assign ad9238_chip_b_clk = ad9238_chip_a_clk;
	
	
	wire ad9238_new_ad_dat_avalible;
	wire[11:0]ad9238_cha0_dat_out;
	wire[11:0]ad9238_cha1_dat_out;
	READ_AD9231_DAT_FOR_BREAKOUT_V_MOD read_ad9238_dat_for_breakout_v_mod
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.AD9238A_DAT_IN( ad9238_chip_a_data ),
		.AD9238_CHIP_A_DCLK( ad9238_chip_a_dclk ),
		.AD9238_CHA0_DAT_OUT( ad9238_cha0_dat_out ),
		.AD9238_CHA1_DAT_OUT( ad9238_cha1_dat_out ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible )
	);
	reg[7:0] cha0_dat_debug;
	reg[7:0] cha1_dat_debug;
	
	reg cha0_check_flag/*synthesis noprune*/;
	reg cha1_check_flag/*synthesis noprune*/;
	reg[11:0]cha1_low_pass_10m_filter_out_reg/*synthesis noprune*/;
	
	wire[24:0]cha1_low_pass_10m_filter_out;
	
	
	low_pass_10m low_pass_10m
	(
		.clk( clk_20m ),
		.reset_n( reset_n ),
		.ast_sink_data( ad9238_cha1_dat_out-2048 ),
		.ast_sink_valid( 1'b1 ),
		.ast_source_ready( 1'b1 ),
		.ast_sink_error( 2'b00 ),
		.ast_source_data( cha1_low_pass_10m_filter_out )
	);
	always @(posedge clk_20m or negedge rst_n)
	begin
		if(!rst_n)
			cha1_low_pass_10m_filter_out_reg <= 0;
		else
			cha1_low_pass_10m_filter_out_reg <= {cha1_low_pass_10m_filter_out[24],cha1_low_pass_10m_filter_out[21:11]};
	end

	
	always @(posedge clk_20m or negedge rst_n)
	begin
		if(!rst_n)
			cha0_check_flag <= 0;
		else if(ad9238_cha0_dat_out>2100)
			cha0_check_flag <= 1;
		else
			cha0_check_flag <= 0;
	end
	
	always @(posedge clk_20m or negedge rst_n)
	begin
		if(!rst_n)
			cha1_check_flag <= 0;
		else if(ad9238_cha1_dat_out>2100)
			cha1_check_flag <= 1;
		else
			cha1_check_flag <= 0;
	end
	
	always @(posedge clk_100m)
	begin
		cha0_dat_debug <= ad9238_cha0_dat_out-1920;
		cha1_dat_debug <= ad9238_cha1_dat_out-1920;
	end
	
	
	parameter ZLPY = -14;// 
	
	always @(posedge clk_100m)
	begin
		DAT_MAX_REG[0] <= cha0_max-ZLPY;
		DAT_MAX_REG[1] <= cha1_max-ZLPY;

		DAT_AVR_REG[0] <= cha0_min-ZLPY;
		DAT_AVR_REG[1] <= cha1_min-ZLPY;
	end
	
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			cha0_max <= 0;
		else
		begin
			if( CHA_RUN_REG[4]  )
				cha0_max <= 0;
			else if( ad9238_new_ad_dat_avalible )
			begin
				if( cha0_max < ad9238_cha0_dat_out)
					cha0_max <= ad9238_cha0_dat_out;
				else
					cha0_max <= cha0_max;
			end
			else
				cha0_max <= cha0_max;
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			cha0_min <= 4095;
		else
		begin
			if( CHA_RUN_REG[4]  )
				cha0_min <= 4095;
			else if( ad9238_new_ad_dat_avalible )
			begin
				if( cha0_min > ad9238_cha0_dat_out)
					cha0_min <= ad9238_cha0_dat_out;
				else
					cha0_min <= cha0_min;
			end
			else
				cha0_min <= cha0_min;
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			cha1_max <= 0;
		else
		begin
			if( CHA_RUN_REG[5]  )
				cha1_max <= 0;
			else if( ad9238_new_ad_dat_avalible )
			begin
				if( cha1_max < ad9238_cha1_dat_out)
					cha1_max <= ad9238_cha1_dat_out;
				else
					cha1_max <= cha1_max;
			end
			else
				cha1_max <= cha1_max;
		end
	end
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			cha1_min <= 4095;
		else
		begin
			if( CHA_RUN_REG[5]  )
				cha1_min <= 4095;
			else if( ad9238_new_ad_dat_avalible )
			begin
				if( cha1_min > ad9238_cha1_dat_out)
					cha1_min <= ad9238_cha1_dat_out;
				else
					cha1_min <= cha1_min;
			end
			else
				cha1_min <= cha1_min;
		end
	end
	



	parameter AD9238_CHA0    	= 4'h0;
	parameter AD9238_CHA1    	= 4'h1;

	

	wire[15:0] breakout_v_dat_max[CHANNEL_NUM-1:0];
	wire[15:0] breakout_v_dat_min[CHANNEL_NUM-1:0];

	
	wire cha0_fifo_ilde;
	assign cha0_fifo_ilde = (spi_read_cha0_dat_fifo_usedw==0)?1:0;
	wire cha1_fifo_ilde;
	assign cha1_fifo_ilde = (spi_read_cha1_dat_fifo_usedw<4094)?1:0;

		
	

	sc_fifo_12w_4096d spi_read_cha0_dat_fifo (
		.clock (clk_100m),//
		.data (cha0_write_fifo_data),//
		.q (cha0_dat_read_from_fifo),
		.rdreq (stm32_read_cha0_one_new_dat_flag),//
		.wrreq (sdram_buf0_wr),//
		.usedw (spi_read_cha0_dat_fifo_usedw)//
	);
	SAVE_AD9238_CHAx_DAT_TO_FIFO save_ad9238_cha0_dat_to_fifo
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible ),
		.ad9238_chax_dat( ad9238_cha0_dat_out ),
		.ad_dat_thr( DAT_THR_REG[0][11:0]  ),
		.change_rate_thr( CHANGE_RATE_THR[0][11:0] ),
		.write_fifo_en( sdram_buf0_wr ),
		.write_fifo_data( cha0_write_fifo_data ),
		.fifo_idle( CHA_RUN_REG[6] ),
		.save_dat_to_fifo_flag( cha0_save_dat_to_fifo_flag ),
		.one_second_clk ( one_second_clk ),
		.save_dat_start_time_flag( cha0_save_dat_start_time_flag ),
		.save_dat_enable( !CHA_STA_REG[0][1] )
	);
	
	sc_fifo_12w_4096d spi_read_cha1_dat_fifo (
		.clock (clk_100m),//
		.data (cha1_write_fifo_data),//
		.q (cha1_dat_read_from_fifo),
		.rdreq (stm32_read_cha1_one_new_dat_flag),//
		.wrreq (sdram_buf1_wr),//
		.usedw (spi_read_cha1_dat_fifo_usedw)//
	);
	SAVE_AD9238_CHAx_DAT_TO_FIFO save_ad9238_cha1_dat_to_fifo
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.ad9238_new_ad_dat_avalible( ad9238_new_ad_dat_avalible ),
		.ad9238_chax_dat( ad9238_cha1_dat_out ),
		.ad_dat_thr( DAT_THR_REG[1][11:0]  ),
		.change_rate_thr( CHANGE_RATE_THR[1][11:0] ),
		.write_fifo_en( sdram_buf1_wr ),
		.write_fifo_data( cha1_write_fifo_data ),
		.fifo_idle( CHA_RUN_REG[6] ),
		.save_dat_to_fifo_flag( cha1_save_dat_to_fifo_flag ),
		.one_second_clk ( one_second_clk ),
		.save_dat_start_time_flag( cha1_save_dat_start_time_flag ),
		.save_dat_enable( !CHA_STA_REG[1][1] )
	);
	
	
	wire fpga_interrupt_next;
	
	reg[3:0]fpga_interrupt_time;
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			fpga_interrupt <= 1'b0;
		else
		begin
			if( (cha0_save_dat_to_fifo_flag)||(cha1_save_dat_to_fifo_flag) )
			//if(cha2_save_dat_to_fifo_flag) 
				fpga_interrupt <= 1'b1;
			else if( CHA_RUN_REG[7:4] )
				fpga_interrupt <= 1'b0;
			else if( fpga_interrupt_time>=3)
				fpga_interrupt <= 1'b0;
			else
				fpga_interrupt <= fpga_interrupt_next;
		end
	end
	assign fpga_interrupt_next = fpga_interrupt;
	
	
	always @(posedge one_second_clk or negedge rst_n)
	begin
		if(!rst_n)
			fpga_interrupt_time <= 0;
		else
		begin
			if( (fpga_interrupt)&&(fpga_interrupt_time<6))
				fpga_interrupt_time <= fpga_interrupt_time + 1;
			else
				fpga_interrupt_time <= 0;
		end
	end
	
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			CHA_STA_REG[0] <= 0;
		else
		begin
			if( cha0_save_dat_to_fifo_flag )
				CHA_STA_REG[0] <= 2;
			else if( CHA_RUN_REG[4] )
				CHA_STA_REG[0] <= 0;
			else
				CHA_STA_REG[0] <= CHA_STA_REG_next[0];
		end
	end
	assign CHA_STA_REG_next[0] = CHA_STA_REG[0];
	
	always @(posedge clk_100m or negedge rst_n)
	begin
		if(!rst_n)
			CHA_STA_REG[1] <= 0;
		else
		begin
			if( cha1_save_dat_to_fifo_flag )
				CHA_STA_REG[1] <= 2;
			else if( CHA_RUN_REG[5] )
				CHA_STA_REG[1] <= 0;
			else
				CHA_STA_REG[1] <= CHA_STA_REG_next[1];
		end
	end
	assign CHA_STA_REG_next[1] = CHA_STA_REG[1];
	

	
	

	
	
	
	
	
	
	
	always @( * )
	begin
		case(cha_from_stm32)
		4'b0000:// 
		begin
			chax_dat_read_from_fifo <= cha0_dat_read_from_fifo;
			stm32_read_cha0_one_new_dat_flag <= stm32_read_one_new_dat_flag;	
			stm32_read_cha1_one_new_dat_flag <= 0;
		end
		4'b0001:// 
		begin
			chax_dat_read_from_fifo <= cha1_dat_read_from_fifo;
			stm32_read_cha1_one_new_dat_flag <= stm32_read_one_new_dat_flag;
			stm32_read_cha0_one_new_dat_flag <= 0;			
		end
		4'b0010:// 
		begin
			chax_dat_read_from_fifo <= cha2_dat_read_from_fifo-1792-ZLPY;
			//chax_dat_read_from_fifo <= {cha2_dat_read_from_fifo[7:0],cha2_dat_read_from_fifo[15:8]};//xiewei debug only
			stm32_read_cha2_one_new_dat_flag <= stm32_read_one_new_dat_flag;	
			stm32_read_cha0_one_new_dat_flag <= 0;	
		end
		default:
		begin
			chax_dat_read_from_fifo <= cha0_dat_read_from_fifo;
			stm32_read_cha0_one_new_dat_flag <= stm32_read_one_new_dat_flag;	
			stm32_read_cha1_one_new_dat_flag <= 0;
		end
		endcase
	end

	reg[7:0] cha2_debug_reg/*synthesis noprune*/;
	always @(posedge clk_100m )
		cha2_debug_reg <= ad9238_cha0_dat_out-1920;
		
		
	reg[7:0] cha1_debug_reg/*synthesis noprune*/;
	always @(posedge clk_100m )
		cha1_debug_reg <= ad9238_cha1_dat_out-1920;
		
	
	wire[24:0]cha2_low_pass_2m_filter_out/*synthesis noprune*/;
	reg[11:0]cha2_low_pass_2m_filter_reg/*synthesis noprune*/;
/*	low_pass_2m low_pass_2m_cha2
	(
		.clk( clk_100m ),
		.reset_n( reset_n ),
		.ast_sink_data( ad9238_cha2_dat_out-2048 ),
		.ast_sink_valid( 1'b1 ),
		.ast_source_ready( 1'b1 ),
		.ast_sink_error( 2'b00 ),
		.ast_source_data( cha2_low_pass_2m_filter_out )
	);
	always @(posedge clk_100m )
		cha2_low_pass_2m_filter_reg <= cha2_low_pass_2m_filter_out[24:13];*/
	
	
endmodule