

module DETECT_L2H_SIG2(clk_100m,reset_n,sig,detect_sig_l2h);
	input clk_100m;
	input reset_n;
	input sig;
	output detect_sig_l2h;

	wire clk_100m;
	wire reset_n;
	wire sig;
	wire detect_sig_l2h;

	reg L2H_F1;
	reg L2H_F2;

	always @ (posedge clk_100m or negedge reset_n)
	if(reset_n==1'b0)
		begin
			L2H_F1 <= 1'b0;
			L2H_F2 <= 1'b0;
		end
	else
		begin
			L2H_F1 <= sig;
			L2H_F2 <= L2H_F1;
		end

	assign detect_sig_l2h = ( !L2H_F2 & L2H_F1 );
endmodule


module DETECT_L2H_SIG(clk_100m,reset_n,sig,detect_sig_l2h);
	input clk_100m;
	input reset_n;
	input sig;
	output detect_sig_l2h;

	wire clk_100m;
	wire reset_n;
	wire sig;
	wire detect_sig_l2h;

	reg L2H_F1;
	reg L2H_F2;

	always @ (posedge clk_100m or negedge reset_n)
	if(reset_n==1'b0)
		begin
			L2H_F1 <= 1'b0;
			L2H_F2 <= 1'b0;
		end
	else
		begin
			L2H_F1 <= sig;
			L2H_F2 <= L2H_F1;
		end

	assign detect_sig_l2h = ( !L2H_F2 & L2H_F1 );
endmodule


module DETECT_H2L_SIG(clk_100m,reset_n,sig,detect_sig_h2l)/*synthesis noprune*/;
	input clk_100m;
	input reset_n;
	input sig;
	output detect_sig_h2l;

	wire clk_100m;
	wire reset_n;
	wire sig;
	wire detect_sig_h2l;

	reg H2L_F1/*synthesis noprune*/;
	reg H2L_F2/*synthesis noprune*/;

	always @ (posedge clk_100m or negedge reset_n)
	if(reset_n==1'b0)
		begin
			H2L_F1 <= 1'b0;
			H2L_F2 <= 1'b0;
		end
	else
		begin
			H2L_F1 <= sig;
			H2L_F2 <= H2L_F1;
		end

	assign detect_sig_h2l = ( H2L_F2 & !H2L_F1 );
endmodule