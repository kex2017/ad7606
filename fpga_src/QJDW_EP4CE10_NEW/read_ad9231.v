


module read_ad9231_top
			(
						//clk,
						clk_200m,
						rst_n,


						spi_sclk_to_ad9231,
						spi_csb_to_ad9231,
						spi_sdio_to_ad9231,
						ad9231_1_powerdown,
						ad9231_clk_p,
						ad9231_clk_n,
						ad9231_dat,
						ad9231_clk_to_fpga
						
		);
		

	
	input clk_200m;
	input rst_n;

	
	output spi_sclk_to_ad9231;
	output spi_csb_to_ad9231;
	output spi_sdio_to_ad9231;
	output ad9231_1_powerdown;
	output ad9231_clk_p;
	output ad9231_clk_n;
	input[11:0] ad9231_dat;
	input ad9231_clk_to_fpga;
	
	
	wire clk_200m;
	wire rst_n;
	reg spi_sclk_to_ad9231;
	reg spi_csb_to_ad9231;
	reg spi_sdio_to_ad9231;
	wire ad9231_clk_p;
	wire ad9231_clk_n;
	wire[11:0] ad9231_dat;
	wire ad9231_clk_to_fpga;
	
	
	wire [12:0]ad9231_spi_write_addr;
	wire [7:0]ad9231_spi_write_data;
	wire ad9231_spi_write_read;//0,WRITE  1:READ
	wire [1:0]ad9231_spi_write_reg_cnt;
	wire ad9231_spi_write_flag;
	reg ad9231_spi_write_over;
	
	
	
	
	
	parameter IDLE    					= 6'b000001;
	parameter WRITE_REG14    			= 6'b000010;
	parameter WRITE_REGff    			= 6'b000100;
	parameter WRITE_REG2e  				= 6'b001000;
	parameter READ_AD_DAT = 6'b010000;
	
	reg[5:0] SPI_WR_CS;
	reg[5:0] SPI_WR_NS;
	
	
	reg[7:0] write_cmd_cnt;
	wire[7:0] write_cmd_cnt_next;
	
	reg[7:0] write_dat_cnt;
	wire[7:0] write_dat_cnt_next;
		
	reg[7:0] read_dat_cnt;
	wire[7:0] read_dat_cnt_next;
	

	wire ad9231_spi_rw_over;
	wire ad9231_spi_w_reg_ff_over;
	reg write_reg14_flag;
	
	reg write_regff_flag;
	reg write_reg2e_flag;
	reg read_reg2_flag;
	wire ad9231_spi_w2e_over;
	
	assign ad9231_1_powerdown = 1'b0;

	
	always @( * )
	begin
		case(SPI_WR_CS)
		IDLE://????
				SPI_WR_NS <= WRITE_REG14;
		WRITE_REG14://?????reg14?0x20
			if(ad9231_spi_rw_over)
				SPI_WR_NS <= WRITE_REG2e;
			else
				SPI_WR_NS <= SPI_WR_CS;
		WRITE_REG2e://?????reg14?0x20
			if(ad9231_spi_w2e_over)
				SPI_WR_NS <= WRITE_REGff;
			else
				SPI_WR_NS <= SPI_WR_CS;
		WRITE_REGff://
			if(ad9231_spi_w_reg_ff_over)
				SPI_WR_NS <= READ_AD_DAT;
			else
				SPI_WR_NS <= SPI_WR_CS;
		READ_AD_DAT://?????AD
				SPI_WR_NS <= SPI_WR_CS;
		default:
			SPI_WR_NS <= IDLE;
		endcase
	end
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			SPI_WR_CS <= IDLE;
		else
			SPI_WR_CS <= SPI_WR_NS;
	end
	
	
	always @( posedge clk_200m )
	begin
		if ( (SPI_WR_CS==IDLE)&&(SPI_WR_NS==WRITE_REG14) )
			write_reg14_flag<= 1'b1;
		else
			write_reg14_flag<= 1'b0;
	end
	
	always @( posedge clk_200m )
	begin
		if ( (SPI_WR_CS==WRITE_REG2e)&&(SPI_WR_NS==WRITE_REGff) )
			write_regff_flag<= 1'b1;
		else
			write_regff_flag<= 1'b0;
	end
	
	
	always @( posedge clk_200m )
	begin
		if ( (SPI_WR_CS==WRITE_REG14)&&(SPI_WR_NS==WRITE_REG2e) )
			write_reg2e_flag<= 1'b1;
		else
			write_reg2e_flag<= 1'b0;
	end
	
	

	

	

	
	wire spi_sclk_to_ad9231_reg14;
	wire spi_csb_to_ad9231_reg14;
	wire spi_sdio_to_ad9231_reg14;
	
	wire spi_sclk_to_ad9231_reg2e;
	wire spi_csb_to_ad9231_reg2e;
	wire spi_sdio_to_ad9231_reg2e;
	
	wire spi_sclk_to_ad9231_regff;
	wire spi_csb_to_ad9231_regff;
	wire spi_sdio_to_ad9231_regff;
	
	
	write_cmd_to_ad9231_by_spi write_reg14
	(
		.clk_200m( clk_200m ),
		.rst_n( rst_n ),
		.ad9231_spi_write_addr( 13'h14 ),
		.ad9231_spi_write_data( 8'h20 ),
		.ad9231_spi_write_read( 1'b0 ),
		.ad9231_spi_write_reg_cnt( 2'b00 ),
		.ad9231_spi_write_flag( write_reg14_flag ),
		.ad9231_spi_write_over( ad9231_spi_rw_over ),

		.spi_sclk_to_ad9231( spi_sclk_to_ad9231_reg14 ),
		.spi_csb_to_ad9231( spi_csb_to_ad9231_reg14 ),
		.spi_sdio_to_ad9231( spi_sdio_to_ad9231_reg14 )
	);
	
	write_cmd_to_ad9231_by_spi write_reg2e
	(
		.clk_200m( clk_200m ),
		.rst_n( rst_n ),
		.ad9231_spi_write_addr( 13'h2e ),
		.ad9231_spi_write_data( 8'h00 ),
		.ad9231_spi_write_read( 1'b0 ),
		.ad9231_spi_write_reg_cnt( 2'b00 ),
		.ad9231_spi_write_flag( write_reg2e_flag ),
		.ad9231_spi_write_over( ad9231_spi_w2e_over ),

		.spi_sclk_to_ad9231( spi_sclk_to_ad9231_reg2e ),
		.spi_csb_to_ad9231( spi_csb_to_ad9231_reg2e ),
		.spi_sdio_to_ad9231( spi_sdio_to_ad9231_reg2e )
	);
	
	write_cmd_to_ad9231_by_spi write_regff
	(
		.clk_200m( clk_200m ),
		.rst_n( rst_n ),
		.ad9231_spi_write_addr( 13'hff ),
		.ad9231_spi_write_data( 8'h01 ),
		.ad9231_spi_write_read( 1'b0 ),
		.ad9231_spi_write_reg_cnt( 2'b00 ),
		.ad9231_spi_write_flag( write_regff_flag ),
		.ad9231_spi_write_over( ad9231_spi_w_reg_ff_over ),

		.spi_sclk_to_ad9231( spi_sclk_to_ad9231_regff ),
		.spi_csb_to_ad9231( spi_csb_to_ad9231_regff ),
		.spi_sdio_to_ad9231( spi_sdio_to_ad9231_regff )
	);
	
	
	
	
	always @( * )
	begin
		case(SPI_WR_CS)
		WRITE_REG14:// 
		begin
			spi_sclk_to_ad9231 <=	spi_sclk_to_ad9231_reg14;
			spi_csb_to_ad9231  <=	spi_csb_to_ad9231_reg14;
			spi_sdio_to_ad9231 <=	spi_sdio_to_ad9231_reg14;
		end
		WRITE_REG2e:// 
		begin
			spi_sclk_to_ad9231 <=	spi_sclk_to_ad9231_reg2e;
			spi_csb_to_ad9231  <=	spi_csb_to_ad9231_reg2e;
			spi_sdio_to_ad9231 <=	spi_sdio_to_ad9231_reg2e;
		end
		WRITE_REGff:// 
		begin
			spi_sclk_to_ad9231 <=	spi_sclk_to_ad9231_regff;
			spi_csb_to_ad9231  <= 	spi_csb_to_ad9231_regff;
			spi_sdio_to_ad9231 <=	spi_sdio_to_ad9231_regff;
			
		end
		default:
		begin
			spi_sclk_to_ad9231 <=	spi_sclk_to_ad9231_reg14;
			spi_csb_to_ad9231  <=	spi_csb_to_ad9231_reg14;
			spi_sdio_to_ad9231 <=	spi_sdio_to_ad9231_reg14;
		end
		endcase
	end
	
	
	
	
	
	reg ad9231_read_enable;
	
	always @( posedge clk_200m )
	begin
		if ( SPI_WR_CS==READ_AD_DAT )
			ad9231_read_enable<= 1'b1;
		else
			ad9231_read_enable<= 1'b0;
	end
	
	
	read_ad_dat_from_ad9231 read_ad9231_dat
	(
			.clk_200m( clk_200m ),
			.rst_n( rst_n ),
			.ad9231_read_enable( ad9231_read_enable ),
			.ad9231_clk_p( ad9231_clk_p ),
			.ad9231_clk_n( ad9231_clk_n ),
			.ad9231_dat( ad9231_dat ),
			.ad9231_clk_to_fpga( ad9231_clk_to_fpga )/*,
			.ad9231_cha_dat(  ),
			.ad9231_chb_dat(  ),
			.ad9231_one_dat_avalible(  )*/
	);
	
	
	
endmodule