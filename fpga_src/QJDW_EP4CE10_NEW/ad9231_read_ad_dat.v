
/*************************************************************************
????CREAT_AD9231_2MHZ_SPI_SCLK
?  ????20M???AD9238 SPI?????
?  ??clk_200m?200M??????FPGA??????
		rst_n:????
		ad9231_spi_sclk_enable:??ad9231_spi_sclk_enable
?  ?: ad9231_spi_sclk:
  		ad9231_spi_sclk_flag:sclk?????????????????????????????
*************************************************************************/
module CREAT_AD9231_AD_CLK(clk_200m,rst_n,ad9231_ad_clk_enable,ad9231_ad_clk,ad9231_ad_clk_flag);
	input clk_200m;
	input rst_n;
	input ad9231_ad_clk_enable;
	output ad9231_ad_clk;
	output ad9231_ad_clk_flag;


	wire clk_200m;
	wire rst_n;
	wire ad9231_ad_clk_enable;
	reg ad9231_ad_clk;
	reg ad9231_ad_clk_flag;

	reg[9:0] count1;
	wire[9:0] count1_next;
	wire ad9231_ad_clk_next;


    always @ ( posedge clk_200m or negedge rst_n )
    begin
		if( !rst_n )
			count1 <= 10'h0;
		else if(count1==9)//????50??
			count1 <= 10'h0;
		else if(ad9231_ad_clk_enable)
			count1 <= count1_next + 1'b1;
		else
			count1 <= 10'h0;
	end

	assign count1_next = count1;


	always @ ( posedge clk_200m or negedge rst_n  )
	begin
		if( !rst_n )
			ad9231_ad_clk <= 1'b1;
		else if(ad9231_ad_clk_enable==1'b0)
			ad9231_ad_clk <= 1'b1;
		else if(count1==0)
			ad9231_ad_clk <= 1'b0;
		else if(count1>4)
			ad9231_ad_clk <= 1'b1;
		else
			ad9231_ad_clk <= ad9231_ad_clk_next;
	end
	assign ad9231_ad_clk_next = ad9231_ad_clk;

	always @ ( posedge clk_200m or negedge rst_n  )
	begin
		if( !rst_n )
			ad9231_ad_clk_flag <= 1'b0;
		else if(count1==5)
			ad9231_ad_clk_flag <= 1'b1;
		else
			ad9231_ad_clk_flag <= 1'b0;
	end
endmodule

module read_ad_dat_from_ad9231 
			(
						//clk,
						clk_200m,
						rst_n,
						ad9231_read_enable,
						ad9231_clk_p,
						ad9231_clk_n,
						ad9231_dat,
						ad9231_clk_to_fpga,
						ad9231_cha_dat,
						ad9231_chb_dat,
						ad9231_one_dat_avalible
		);

	
	input clk_200m;
	input rst_n;
	input ad9231_read_enable;
	output ad9231_clk_p;
	output ad9231_clk_n;
	input[11:0] ad9231_dat;
	input ad9231_clk_to_fpga;
	output[11:0] ad9231_cha_dat;
	output[11:0] ad9231_chb_dat;
	output ad9231_one_dat_avalible;
	
	
	wire clk_200m;
	wire rst_n;
	wire ad9231_read_enable;
	wire ad9231_clk_p;
	wire ad9231_clk_n;
	wire[11:0] ad9231_dat;
	wire ad9231_clk_to_fpga;
	reg[11:0] ad9231_cha_dat;
	reg[11:0] ad9231_chb_dat;
	reg ad9231_one_dat_avalible;
	
	wire ad9231_ad_clk;
	wire ad9231_ad_clk_flag;
	
	assign ad9231_clk_p = ad9231_ad_clk;
	assign ad9231_clk_n = !ad9231_ad_clk;
	CREAT_AD9231_AD_CLK creat_ad9231_10m_ad_clk
	(
		.clk_200m( clk_200m ),
		.rst_n( rst_n ),
		.ad9231_ad_clk_enable( ad9231_read_enable ),
		.ad9231_ad_clk( ad9231_ad_clk ),
		.ad9231_ad_clk_flag( ad9231_ad_clk_flag )
	);
	
	
	reg ad9231_clk_to_fpga_d1clk;
	
	always @(posedge clk_200m)
		ad9231_clk_to_fpga_d1clk <= ad9231_clk_to_fpga;
	
	always @(posedge ad9231_clk_to_fpga_d1clk or negedge rst_n)
	begin
		if(!rst_n)
			ad9231_chb_dat <= 12'h0;
		else
			ad9231_chb_dat <= ad9231_dat;
	end
	
	always @(negedge ad9231_clk_to_fpga_d1clk or negedge rst_n)
	begin
		if(!rst_n)
			ad9231_cha_dat <= 12'h0;
		else
			ad9231_cha_dat <= ad9231_dat;
	end
	
	
		
	
	
endmodule

