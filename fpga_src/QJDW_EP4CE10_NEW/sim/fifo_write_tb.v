// Copyright (C) 1991-2013 Altera Corporation
// Your use of Altera Corporation's design tools, logic functions 
// and other software and tools, and its AMPP partner logic 
// functions, and any output files from any of the foregoing 
// (including device programming or simulation files), and any 
// associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License 
// Subscription Agreement, Altera MegaCore Function License 
// Agreement, or other applicable license agreement, including, 
// without limitation, that your use is for the sole purpose of 
// programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the 
// applicable agreement for further details.

// *****************************************************************************
// This file contains a Verilog test bench template that is freely editable to  
// suit user's needs .Comments are provided in each section to help the user    
// fill out necessary details.                                                  
// *****************************************************************************
// Generated on "01/04/2018 18:22:47"
                                                                                
// Verilog Test Bench template for design : fifo_1m_data
// 
// Simulation tool : ModelSim-Altera (Verilog)
// 

`timescale 1 ns/ 1 ps
module fifo_write_tb(  );


reg clk;
reg adc_clk;
reg rst_n;
reg [95:0]adc7656_data;
reg [47:0]pd_dat;
reg [107:0]dw_dat;

reg ad7656_new_ad_dat_avalible;
reg pd_dat_avalible;
reg dw_new_ad_dat_avalible;

reg ad7656_dat_fifo_clear;
reg pd_dat_fifo_clear;
reg dw_dat_fifo_clear;

reg burst_finish;
reg wr_brust_data_req;


wire wr_burst_req;

reg wr_burst_data_req;

			
reg[7:0] set_rq_clk_cnt;

	initial                                                
	begin                                               
                                           
		clk = 0;
		forever #5 clk =~clk;   
	end	    
	
	initial                                                
	begin                                               
                                           
		adc_clk = 0;
		forever #15 adc_clk =~adc_clk;   
	end	                                    
  
    initial                                                
	begin                                               
                                           
		rst_n = 0;
		#100 rst_n = 1;  
	end	                                          
                                                

 reg rd_burst_req;
 wire[23:0] rd_burst_addr;
assign rd_burst_addr = 1234;


	

	
	reg[7:0] clk_cnt;
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			clk_cnt <= 0;
		else
			clk_cnt <= clk_cnt + 1;
	end
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			adc7656_data <= 0;
		else if( (clk_cnt%100)==2 )
			adc7656_data <= adc7656_data + 1;
		else
			adc7656_data <= adc7656_data;
	end
	
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			ad7656_new_ad_dat_avalible <= 0;
		else if( (clk_cnt%100)==2 )
			ad7656_new_ad_dat_avalible <= 1;
		else
			ad7656_new_ad_dat_avalible <= 0;
	end
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			ad7656_dat_fifo_clear <= 0;
		else if( clk_cnt==1 )
			ad7656_dat_fifo_clear <= 0;//1;
		else
			ad7656_dat_fifo_clear <= 0;
	end
	


	

	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			pd_dat <= 0;
		else if((clk_cnt%50)==2 )
			pd_dat <= pd_dat + 2;
		else
			pd_dat <= pd_dat;
	end
	
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			pd_dat_avalible <= 0;
		else if( (clk_cnt%50)==2 )
			pd_dat_avalible <= 1;
		else
			pd_dat_avalible <= 0;
	end
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			pd_dat_fifo_clear <= 0;
		else if( clk_cnt==1 )
			pd_dat_fifo_clear <= 0;//1;
		else
			pd_dat_fifo_clear <= 0;
	end
	
	


	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			dw_dat <= 0;
		else if((clk_cnt%3)==2 )
			dw_dat <= dw_dat + 3;
		else
			dw_dat <= dw_dat;
	end
	
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			dw_new_ad_dat_avalible <= 0;
		else if( (clk_cnt%3)==2 )
			dw_new_ad_dat_avalible <= 1;
		else
			dw_new_ad_dat_avalible <= 0;
	end
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			dw_dat_fifo_clear <= 0;
		else if( clk_cnt==1 )
			dw_dat_fifo_clear <= 0;//1;
		else
			dw_dat_fifo_clear <= 0;
	end
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			rd_burst_req <= 0;
		else if( clk_cnt==7 )
			rd_burst_req <=  1;
		else
			rd_burst_req <= 0;
	end
	
	
	
	
	
parameter IDLE = 5'h0;
parameter SET_RQ_SIG = 5'h1;
parameter SEND_BURST_FINISH	 = 5'h2;
	
	reg[4:0] WR_BURST_CS;
	reg[4:0] WR_BURST_NS;
	
	always @( * )
	begin
		case(WR_BURST_CS)
		IDLE:
		begin
			if( wr_burst_req )
				WR_BURST_NS <= SET_RQ_SIG;
			else
				WR_BURST_NS <= WR_BURST_CS;
		end
		SET_RQ_SIG:  
		begin
			if(set_rq_clk_cnt>=128)
				WR_BURST_NS <= SEND_BURST_FINISH;
			else
				WR_BURST_NS <= WR_BURST_CS;
		end
		SEND_BURST_FINISH: 
			WR_BURST_NS <= IDLE;
		default:
			WR_BURST_NS <= IDLE;
		endcase
	end
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			WR_BURST_CS <= IDLE;
		else
			WR_BURST_CS <= WR_BURST_NS;
	end
	
	
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			wr_burst_data_req <= 1'b0;
		else if( WR_BURST_CS==SET_RQ_SIG )
			wr_burst_data_req <= 1'b1;
		else
			wr_burst_data_req <= 1'b0;
	end
	
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			set_rq_clk_cnt <=8'h0;
		else if( WR_BURST_CS==IDLE )
			set_rq_clk_cnt <=8'h0;
		else if( WR_BURST_CS==SET_RQ_SIG )
			set_rq_clk_cnt <= set_rq_clk_cnt+ 1'b1;
		else
			set_rq_clk_cnt <= set_rq_clk_cnt;
	end
	
	
	always @(posedge clk or negedge rst_n)
	begin
		if(!rst_n)
			burst_finish <=1'h0;
		else if( WR_BURST_CS==SEND_BURST_FINISH )
			burst_finish <=1'h1;
		else
			burst_finish <=1'h0;
	end
	
	wire ad9231_sample_clk;

	reg[2:0]clk_cnt2;
	always @(posedge clk)
	begin
		clk_cnt2 <= clk_cnt2+1;

	end
	assign ad9231_sample_clk = adc_clk;
	
	reg[15:0] cha0_dat;
	reg[15:0] cha1_dat;
	reg[15:0] cha2_dat;
	reg[15:0] cha3_dat;
	
	always @(posedge adc_clk or negedge rst_n)
	begin
		if(!rst_n)
		begin
			cha0_dat <=16'h123;
			cha2_dat <=16'h234;
		end
		else
		begin
			cha0_dat <= cha0_dat + 15'h1;
			cha2_dat <= cha2_dat + 15'h3;
		end
	end
	
	always @(negedge adc_clk or negedge rst_n)
	begin
		if(!rst_n)
		begin
			cha1_dat <=16'h222;
			cha3_dat <=16'h444;
		end
		else
		begin
			cha1_dat <= cha1_dat + 15'h2;
			cha3_dat <= cha3_dat + 15'h4;
		end

	end



	
	
	spi_slave_for_stm32 test_spi_stm3222
	(

		.clk_100m( clk ),
		.rst_n( rst_n ),
 
		.ad9238_chip_a_clk( ad9231_sample_clk ),
		.ad9238_chip_a_dclk( ad9231_sample_clk ),
		.ad9238_chip_a_data( cha0_dat ),

		.ad9238_chip_b_clk( ad9231_sample_clk ),
		.ad9238_chip_b_dclk( ad9231_sample_clk ),
		.ad9238_chip_b_data( cha2_dat )

	);
	
endmodule

