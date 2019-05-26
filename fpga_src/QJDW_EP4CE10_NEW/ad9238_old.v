

module DEAL_FPGA_TO_MCU_INTERRUPT(clk_100m,reset_n,interrupt_sig_in,one_second_clk_in,interrupt_sig_out);
	input clk_100m;
	input reset_n;
	input interrupt_sig_in;
	input one_second_clk_in;
	output interrupt_sig_out;

	wire clk_100m;
	wire reset_n;
	wire interrupt_sig_in;
	wire one_second_clk_in;
	wire interrupt_sig_out;
	wire one_second_clk_l2h;
	
	reg interrupt_out_reg;
	reg[7:0] one_second_clk_cnt;
	wire [7:0]one_second_clk_cnt_next;
	
	always @ (posedge clk_100m or negedge reset_n)
	begin
		if(reset_n==1'b0)
				one_second_clk_cnt <= 0;
		else
		begin
				if(one_second_clk_l2h)
					one_second_clk_cnt <= one_second_clk_cnt_next + 1;
				else
					one_second_clk_cnt <= one_second_clk_cnt_next;
		end
	end
	assign one_second_clk_cnt_next = one_second_clk_cnt;

	DETECT_L2H_SIG deal_fpga_to_mcu_interrupt_detect_one_second_clk_l2h
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.sig( one_second_clk_in ),
		.detect_sig_l2h( one_second_clk_l2h )
	);

	always @ (posedge clk_100m or negedge reset_n)
	begin
 		if(reset_n==1'b0)
			interrupt_out_reg <= 1'b0;
		else
		begin
			if( (interrupt_sig_in)&&((one_second_clk_cnt%4)==0) )
				interrupt_out_reg <= 1;
			else
				interrupt_out_reg <= 0;
		end
	end
	assign interrupt_sig_out = interrupt_out_reg;


endmodule



module div5(reset_n,div_enable,clkin,clkout);
	input reset_n;
	input div_enable;
	input clkin;
	output clkout;

	wire reset_n;
	wire div_enable;
	wire clkin;
	wire clkout;

	reg [2:0]step1;
	reg [2:0]step;
	always @(posedge clkin or negedge reset_n)
	begin
		if(!reset_n)
			step<=3'b000;
		else
		begin
			case(step)
				3'b000: step<=3'b010;
				3'b010: step<=3'b100;
				3'b100: step<=3'b001;
				3'b001: step<=3'b011;
				3'b011: step<=3'b000;
				default: step<=3'b000;
			endcase
		end
	end

	always @(negedge clkin or negedge reset_n)
	begin
		if(!reset_n)
			step1<=3'b000;
		else
		begin
			case(step1)
			3'b000: step1<=3'b010;
			3'b010: step1<=3'b100;
			3'b100: step1<=3'b001;
			3'b001: step1<=3'b011;
			3'b011: step1<=3'b000;
			default: step1<=3'b000;
			endcase
		end
	end
	wire clk_tmp;
	assign clk_tmp = div_enable ? (step[0]|step1[0]):1'b0;
	assign clkout = ~clk_tmp;
endmodule

module div100000(reset_n,div_enable,clkin,clkout);
	input reset_n;
	input div_enable;
	input clkin;
	output clkout;

	wire reset_n;
	wire div_enable;
	wire clkin;
	reg clkout;

	reg [16:0]clk_cnt;
	always @(posedge clkin or negedge reset_n)
	begin
		if(!reset_n)
			clk_cnt<=0;
		else
		begin
			if(clk_cnt < 100000)
				clk_cnt <= clk_cnt + 1;
			else
				clk_cnt <= 0;
		end
	end

	always @(negedge clkin or negedge reset_n)
	begin
		if(!reset_n)
			clkout <= 1'b0;
		else
		begin
			if(clk_cnt<=49999)
				clkout <= 1'b0;
			else
				clkout <= 1'b1;
		end
	end
endmodule



module CREAT_20MHZ_AD9238_CLK(clk_100m,reset_n,ad9238_start_one_sample_flag,ad9238_clk,ad9238_one_sample_over);
	input clk_100m;
	input reset_n;
	input ad9238_start_one_sample_flag;
	output ad9238_clk;
	output ad9238_one_sample_over;


	wire clk_100m;
	wire reset_n;
	wire ad9238_start_one_sample_flag;
	wire ad9238_clk;
	reg ad9238_one_sample_over;

	reg[31:0] ad9238_sample_dat_num;
	wire[31:0] ad9238_sample_dat_num_next;

	reg[9:0] count1;
	wire[9:0] count1_next;

	parameter AD9238_ONE_SAMPLE_DAT_NUM    = 20000000;
	reg ad_sample_enable;
	wire ad_sample_enable_next;

	reg ad_sample_flag;

	always @ ( posedge clk_100m or negedge reset_n )
    begin
		if( !reset_n )
			ad_sample_enable <= 1'b0;
		else
		if(ad9238_start_one_sample_flag)
			ad_sample_enable <= 1'b1;
		else if( ad9238_sample_dat_num >= AD9238_ONE_SAMPLE_DAT_NUM)
			ad_sample_enable <= 1'b0;
		else
			ad_sample_enable <= ad_sample_enable_next;
	end
	assign ad_sample_enable_next = ad_sample_enable;


	div5 div_100m_clk
	(
		.reset_n(reset_n),
		.div_enable( 1'b1 ),
		.clkin(  clk_100m ),
		.clkout(ad9238_clk)
	);

/*	div100000 div_100m_clk_to_1k
	(
		.reset_n(reset_n),
		.div_enable( 1'b1 ),
		.clkin(  clk_100m ),
		.clkout(ad9238_clk)
	);*/
	wire ad9238_clk_l2h;

	DETECT_L2H_SIG detect_ad9238_clk_l2h
	(
		.clk_100m(clk_100m),
		.reset_n(reset_n),
		.sig(ad9238_clk),
		.detect_sig_l2h(ad9238_clk_l2h)
	);


	always @ ( posedge clk_100m  )
	begin
		if( (count1==0)&&(ad_sample_enable) )
			ad_sample_flag <= 1'b1;
		else
			ad_sample_flag <= 1'b0;
	end

	always @ ( posedge clk_100m or negedge reset_n  )
	begin
		if( !reset_n )
			ad9238_sample_dat_num <= 32'h0;
		else
		begin
			if(ad9238_start_one_sample_flag)
				ad9238_sample_dat_num <= 32'h0;
			else if( ad9238_clk_l2h )
				ad9238_sample_dat_num <= ad9238_sample_dat_num_next + 1'b1;
			else
				ad9238_sample_dat_num <= ad9238_sample_dat_num_next;
		end
	end
	assign ad9238_sample_dat_num_next = ad9238_sample_dat_num;


	always @ ( posedge clk_100m or negedge reset_n  )
	begin
		if( !reset_n )
			ad9238_one_sample_over <= 1'b0;
		else
		begin
			if( ad9238_sample_dat_num >= AD9238_ONE_SAMPLE_DAT_NUM )
				ad9238_one_sample_over <= 1'b1;
			else
				ad9238_one_sample_over <= 1'b0;
		end
	end


endmodule









module READ_AD9231_DAT_FOR_BREAKOUT_V_MOD
	(
		clk_100m,
		reset_n,
		AD9238A_DAT_IN,AD9238B_DAT_IN,AD9238_CHIP_A_DCLK,AD9238_CHIP_B_DCLK,
		AD9238_CHA0_DAT_OUT,AD9238_CHA1_DAT_OUT,AD9238_CHA2_DAT_OUT,AD9238_CHA3_DAT_OUT,
		ad9238_new_ad_dat_avalible
	)/*synthesis noprune*/;
	input clk_100m;
	input reset_n;
	input[11:0] AD9238A_DAT_IN;
	input[11:0] AD9238B_DAT_IN;
	input AD9238_CHIP_A_DCLK;
	input AD9238_CHIP_B_DCLK;

	output[11:0] AD9238_CHA0_DAT_OUT;
	output[11:0] AD9238_CHA1_DAT_OUT;
	output[11:0] AD9238_CHA2_DAT_OUT;
	output[11:0] AD9238_CHA3_DAT_OUT;
	output ad9238_new_ad_dat_avalible;


	wire clk_100m;
	wire reset_n;
	wire[11:0] AD9238A_DAT_IN;
	wire[11:0] AD9238B_DAT_IN;
	wire AD9238_CHIP_A_DCLK;
	wire AD9238_CHIP_B_DCLK;

	reg[11:0] AD9238_CHA0_DAT_OUT/*synthesis noprune*/;
	reg[11:0] AD9238_CHA1_DAT_OUT/*synthesis noprune*/;
	reg[11:0] AD9238_CHA2_DAT_OUT/*synthesis noprune*/;
	reg[11:0] AD9238_CHA3_DAT_OUT/*synthesis noprune*/;
	reg ad9238_new_ad_dat_avalible/*synthesis noprune*/;


	reg[11:0] ad9238_cha0_ad_dat/*synthesis noprune*/;
	reg[11:0] ad9238_cha1_ad_dat/*synthesis noprune*/;
	reg[11:0] ad9238_cha2_ad_dat/*synthesis noprune*/;
	reg[11:0] ad9238_cha3_ad_dat/*synthesis noprune*/;


	reg ad_chip_a_clk_d1cyc/*synthesis noprune*/;
	reg ad_chip_b_clk_d1cyc/*synthesis noprune*/;


	reg  [11:0]cha0_one_pha_max/*synthesis noprune*/;
	wire [11:0]cha0_one_pha_max_next;
	reg  [11:0]cha1_one_pha_max/*synthesis noprune*/;
	wire [11:0]cha1_one_pha_max_next;
	reg  [11:0]cha2_one_pha_max/*synthesis noprune*/;
	wire [11:0]cha2_one_pha_max_next;
	reg  [11:0]cha3_one_pha_max/*synthesis noprune*/;
	wire [11:0]cha3_one_pha_max_next;

	wire[11:0]AD9238_CHA0_DAT_OUT_next/*synthesis noprune*/;
	wire[11:0]AD9238_CHA1_DAT_OUT_next/*synthesis noprune*/;
	wire[11:0]AD9238_CHA2_DAT_OUT_next/*synthesis noprune*/;
	wire[11:0]AD9238_CHA3_DAT_OUT_next/*synthesis noprune*/;

	reg[15:0]cha02_dat_cnt/*synthesis noprune*/;
	wire[15:0]cha02_dat_cnt_next;
	reg[15:0]cha13_dat_cnt/*synthesis noprune*/;
	wire[15:0]cha13_dat_cnt_next;

	wire ad9238_clk_l2h;
	wire ad9238_clk_h2l;




	DETECT_L2H_SIG detect_ad9238_clk_l2h
	(
		.clk_100m(clk_100m),
		.reset_n(reset_n),
		.sig(AD9238_CLK),
		.detect_sig_l2h(ad9238_clk_l2h)
	);

	DETECT_H2L_SIG detect_ad9238_clk_h2l
	(
		.clk_100m(clk_100m),
		.reset_n(reset_n),
		.sig(AD9238_CHIP_A_DCLK),
		.detect_sig_h2l(ad9238_clk_h2l)
	);

	always @ ( posedge clk_100m )
	begin
		ad_chip_a_clk_d1cyc <= AD9238_CHIP_A_DCLK;
	end
	
	always @ ( posedge clk_100m )
	begin
		ad_chip_b_clk_d1cyc <= AD9238_CHIP_B_DCLK;
	end


   	always @(negedge AD9238_CHIP_A_DCLK)
   	begin
			ad9238_cha0_ad_dat <= AD9238A_DAT_IN;
   	end

   	always @(posedge AD9238_CHIP_A_DCLK)
   	begin
			ad9238_cha1_ad_dat <= AD9238A_DAT_IN;
   	end


   	always @(negedge AD9238_CHIP_B_DCLK)
   	begin
			ad9238_cha2_ad_dat <= AD9238B_DAT_IN;
   	end



   	always @(posedge AD9238_CHIP_B_DCLK)
   	begin
			ad9238_cha3_ad_dat <= AD9238B_DAT_IN;
   	end


/*
   	always @( * )
	begin
		AD9238_CHA1_DAT_OUT <= ad9238_cha0_ad_dat;
		AD9238_CHA0_DAT_OUT <= ad9238_cha1_ad_dat;
		AD9238_CHA3_DAT_OUT <= ad9238_cha2_ad_dat;
		AD9238_CHA2_DAT_OUT <= ad9238_cha3_ad_dat;
	end
	*/
	
	always @( * )
	begin
		AD9238_CHA0_DAT_OUT <= ad9238_cha3_ad_dat;
		AD9238_CHA1_DAT_OUT <= ad9238_cha2_ad_dat;
		AD9238_CHA2_DAT_OUT <= ad9238_cha0_ad_dat;
		AD9238_CHA3_DAT_OUT <= ad9238_cha1_ad_dat;
	end

	always @( * )
		ad9238_new_ad_dat_avalible <= ad9238_clk_h2l;



endmodule



module SAVE_AD9238_CHAx_DAT_TO_FIFO(
	clk_100m,
	reset_n,
	ad9238_new_ad_dat_avalible,
	ad9238_chax_dat,
	ad_dat_thr,
	change_rate_thr,
	write_fifo_en,
	write_fifo_data,
	fifo_idle,
	save_dat_to_fifo_flag,
	one_second_clk);


	input clk_100m;
	input reset_n;
	input ad9238_new_ad_dat_avalible;
	input[11:0]ad9238_chax_dat;
	input[11:0]ad_dat_thr;
	input[11:0]change_rate_thr;
	output write_fifo_en;
	output[11:0] write_fifo_data;
	input fifo_idle;
	output save_dat_to_fifo_flag;
	input wire one_second_clk;

	wire clk_100m;
	wire reset_n;
	wire clear_write_sdram_addr;
	wire ad9238_new_ad_dat_avalible;
	wire[11:0]ad9238_chax_dat;
	wire[11:0]ad_dat_thr;
	wire[11:0]change_rate_thr;
	reg write_fifo_en;
	reg[11:0] write_fifo_data;
	wire fifo_idle;
	reg save_dat_to_fifo_flag;

	reg save_ad923x_dat_to_fifo_flag;
	
	
	

	

	reg[15:0] dat_tmp;
	wire[15:0] dat_tmp_next;

	reg[31:0] addr_tmp;
	wire[31:0] addr_tmp_next;
	
	reg[15:0]read_ad_cnt;
	wire[15:0]read_ad_cnt_next;
	
	
	
	parameter WRITE_BREAKOUT_V_DAT_CNT    	= 4096;
	parameter ZLPY = -30;//ֱ��ƫ������ƫ��30
	
	reg[11:0]dat_max;
	reg[11:0]dat_min;
	reg[19:0]check_dat_num;
	
	wire one_second_clk_l2h;
	
	DETECT_L2H_SIG save_dat_to_fifo_one_second_clk_l2h
	(
		.clk_100m( clk_100m ),
		.reset_n( reset_n ),
		.sig( one_second_clk ),
		.detect_sig_l2h( one_second_clk_l2h )
	);
	
	always @(posedge clk_100m or negedge reset_n)
   	begin
   		if(!reset_n)
   			dat_max <= 0;
   		else
   		begin
   			if( (ad9238_new_ad_dat_avalible)&&(check_dat_num<1000000) )
   			begin
   				if(dat_max<ad9238_chax_dat)
   					dat_max <= ad9238_chax_dat;
   				else
   					dat_max <= dat_max;
   			end
   			else 
   				dat_max <= 0;
   		end
   	end
   	
   	reg[7:0]clk_100m_cnt;
   	always @(posedge clk_100m or negedge reset_n)
   	begin
   		if(!reset_n)
   			clk_100m_cnt <= 0;
   		else
   		begin
   			if(clk_100m_cnt<100)
   				clk_100m_cnt <= clk_100m_cnt + 1;
   			else
   				clk_100m_cnt <= 0;
   		end
   	end
   			
   			
   	
	
	//��1M��Ƶ�ʸ������ݣ������ᵼ��32�����ݱ仯��̫С����ֵ�������á�
	reg[11:0]chax_dat_buf[31:0];
	reg[959:0]chax_dat_reg;
	always @(posedge clk_100m or negedge reset_n)
   	begin
   		if(!reset_n)
   			chax_dat_reg <= 0;
   		else
   			chax_dat_reg <= {chax_dat_reg[947:0],ad9238_chax_dat};
   	end
	always @(posedge clk_100m or negedge reset_n)
   	begin
   		if(!reset_n)
   		begin
			chax_dat_buf[0]  <= 12'h000;
			chax_dat_buf[1]  <= 12'h000;
			chax_dat_buf[2]  <= 12'h000;
			chax_dat_buf[3]  <= 12'h000;
			chax_dat_buf[4]  <= 12'h000;
			chax_dat_buf[5]  <= 12'h000;
			chax_dat_buf[6]  <= 12'h000;
			chax_dat_buf[7]  <= 12'h000;
			chax_dat_buf[8]  <= 12'h000;
			chax_dat_buf[9]  <= 12'h000;
			chax_dat_buf[10] <= 12'h000;
			chax_dat_buf[11] <= 12'h000;
			chax_dat_buf[12] <= 12'h000;
			chax_dat_buf[13] <= 12'h000;
			chax_dat_buf[14] <= 12'h000;
			chax_dat_buf[15] <= 12'h000;
			
			chax_dat_buf[16] <= 12'h000;
			chax_dat_buf[17] <= 12'h000;
			chax_dat_buf[18] <= 12'h000;
			chax_dat_buf[19] <= 12'h000;
			chax_dat_buf[20] <= 12'h000;
			chax_dat_buf[21] <= 12'h000;
			chax_dat_buf[22] <= 12'h000;
			chax_dat_buf[23] <= 12'h000;
			chax_dat_buf[24] <= 12'h000;
			chax_dat_buf[25] <= 12'h000;
			chax_dat_buf[26] <= 12'h000;
			chax_dat_buf[27] <= 12'h000;
			chax_dat_buf[28] <= 12'h000;
			chax_dat_buf[29] <= 12'h000;
			chax_dat_buf[30] <= 12'h000;
			chax_dat_buf[31] <= 12'h000;
			
   		end
   		else
   		begin
			if( (ad9238_new_ad_dat_avalible))
			begin
				chax_dat_buf[0]  <= ad9238_chax_dat;
				chax_dat_buf[1]  <= chax_dat_buf[0]; 
				chax_dat_buf[2]  <= chax_dat_buf[1]; 
				chax_dat_buf[3]  <= chax_dat_buf[2]; 
				chax_dat_buf[4]  <= chax_dat_buf[3]; 
				chax_dat_buf[5]  <= chax_dat_buf[4]; 
				chax_dat_buf[6]  <= chax_dat_buf[5]; 
				chax_dat_buf[7]  <= chax_dat_buf[6]; 
				chax_dat_buf[8]  <= chax_dat_buf[7]; 
				chax_dat_buf[9]  <= chax_dat_buf[8]; 
				chax_dat_buf[10] <= chax_dat_buf[9]; 
				chax_dat_buf[11] <= chax_dat_buf[10];
				chax_dat_buf[12] <= chax_dat_buf[11];
				chax_dat_buf[13] <= chax_dat_buf[12];
				chax_dat_buf[14] <= chax_dat_buf[13];
				chax_dat_buf[15] <= chax_dat_buf[14];
								
				chax_dat_buf[16] <= chax_dat_buf[15];
				chax_dat_buf[17] <= chax_dat_buf[16];
				chax_dat_buf[18] <= chax_dat_buf[17];
				chax_dat_buf[19] <= chax_dat_buf[18];
				chax_dat_buf[20] <= chax_dat_buf[19];
				chax_dat_buf[21] <= chax_dat_buf[20];
				chax_dat_buf[22] <= chax_dat_buf[21];
				chax_dat_buf[23] <= chax_dat_buf[22];
				chax_dat_buf[24] <= chax_dat_buf[23];
				chax_dat_buf[25] <= chax_dat_buf[24];
				chax_dat_buf[26] <= chax_dat_buf[25];
				chax_dat_buf[27] <= chax_dat_buf[26];
				chax_dat_buf[28] <= chax_dat_buf[27];
				chax_dat_buf[29] <= chax_dat_buf[28];
				chax_dat_buf[30] <= chax_dat_buf[29];
				chax_dat_buf[31] <= chax_dat_buf[30];
			end
   		end
   	end


	
	always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			read_ad_cnt <= 0;
		else
		begin
			if( (ad9238_new_ad_dat_avalible )&&(read_ad_cnt<=4096) )
				read_ad_cnt <= read_ad_cnt+ 1;
			else
				read_ad_cnt <= read_ad_cnt;
		end
	end
	
	always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			save_ad923x_dat_to_fifo_flag <= 0;
		else
		begin
			if(write_ad923x_dat_to_fifo_cs==WRITE_SUCCESS)
				save_ad923x_dat_to_fifo_flag <= 0;
			else if( (chax_dat_buf[0]>=(2048+ad_dat_thr+ZLPY))||(chax_dat_buf[0]<=(2048-ad_dat_thr+ZLPY)) )
				save_ad923x_dat_to_fifo_flag <= 1;
			else if( (chax_dat_buf[0]>chax_dat_buf[31] )&&((chax_dat_buf[0]-chax_dat_buf[31])>=change_rate_thr) )
				save_ad923x_dat_to_fifo_flag <= 1;
			else if( (chax_dat_buf[0]<chax_dat_buf[31] )&&((chax_dat_buf[31]-chax_dat_buf[0])>=change_rate_thr) )
				save_ad923x_dat_to_fifo_flag <= 1;
			else
				save_ad923x_dat_to_fifo_flag <= 0;
		end
	end
	

	reg [5:0]  write_ad923x_dat_to_fifo_cs/*synthesis noprune*/;
	reg [5:0]  write_ad923x_dat_to_fifo_ns;


	parameter IDLE    					= 6'b000001;
	parameter START    					= 6'b000010;
	parameter PREPARE_DAT_AND_ADDR    	= 6'b000100;
	parameter WRITE_EN  				= 6'b001000;
	parameter WAIT_WRITE_SUCCESS    	= 6'b010000;
	parameter WRITE_SUCCESS    	        = 6'b100000;


	
	reg[15:0]success_write_dat_count;

	reg fifo_empty_flag;
	wire fifo_empty_flag_next;

	always @( * )
	begin
		case(write_ad923x_dat_to_fifo_cs)
		IDLE: 
		begin
			if( (save_ad923x_dat_to_fifo_flag)/*&&(fifo_empty_flag)&&(read_ad_cnt>=4096)*/ )
				write_ad923x_dat_to_fifo_ns <= START;
			else
				write_ad923x_dat_to_fifo_ns <= write_ad923x_dat_to_fifo_cs;
		end
		START: 
		begin
			if( success_write_dat_count>=(WRITE_BREAKOUT_V_DAT_CNT-1)  )
				write_ad923x_dat_to_fifo_ns <= WRITE_SUCCESS;
			else
				write_ad923x_dat_to_fifo_ns <= write_ad923x_dat_to_fifo_cs;
		end
		WRITE_SUCCESS: 
				write_ad923x_dat_to_fifo_ns <= IDLE;
		default:
			write_ad923x_dat_to_fifo_ns <= IDLE;
		endcase
	end
	
	always @(posedge clk_100m or negedge reset_n)
	begin
		if(!reset_n)
			write_ad923x_dat_to_fifo_cs <= IDLE;
		else
			write_ad923x_dat_to_fifo_cs <= write_ad923x_dat_to_fifo_ns;
	end
	
	reg[7:0]fifo_empty_flag_continue_0_time_cnt;
	
	
	always @(posedge clk_100m  or negedge reset_n)
	begin
		if(!reset_n)
			fifo_empty_flag_continue_0_time_cnt <= 0;
		else
		begin	
			if(fifo_empty_flag)
				fifo_empty_flag_continue_0_time_cnt <= 0;
			else if(one_second_clk_l2h)
				fifo_empty_flag_continue_0_time_cnt <= fifo_empty_flag_continue_0_time_cnt + 1;
			else
				fifo_empty_flag_continue_0_time_cnt <= fifo_empty_flag_continue_0_time_cnt;
		end
	end
	
	always @(posedge clk_100m  or negedge reset_n)
	begin
		if(!reset_n)
			fifo_empty_flag <= 1'b1;
		else
		begin	
			if(fifo_idle)
				fifo_empty_flag <= 1'b1;
			else if(fifo_empty_flag_continue_0_time_cnt>=10)
				fifo_empty_flag <= 1'b1;
			else if(write_ad923x_dat_to_fifo_cs==WRITE_SUCCESS)
				fifo_empty_flag <= 1'b0;
			else
				fifo_empty_flag <= fifo_empty_flag_next;
		end
	end
	assign fifo_empty_flag_next = fifo_empty_flag;
	
	wire[15:0] success_write_dat_count_next;
	always @(posedge clk_100m  or negedge reset_n)
	begin
		if(!reset_n)
			success_write_dat_count <= 16'h00;
		else if(write_ad923x_dat_to_fifo_cs == IDLE)
			success_write_dat_count <= 16'h00;
		else if( ( ad9238_new_ad_dat_avalible )&&(write_ad923x_dat_to_fifo_cs==START))
		begin
			if(success_write_dat_count<WRITE_BREAKOUT_V_DAT_CNT)
				success_write_dat_count <= success_write_dat_count_next + 1'b1;
			else
				success_write_dat_count <= success_write_dat_count_next;
		end
		else
			success_write_dat_count <= success_write_dat_count_next;
	end
	assign success_write_dat_count_next = success_write_dat_count;
	
	
	
	always @(posedge clk_100m  or negedge reset_n)
	begin
		if(!reset_n)
			write_fifo_en <= 1'b0;
		else if( write_ad923x_dat_to_fifo_cs==START )
			write_fifo_en <= ad9238_new_ad_dat_avalible;
		else
			write_fifo_en <= 1'b0;
	end
	
	
	always @(posedge clk_100m  or negedge reset_n)
	begin
		if(!reset_n)
			write_fifo_data <= 12'h0;
		else if( ad9238_new_ad_dat_avalible )
			write_fifo_data <= chax_dat_buf[31];
	end
	
	always @(posedge clk_100m  or negedge reset_n)
	begin
		if(!reset_n)
			save_dat_to_fifo_flag <= 1'b0;
		else if( write_ad923x_dat_to_fifo_cs==WRITE_SUCCESS )
			save_dat_to_fifo_flag <= 1'b1;
		else
			save_dat_to_fifo_flag <= 1'b0;
	end
	


endmodule









