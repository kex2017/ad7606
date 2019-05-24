/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  SPI MODE 3
		CHANGE DATA @ NEGEDGE
		read data @posedge

 RSTB-active low asyn reset, CLK-clock, T_RB=0-rx  1-TX, mlb=0-LSB 1st 1-msb 1st
 START=1- starts data transmission cdiv 0=clk/4 1=/8   2=/16  3=/32
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
module master_spi(rstb,clk,mlb,start,tdat,cdiv,din, ss,sck,dout,done,rdata);
    input rstb,clk,mlb,start;
    input [23:0] tdat;  //transmit data
    input [1:0] cdiv;  //clock divider
	input din;
	output reg ss; 
	output reg sck; 
	output dout; 
   output reg done;
	output reg [7:0] rdata; //received data
   reg    rdout;
parameter idle=2'b00;		
parameter send=2'b10; 
parameter finish=2'b11; 
reg [1:0] cur,nxt;

	reg [23:0] treg;
	reg [7:0] rreg;
	reg [5:0] nbit;
	reg [4:0] mid,cnt;
	reg shift,clr;

//FSM i/o
always @(start or cur or nbit or cdiv or rreg) begin
		 nxt=cur;
		 clr=0;  
		 shift=0;//ss=0;
		 case(cur)
			idle:begin
				if(start==1)
		               begin 
							 case (cdiv)
								2'b00: mid=2;
								2'b01: mid=4;
								2'b10: mid=8;
								2'b11: mid=16;
 							 endcase
						shift=1;
						done=1'b0;
						nxt=send;	 
						end
		        end //idle
			send:begin
				ss=0;
				if(nbit!=24)
					begin shift=1; end
				else begin
						rdata=rreg;done=1'b1;
						nxt=finish;
					end
				end//send
			finish:begin
					shift=0;
					ss=1;
					clr=1;
					nxt=idle;
				 end
			default: nxt=finish;
      endcase
    end//always

//state transistion
always@(negedge clk or negedge rstb) begin
 if(rstb==0) 
   cur<=finish;
 else 
   cur<=nxt;
 end

//setup falling edge (shift dout) sample rising edge (read din)
always@(negedge clk or posedge clr) begin
  if(clr==1) 
		begin cnt=0; sck=1; end
  else begin
	if(shift==1) begin
		cnt=cnt+1; 
	  if(cnt==mid) begin
	  	sck=~sck;
		cnt=0;
		end //mid
	end //shift
 end //rst
end //always

assign dout = rdout;
assign rStart = tdat[23] && nbit >= 16;
//sample @ rising edge (read din)
always@(posedge sck or posedge clr ) begin // or negedge rstb
 if(clr==1)  
	begin
			nbit=0;  
			rreg=8'hFF;  
	end
   else begin 
		if(rStart)
		begin
		  if(mlb==0) //LSB first, din@msb -> right shift
			begin  rreg={din,rreg[7:1]};  end 
		  else  //MSB first, din@lsb -> left shift
			begin  rreg={rreg[6:0],din};  end
		end	
		nbit=nbit+1;
   end //rst
end //always

always@(negedge sck or posedge clr) begin
 if(clr==1) begin
	  treg=24'hFFFF;  rdout=1;  
  end  
 else begin
		if(nbit==0) begin //load data into TREG
			treg=tdat; rdout=mlb?treg[23]:treg[0];
		end //nbit_if
		else begin
			if(mlb==0) //LSB first, shift right
				begin treg={1'b1,treg[23:1]}; rdout=treg[0]; end
			else//MSB first shift LEFT
				begin treg={treg[22:0],1'b1}; rdout=treg[23]; end
		end
 end //rst
end //always

endmodule

module AD9231_spi(
	rstb,
	clk,
	ss,
	sck,
	sdio,
	rdata);
parameter w_reg_05=24'h000503;
parameter w_reg_14=24'h001420;
parameter w_reg_ff=24'h00FF01;

parameter SPI_IDLE=3'd0;
parameter SPI_W_05=3'd1;
parameter SPI_W_14=3'd2;
parameter SPI_W_FF=3'd3;

input rstb;
input clk;
output	ss;
output	sck;
output	sdio;
output [7:0]	rdata;

reg  [31:0] cnt;
reg  [2:0] adc_spi_sta;
reg  adc_spi_start;
wire  adc_spi_done;
reg [23:0] adc_spi_sdata;
reg [1:0] done_shif;

master_spi spi_m(
.rstb(rstb),
.clk(clk),
.mlb(1'b1),
.start(adc_spi_start),
.tdat(adc_spi_sdata),
.cdiv(2'b10),
.ss(ss),
.sck(sck),
.dout(sdio),
.done(adc_spi_done)
);
always @(posedge clk or negedge rstb) 
begin 
	if(!rstb)
		done_shif <= 0;
	else
	begin
		done_shif[1] <= done_shif[0];
		done_shif[0] <= adc_spi_done; 
	end
end


always @(negedge rstb or posedge clk) 
begin
   if(!rstb)
	begin
	   adc_spi_sta <= SPI_IDLE;
	   adc_spi_start <= 1'b0;
		adc_spi_sdata <= 24'hffffff;
		cnt <= 0;
	end
	else
	begin		   
//		if(done_shif == 2'b01)
//			adc_spi_start <= 0;
		if(cnt == 32'd5000)
		begin
			adc_spi_start <= 1'b1;	
			adc_spi_sdata <= w_reg_05;
			cnt <= cnt+1;	
		end
		else
		if(cnt == 32'd10000)
		begin
			adc_spi_start <= 1'b1;	
			adc_spi_sdata <= w_reg_14;	
			cnt <= cnt+1;
		end
		else
		if(cnt == 32'd15000)
		begin
			adc_spi_start <= 1'b1;	
			adc_spi_sdata <= w_reg_ff;	
			cnt <= cnt+1;
		end
		else
		if(cnt == 32'd20000)
			cnt <= cnt;
		else
		begin
			adc_spi_start <= 1'b0;
			cnt <= cnt+1;
		end
		end
end
endmodule



module cfg_ad9231_by_fpga_spi(
	
	clk_in,
	n_rst,
	
	ADH_PDWN,
	ADH2_CS,
	ADH1_CS,
	ADH_SCLK,
	ADH_SDIO,


); 


	output ADH1_CS;
	output ADH2_CS;
	output ADH_SCLK;
	inout ADH_SDIO;
	output ADH_PDWN;









	input clk_in;
	input n_rst;







	assign ADH2_CS = ADH1_CS;

	assign ADH_PDWN = 1'b0;



	AD9231_spi ad_spi(
		.rstb(n_rst),
		.clk(clk_in),
		.ss(ADH1_CS),
		.sck(ADH_SCLK),
		.sdio(ADH_SDIO)
	//	.rdata()
	);
	
	

	
endmodule