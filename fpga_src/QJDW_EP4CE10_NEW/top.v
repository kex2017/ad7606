


module config_ad9231_by_spi_top
			(
						//clk,
						clk_200m,
						rst_n,



						ad9231_1_powerdown,
						ad9231_spi_write_addr,
						ad9231_spi_write_data,
						ad9231_spi_write_read,
						ad9231_spi_write_reg_cnt,
						ad9231_spi_write_flag,
						ad9231_spi_write_over
		);

	
	input clk_200m;
	input rst_n;

	

	output ad9231_1_powerdown;
	output [12:0]ad9231_spi_write_addr;
	output [7:0]ad9231_spi_write_data;
	output ad9231_spi_write_read;//0,WRITE  1:READ
	output [1:0]ad9231_spi_write_reg_cnt;
	output ad9231_spi_write_flag;
	input ad9231_spi_write_over;
	
	
	wire clk_200m;
	wire rst_n;
	wire [12:0]ad9231_spi_write_addr;
	wire [7:0]ad9231_spi_write_data;
	wire ad9231_spi_write_read;//0,WRITE  1:READ
	wire [1:0]ad9231_spi_write_reg_cnt;
	wire ad9231_spi_write_flag;
	wire ad9231_spi_write_over;
	
	assign ad9231_spi_write_addr = 13'h14;
	assign ad9231_spi_write_read = 1'b0;
	assign ad9231_spi_write_data = 8'h20;

	
	
	
	parameter IDLE    					= 6'b000001;
	parameter WRITE_REG14    		= 6'b000010;
	parameter READ_REG01    			= 6'b000100;
	parameter READ_REG02  				= 6'b001000;
	
	reg[5:0] SPI_WR_CS;
	reg[5:0] SPI_WR_NS;
	
	
	reg[7:0] write_cmd_cnt;
	wire[7:0] write_cmd_cnt_next;
	
	reg[7:0] write_dat_cnt;
	wire[7:0] write_dat_cnt_next;
		
	reg[7:0] read_dat_cnt;
	wire[7:0] read_dat_cnt_next;
	

	wire ad9231_spi_rw_over;
	
	reg write_reg14_flag;
	reg read_reg1_flag;
	reg read_reg2_flag;
	
	assign ad9231_1_powerdown = 1'b0;
	assign ad9231_2_powerdown = 1'b0;

	
	always @( * )
	begin
		case(SPI_WR_CS)
		IDLE://????
				SPI_WR_NS <= WRITE_REG14;
		WRITE_REG14://?????reg14?0x20
				SPI_WR_NS <= READ_REG01;
		READ_REG01://?????reg1
			//if(ad9231_spi_write_over)
				SPI_WR_NS <= READ_REG02;
			//else
			//	SPI_WR_NS <= SPI_WR_CS;
		READ_REG02://?????reg2
				SPI_WR_NS <= IDLE;
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
		if ( (SPI_WR_CS==WRITE_REG14)&&(SPI_WR_NS==READ_REG01) )
			read_reg1_flag<= 1'b1;
		else
			read_reg1_flag<= 1'b0;
	end
	
	assign ad9231_spi_write_flag = read_reg1_flag;
	
	always @( posedge clk_200m )
	begin
		if ( (SPI_WR_CS==READ_REG01)&&(SPI_WR_NS==READ_REG02) )
			read_reg2_flag<= 1'b1;
		else
			read_reg2_flag<= 1'b0;
	end
	


	
	
	
endmodule