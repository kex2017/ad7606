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
	output dev_idle;//SDRAM���У����Խ�����һ�ζ�����
	output data_avalid;//֪ͨ�ⲿ�߼�Ҫ��������Ѿ�����
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
	reg dev_idle;//SDRAM���У����Խ�����һ�δ���
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
		IDLE://����״̬��һ�����ֶ�ʹ���ź���Ч��ת�� ��ʼ״̬
			if(read_en)
				READ_SDRAM_NS = START;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		START://�ڸ�״̬�¼��master_waitequest�Ƿ���Ч�����ѵ�ַ׼����
		begin
			if( !master_waitequest)
				READ_SDRAM_NS = READING;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		end
		READING://��WRITEING״̬������ʹ���źţ������master_readdatavalid�Ƿ���Ч
			if(!master_waitequest)
				READ_SDRAM_NS = FINISH;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		FINISH://�ڸ�״̬�¶������ݣ������ⷢ��������Ч����
			if(master_readdatavalid)
				READ_SDRAM_NS = IDLE;
			else
				READ_SDRAM_NS = READ_SDRAM_CS;
		default:
			READ_SDRAM_NS = IDLE;
		endcase
	end

	always@( * )//��IDLE״̬��֪ͨ�����豸SDRAM���У����Խ������ݴ���
	begin
		if(READ_SDRAM_CS == IDLE)
			dev_idle <= 1'b1;
		else
			dev_idle <= 1'b0;
	end

	always@( * )//��START״̬�¼��master_waitequest�Ƿ���Ч�����ѵ�ַ׼����
	begin
		if(READ_SDRAM_CS == START)
		begin
			master_address <= addr;
		end
	end

	always@( * )////��READING״̬����дʹ���ź�
	begin
		if(READ_SDRAM_CS == READING)
			master_read <= 1'b1;
		else
			master_read <= 1'b0;
	end

	always@( * )////��FINISH״̬��������
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

	always@( * )////��FINISH״̬���ⷢ��������Ч����
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