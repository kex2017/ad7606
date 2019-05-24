module WRITE_SDRAM_FIFO_BURST
			(
               // inputs:
               	clk,
               	reset_n,

               	write_en,
               	data,
               	addr,
               	dev_idle,

                master_waitequest,


               // outputs:

                master_address,
                master_byteenable,
                master_write,
                master_writedata,
                master_burstcount,
                

             );

	input clk;
	input reset_n;
	input write_en;	
	input[15:0] data;
	input[31:0] addr;
	output dev_idle;//SDRAM????????????

	input master_waitequest;

    output [31:0]master_address;
    output [1:0]master_byteenable;
    output master_write;
    output [15:0]master_writedata;
    output[7:0] master_burstcount;
   // output[4:0] success_write_dat_count;



    wire clk;
    wire reset_n;
    wire[15:0] data;
	wire[31:0] addr;
	wire dev_idle;//SDRAM????????????

	wire master_waitequest;
    wire user_buffer_full;

    wire[31:0] master_address;
    wire[31:0] master_address_next;
    reg[1:0]  master_byteenable;
    wire       master_write;
    wire[15:0] master_writedata;
    reg[7:0] master_burstcount;




	wire fifo_full;
	wire[4:0] fifo_usedw;
	wire fifo_empty;
	wire fifo_rdreq;


	wire [47:0] q;



	//assign dev_idle = !fifo_full;//??????????????
	//assign dev_idle = fifo_empty;
	assign dev_idle = (fifo_usedw<31) ? 1:0;
	//assign dev_idle = ({buf_round_flag,write_buf_count} - {1'b0,read_buf_count} >30)? 0:1;



	reg[4:0] success_write_dat_count;
	wire[4:0]success_write_dat_count_next;
	reg read_fifo_en;//?fifo?????????????avalon slave???waitrequest??????????????????????fifo????????????????????????????
	reg first_into_prepare_dat_and_addr;
	reg master_write_pre;


	always @( * )
		master_byteenable <= 2'b11;
		
	always @( * )
		master_burstcount <= 16;


	reg [5:0]  WRITE_AVALON_SDRAM_CS;
	reg [5:0]  WRITE_AVALON_SDRAM_NS;


	parameter IDLE    					= 6'b000001;
	parameter START    					= 6'b000010;
	parameter PREPARE_DAT_AND_ADDR    	= 6'b000100;
	parameter WRITE_EN  				= 6'b001000;
	parameter WAIT_WRITE_SUCCESS    	= 6'b010000;
	parameter WRITE_SUCCESS    	        = 6'b100000;

	parameter BURST_CNT    				= 5'b10000;

	always @( * )
	begin
		case(WRITE_AVALON_SDRAM_CS)
		IDLE://????
		begin
			if( (fifo_usedw>=BURST_CNT)&&(master_waitequest==1'b0) )
				WRITE_AVALON_SDRAM_NS <= START;
			else
				WRITE_AVALON_SDRAM_NS <= WRITE_AVALON_SDRAM_CS;
		end
		START://在该状态进行读fifo，发出master_write信号，并监测master_waitequest信号
			if( (success_write_dat_count>=(BURST_CNT-1))&&(master_waitequest==1'b0) )
				WRITE_AVALON_SDRAM_NS <= WRITE_SUCCESS;
			else
				WRITE_AVALON_SDRAM_NS <= WRITE_AVALON_SDRAM_CS;
		WRITE_SUCCESS://监测最后一个数据是否写成功
			if(master_waitequest==1'b0)
				WRITE_AVALON_SDRAM_NS <= IDLE;
			else
				WRITE_AVALON_SDRAM_NS <= WRITE_AVALON_SDRAM_CS;
		default:
			WRITE_AVALON_SDRAM_NS <= IDLE;
		endcase
	end

	always @(posedge clk or negedge reset_n)
	begin
		if(!reset_n)
			WRITE_AVALON_SDRAM_CS <= IDLE;
		else
			WRITE_AVALON_SDRAM_CS <= WRITE_AVALON_SDRAM_NS;
	end
	
	always @(posedge clk or negedge reset_n)
	begin
		if(!reset_n)
			master_write_pre <= 1'b0;
		else
		begin
			if( (WRITE_AVALON_SDRAM_CS == START)||((WRITE_AVALON_SDRAM_CS==WRITE_SUCCESS)&&(master_waitequest==1'b1)) )
				master_write_pre <= 1'b1;
			else
				master_write_pre <= 1'b0;
		end
	end
	
	//assign fifo_rdreq = (master_write_pre&(!master_waitequest));
	assign fifo_rdreq = (WRITE_AVALON_SDRAM_CS==START) ? (master_write_pre&(!master_waitequest)):1'b0;
	
	
	assign master_writedata = q[47:32];
	//assign master_address  =  {q[31:7],7'b0000000};
	assign master_address  =  q[31:0];
	
	always @(posedge clk  or negedge reset_n)
	begin
		if(!reset_n)
			success_write_dat_count <= 5'h00;
		else if(WRITE_AVALON_SDRAM_CS == IDLE)
			success_write_dat_count <= 5'h00;
		else if( fifo_rdreq )
		begin
			if(success_write_dat_count<BURST_CNT)
				success_write_dat_count <= success_write_dat_count_next + 1'b1;
			else
				success_write_dat_count <= 1'b1;
		end
		else
			success_write_dat_count <= success_write_dat_count_next;
	end
	assign success_write_dat_count_next = success_write_dat_count;
	
	reg master_write_tmp;
	always @(posedge clk )
	begin
		if((WRITE_AVALON_SDRAM_CS == START)||(WRITE_AVALON_SDRAM_CS == WRITE_SUCCESS))
			master_write_tmp <= master_write_pre;
		else
			master_write_tmp <= 1'b0;
	end
	
	assign master_write = master_write_tmp&master_write_pre;

	write_sdram_fifo_48w_32d write_cha_dat_to_sdram_fifo (
		.clock (clk),//
		.data ({data,addr}),//
		.q (q),/*?SDRAM???????FIFO????*/
		.rdreq (fifo_rdreq),//
		.wrreq (write_en),//
		.usedw (fifo_usedw),//
		.full (fifo_full),
		.empty (fifo_empty)
	);


endmodule

