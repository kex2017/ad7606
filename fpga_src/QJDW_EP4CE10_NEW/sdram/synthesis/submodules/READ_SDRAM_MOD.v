module READ_SDRAM
			(
               // inputs:
               	clk,
               	reset_n,

               	read_en,
               	data,
               	addr,
               	dev_idle,
               	data_avalid,

                /*************************/
                master_waitequest,
                master_readdata,
                master_readdatavalid,
               // outputs:

                /*************************/
                master_address,
                master_byteenable,
                master_read
             );

	input clk;
	input reset_n;
	input  read_en;

	input[31:0]  addr;
	output dev_idle;//SDRAM空闲，可以进行下一次读传输
	output data_avalid;//通知外部逻辑要求的数据已经读到
	output[15:0] data;

	input master_waitequest;
	input [15:0]master_readdata;
    input master_readdatavalid;

    output [31:0] master_address;
    output [1:0]  master_byteenable;
    output master_read;


    wire clk;
    wire reset_n;
    wire read_en;
    reg[15:0] data;
	wire[31:0] addr;
	reg dev_idle;//SDRAM空闲，可以进行下一次传输
	reg data_avalid;

	wire master_waitequest;
	wire[15:0]master_readdata;
	wire master_readdatavalid;


    reg[31:0] master_address;
    reg[1:0]  master_byteenable;
    reg       master_read;




	reg[2:0] READ_SDRAM_CS;
	reg[2:0] READ_SDRAM_NS;
	reg[15:0] data_next;
	reg data_avalid_next;


	parameter IDLE    	= 3'h0;
	parameter START    	= 3'h1;
	parameter READING   = 3'h2;
	parameter FINISH    = 3'h3;



	reg[15:0] TIME_CNT;
	reg[15:0] TIME_CNT_NEXT;

	always @( * )
		master_byteenable <= 2'b11;

	always @( * )
	begin
		if(TIME_CNT == 15'h64)
			TIME_CNT_NEXT = 7'h0;
		else
			TIME_CNT_NEXT = TIME_CNT + 15'h1;
	end

	always @(posedge clk or negedge reset_n)
	begin
		if(!reset_n)
			TIME_CNT <= 15'h0;
		else if(READ_SDRAM_CS==IDLE)
			TIME_CNT <= 15'h0;
		else if(READ_SDRAM_CS!=FINISH)
			TIME_CNT <= 15'h0;
		else
			TIME_CNT <= TIME_CNT_NEXT;
	end





	always @(posedge clk or negedge reset_n)
	begin
		if(reset_n==1'b0)
			READ_SDRAM_CS <= IDLE;
		else
			READ_SDRAM_CS <= READ_SDRAM_NS;
	end

	always @( * )
	begin
		case(READ_SDRAM_CS)
		IDLE://空闲状态，一旦发现读使能信号有效则转入 开始状态
			if(read_en)
				READ_SDRAM_NS = START;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		START://在该状态下检查master_waitequest是否有效，并把地址准备好
		begin
			if( !master_waitequest)
				READ_SDRAM_NS = READING;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		end
		READING://在WRITEING状态发出读使能信号，并检查master_readdatavalid是否有效
			if(!master_waitequest)
				READ_SDRAM_NS = FINISH;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		FINISH://在该状态下读入数据，并对外发出数据有效脉冲
			if(master_readdatavalid)
				READ_SDRAM_NS = IDLE;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		default:
			READ_SDRAM_NS = IDLE;
		endcase
	end

	always@( * )//在IDLE状态下通知外面设备SDRAM空闲，可以进行数据传输
	begin
		if(READ_SDRAM_CS == IDLE)
			dev_idle <= 1'b1;
		else
			dev_idle <= 1'b0;
	end

	always@( * )//在START状态下检查master_waitequest是否有效，并把地址准备好
	begin
		if(READ_SDRAM_CS == START)
		begin
			master_address <= addr;
		end
	end

	always@( * )////在READING状态发出写使能信号
	begin
		if(READ_SDRAM_CS == READING)
			master_read <= 1'b1;
		else
			master_read <= 1'b0;
	end

	always@( * )////在FINISH状态读入数据
	begin
		if( (READ_SDRAM_CS == FINISH)&&(master_readdatavalid))
			data_next <= master_readdata[15:0];
		else
			data_next <= data;
	end

	always @(posedge clk or negedge reset_n)
	begin
		if(!reset_n)
			data <= 15'h0;
		else
			data <= data_next;
	end

	always@( * )////在FINISH状态对外发出数据有效脉冲
	begin
		if(READ_SDRAM_CS == FINISH)
			data_avalid_next <= 1'h1;
		else
			data_avalid_next <= 1'h0;
	end

	always @(posedge clk or negedge reset_n)
	begin
		if(!reset_n)
			data_avalid <= 1'h0;
		else
			data_avalid <= data_avalid_next;
	end


endmodule