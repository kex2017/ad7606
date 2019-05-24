module CREAT_1HZ_CLK(clk_100m,reset_n,one_second_clk);
	input clk_100m;
	input reset_n;
	output one_second_clk;

	wire clk_100m;
	wire reset_n;
	reg one_second_clk;


	reg[31:0] count1;
	wire[31:0] count1_next;

    always @ ( posedge clk_100m or negedge reset_n )
    begin
		if( !reset_n )
			count1 <= 32'h0;
		else if(count1_next>=100000000)//如果产生内部同步信号
			count1 <= 32'h0;
		else
			count1 <= count1_next + 1'b1;
	end
	assign count1_next = count1;

	 always @ ( posedge clk_100m or negedge reset_n )
    begin
		if( !reset_n )
			one_second_clk <= 1'b0;
		else if(count1_next==0)//如果产生内部同步信号
			one_second_clk <= 1'b1;
		else if(count1_next==(50000000-1))//如果产生内部同步信号
			one_second_clk <= 1'b0;
		else
			one_second_clk <= one_second_clk_next;
	end
	assign one_second_clk_next = one_second_clk;



endmodule

module write_ad9231_cfg_by_fpga_spi
(
	clk,
	reset_n,


	ADG1_CS,
	ADG_SDIO,
	ADG_SCLK,
	ADG2_CS,
	ADH_PDWN,
	one_second_clk

); 

input clk;
input reset_n;
output ADG1_CS;
output ADG2_CS;
output ADG_SCLK;
inout ADG_SDIO;
output ADH_PDWN;
input one_second_clk;

wire clk;
wire reset_n;
wire ADG1_CS;
wire ADG2_CS;
wire ADG_SCLK;
wire ADG_SDIO;
wire ADH_PDWN;
wire one_second_clk;

assign ADH_PDWN = 1'b0;



assign ADG2_CS = ADG1_CS;

wire clk_100m;

reg[1:0] clk_cnt;

always @(posedge clk or negedge reset_n) 
begin 
	if(!reset_n)
		clk_cnt <= 0;
	else
	begin
		clk_cnt <= clk_cnt + 1;
	end
end


AD9231_spi ad_spi1(
	.rstb(reset_n),
	.clk(clk),
	.ss(ADG1_CS),
	.sck(ADG_SCLK),
	.sdio(ADG_SDIO),
	.one_second_clk(one_second_clk)

);
	
	
endmodule



