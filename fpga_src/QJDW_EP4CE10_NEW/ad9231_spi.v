



module CREAT_1S_CLK_FROM_50HZ(clk_50hz,clk_1s);
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
/*************************************************************************
????CREAT_AD9231_2MHZ_SPI_SCLK
?  ????20M???AD9238 SPI?????
?  ??clk_200m?200M??????FPGA??????
		rst_n:????
		ad9231_spi_sclk_enable:??ad9231_spi_sclk_enable
?  ?: ad9231_spi_sclk:
  		ad9231_spi_sclk_flag:sclk?????????????????????????????
*************************************************************************/
module CREAT_AD9231_2MHZ_SPI_SCLK(clk_200m,rst_n,ad9231_spi_sclk_enable,ad9231_spi_sclk,ad9231_spi_sclk_flag);
	input clk_200m;
	input rst_n;
	input ad9231_spi_sclk_enable;
	output ad9231_spi_sclk;
	output ad9231_spi_sclk_flag;


	wire clk_200m;
	wire rst_n;
	wire ad9231_spi_sclk_enable;
	reg ad9231_spi_sclk;
	reg ad9231_spi_sclk_flag;

	reg[9:0] count1;
	wire[9:0] count1_next;
	wire ad9231_spi_sclk_next;


    always @ ( posedge clk_200m or negedge rst_n )
    begin
		if( !rst_n )
			count1 <= 10'h0;
		else if(count1==199)//????50??
			count1 <= 10'h0;
		else if(ad9231_spi_sclk_enable)
			count1 <= count1_next + 1'b1;
		else
			count1 <= 10'h0;
	end

	assign count1_next = count1;


	always @ ( posedge clk_200m or negedge rst_n  )
	begin
		if( !rst_n )
			ad9231_spi_sclk <= 1'b1;
		else if(ad9231_spi_sclk_enable==1'b0)
			ad9231_spi_sclk <= 1'b1;
		else if(count1==0)
			ad9231_spi_sclk <= 1'b0;
		else if(count1>99)
			ad9231_spi_sclk <= 1'b1;
		else
			ad9231_spi_sclk <= ad9231_spi_sclk_next;
	end
	assign ad9231_spi_sclk_next = ad9231_spi_sclk;

	always @ ( posedge clk_200m or negedge rst_n  )
	begin
		if( !rst_n )
			ad9231_spi_sclk_flag <= 1'b0;
		else if(count1==145)
			ad9231_spi_sclk_flag <= 1'b1;
		else
			ad9231_spi_sclk_flag <= 1'b0;
	end
endmodule

module write_cmd_to_ad9231_by_spi
			(
						//clk,
						clk_200m,
						rst_n,
						ad9231_spi_write_addr,
						ad9231_spi_write_data,
						ad9231_spi_write_read,
						ad9231_spi_write_reg_cnt,
						ad9231_spi_write_flag,
						ad9231_spi_write_over,

						spi_sclk_to_ad9231,
						spi_csb_to_ad9231,
						spi_sdio_to_ad9231
		);

	
	input clk_200m;
	input rst_n;
	input [12:0]ad9231_spi_write_addr;
	input [7:0]ad9231_spi_write_data;
	input ad9231_spi_write_read;//0,WRITE  1:READ
	input [1:0]ad9231_spi_write_reg_cnt;
	input ad9231_spi_write_flag;
	output ad9231_spi_write_over;
	
	output spi_sclk_to_ad9231;
	output spi_csb_to_ad9231;
	output spi_sdio_to_ad9231;
	
	
	wire clk_200m;
	wire rst_n;
	wire [12:0]ad9231_spi_write_addr;
	wire [7:0]ad9231_spi_write_data;
	wire ad9231_spi_write_read;//0,WRITE  1:READ
	wire [1:0]ad9231_spi_write_reg_cnt;
	wire ad9231_spi_write_flag;
	reg ad9231_spi_write_over;
	
	wire spi_sclk_to_ad9231;
	reg spi_csb_to_ad9231;
	wire spi_sdio_to_ad9231;
	
	
	
	parameter IDLE    					= 6'b000001;
	parameter PULL_DOWN_CSN    	= 6'b000010;
	parameter WRITE_CMD    			= 6'b000100;
	parameter WRITE_DAT  				= 6'b001000;
	parameter READ_DAT    			= 6'b010000;
	parameter RW_SUCCESS    		= 6'b100000;
	
	reg[5:0] SPI_WRITE_CS;
	reg[5:0] SPI_WRITE_NS;
	
	
	reg[7:0] write_cmd_cnt;
	wire[7:0] write_cmd_cnt_next;
	
	reg[7:0] write_dat_cnt;
	wire[7:0] write_dat_cnt_next;
		
	reg[7:0] read_dat_cnt;
	wire[7:0] read_dat_cnt_next;
	

	wire ad9231_spi_sclk_flag;//spi???????
	reg ad9231_spi_clk_enable;

	
	always @( * )
	begin
		case(SPI_WRITE_CS)
		IDLE://????
		begin
			if( ad9231_spi_write_flag )
				SPI_WRITE_NS <= PULL_DOWN_CSN;
			else
				SPI_WRITE_NS <= SPI_WRITE_CS;
		end
		PULL_DOWN_CSN://??????????
				SPI_WRITE_NS <= WRITE_CMD;
		WRITE_CMD://????? ???????????????
			if( (write_cmd_cnt>=16)&&(ad9231_spi_write_read==1'b0) )
				SPI_WRITE_NS <= WRITE_DAT;		
			else if( (write_cmd_cnt>=16)&&(ad9231_spi_write_read==1'b1) )
				SPI_WRITE_NS <= READ_DAT;
			else
				SPI_WRITE_NS <= SPI_WRITE_CS;
		WRITE_DAT://??????????
			if(write_dat_cnt>=8)
				SPI_WRITE_NS <= RW_SUCCESS;
			else
				SPI_WRITE_NS <= SPI_WRITE_CS;
		READ_DAT://??????????
			if(read_dat_cnt>=8)
				SPI_WRITE_NS <= RW_SUCCESS;
			else
				SPI_WRITE_NS <= SPI_WRITE_CS;
		RW_SUCCESS://????????????????????
			SPI_WRITE_NS <= IDLE;
		default:
			SPI_WRITE_NS <= IDLE;
		endcase
	end
	
	
	always @( * )
	begin
		if ( (SPI_WRITE_CS==WRITE_CMD)||(SPI_WRITE_CS==WRITE_DAT)||(SPI_WRITE_CS==READ_DAT) )
			ad9231_spi_clk_enable<= 1'b1;
		else
			ad9231_spi_clk_enable<= 1'b0;
	end
	
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			SPI_WRITE_CS <= IDLE;
		else
			SPI_WRITE_CS <= SPI_WRITE_NS;
	end
	
	//??????
	wire spi_csb_to_ad9231_next;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			spi_csb_to_ad9231 <= 1'b1;
		else
		begin
			if( SPI_WRITE_CS == PULL_DOWN_CSN )
				spi_csb_to_ad9231 <= 1'b0;
			else if( SPI_WRITE_CS == RW_SUCCESS )
				spi_csb_to_ad9231 <= 1'b1;
			else
				spi_csb_to_ad9231 <= spi_csb_to_ad9231_next;
		end
	end
	assign spi_csb_to_ad9231_next = spi_csb_to_ad9231;
	
	
	//??cmd?????
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			write_cmd_cnt <= 0;
		else
		begin
			if( SPI_WRITE_CS == WRITE_CMD )
			begin
				if(ad9231_spi_sclk_flag)
					write_cmd_cnt <= write_cmd_cnt_next + 1'b1;
				else	
					write_cmd_cnt <= write_cmd_cnt_next;
			end
			else
				write_cmd_cnt <= 0;
		end
	end
	assign write_cmd_cnt_next = write_cmd_cnt;
	
	
	reg[15:0] cmd;
	wire[15:0]cmd_next;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			cmd <= 0;
		else
		begin
			if(SPI_WRITE_CS == PULL_DOWN_CSN)
				cmd <= {ad9231_spi_write_read,ad9231_spi_write_reg_cnt,ad9231_spi_write_addr};
			else if( SPI_WRITE_CS == WRITE_CMD )
			begin
				if(ad9231_spi_sclk_flag)
					cmd <= {cmd_next[14:0],1'b0};
				else	
					cmd <= cmd_next;
			end
			else
				cmd <= {ad9231_spi_write_read,ad9231_spi_write_reg_cnt,ad9231_spi_write_addr};
		end
	end
	assign cmd_next = cmd;
	
	
	
	
	
	//??dat?????
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			write_dat_cnt <= 0;
		else
		begin
			if( SPI_WRITE_CS == WRITE_DAT )
			begin
				if(ad9231_spi_sclk_flag)
					write_dat_cnt <= write_dat_cnt_next + 1'b1;
				else	
					write_dat_cnt <= write_dat_cnt_next;
			end
			else
				write_dat_cnt <= 0;
		end
	end
	assign write_dat_cnt_next = write_dat_cnt;
	
	
	reg[7:0] write_dat_reg;
	wire[7:0]write_dat_reg_next;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			write_dat_reg <= 0;
		else
		begin
			if( (SPI_WRITE_CS == PULL_DOWN_CSN)&&(ad9231_spi_write_read==1'b0) )
				write_dat_reg <= ad9231_spi_write_data;
			else if( SPI_WRITE_CS == WRITE_DAT )
			begin
				if(ad9231_spi_sclk_flag)
					write_dat_reg <= {write_dat_reg_next[6:0],1'b0};
				else	
					write_dat_reg <= write_dat_reg_next;
			end
			else
				write_dat_reg <= ad9231_spi_write_data;
		end
	end
	assign write_dat_reg_next = write_dat_reg;
	
	//??dat?????
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			read_dat_cnt <= 0;
		else
		begin
			if( SPI_WRITE_CS == READ_DAT )
			begin
				if(ad9231_spi_sclk_flag)
					read_dat_cnt <= read_dat_cnt_next + 1'b1;
				else	
					read_dat_cnt <= read_dat_cnt_next;
			end
			else
				read_dat_cnt <= 0;
		end
	end
	assign read_dat_cnt_next = read_dat_cnt;
	
	
	reg[7:0]read_dat_reg;
	wire[7:0]read_dat_reg_next;
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			read_dat_reg <= 0;
		else
		begin
			if( SPI_WRITE_CS == WRITE_DAT )
			begin
				if(ad9231_spi_sclk_flag)
					read_dat_reg <= {read_dat_reg_next[6:0],1'b0};
				else	
					read_dat_reg <= read_dat_reg_next;
			end
			else
				read_dat_reg <= read_dat_reg_next;
		end
	end
	assign read_dat_reg_next = read_dat_reg;
	
	
	
	
	CREAT_AD9231_2MHZ_SPI_SCLK creat_ad9231_spi_sclk
	(
		.clk_200m( clk_200m ),
		.rst_n( rst_n ),
		.ad9231_spi_sclk_enable( ad9231_spi_clk_enable ),
		.ad9231_spi_sclk( spi_sclk_to_ad9231 ),
		.ad9231_spi_sclk_flag( ad9231_spi_sclk_flag )
	);
	
	reg spi_sdio_send_to_ad9231;
	always @( * )
	begin
		case(SPI_WRITE_CS)
		WRITE_CMD: 
			spi_sdio_send_to_ad9231 <= cmd[15];
		WRITE_DAT: 
			spi_sdio_send_to_ad9231 <= write_dat_reg[7];
		default:
			spi_sdio_send_to_ad9231 <= 1'b1;
		endcase
	end
	
	//assign spi_sdio_to_ad9231 = (SPI_WRITE_CS!=READ_DAT)?spi_sdio_send_to_ad9231:1'bz;
	assign spi_sdio_to_ad9231 = spi_sdio_send_to_ad9231;
	
	
	//???????????
	always @(posedge clk_200m or negedge rst_n)
	begin
		if(!rst_n)
			ad9231_spi_write_over <= 1'b0;
		else
		begin
			if( SPI_WRITE_CS == RW_SUCCESS )
				ad9231_spi_write_over <= 1'b1;
			else
				ad9231_spi_write_over <= 1'b0;
		end
	end
	
	
	
endmodule

