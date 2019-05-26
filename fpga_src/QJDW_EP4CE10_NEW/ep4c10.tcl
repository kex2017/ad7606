set_location_assignment PIN_23    -to 	clk_in 
#FPGA READ AD9231
set_location_assignment PIN_144   -to 	ADG1_ORB            
set_location_assignment PIN_143   -to 	ADG1_DCLK          
set_location_assignment PIN_142   -to 	1ADG_D[0]             
set_location_assignment PIN_141   -to 	1ADG_D[1]             
set_location_assignment PIN_138   -to 	1ADG_D[2]             
set_location_assignment PIN_137   -to 	1ADG_D[3]             
set_location_assignment PIN_136   -to 	1ADG_D[4]             
set_location_assignment PIN_135   -to 	1ADG_D[5]             
set_location_assignment PIN_133   -to 	1ADG_D[6]             
set_location_assignment PIN_132   -to 	1ADG_D[7]             
set_location_assignment PIN_129   -to 	1ADG_D[8]             
set_location_assignment PIN_128   -to 	1ADG_D[9]             
set_location_assignment PIN_127   -to 	1ADG_D[10]            
set_location_assignment PIN_126   -to 	1ADG_D[11]            
set_location_assignment PIN_125   -to 	ADG1_ORA            
set_location_assignment PIN_124   -to 	ADG1_CLK            

#FPGA cfg ad9231                    
set_location_assignment PIN_98    -to 	ADG1_CS    
set_location_assignment PIN_84    -to 	ADG_SDIO            
set_location_assignment PIN_85    -to 	ADG_SCLK  
         
        
    
#stm32 communicate fpga by spi          
set_location_assignment PIN_89    -to 	QSPI_D1_CS 
set_location_assignment PIN_83    -to 	QSPI_D2_MOSI        
set_location_assignment PIN_80    -to 	QSPI_D3_MISO        
set_location_assignment PIN_88    -to 	QSPI_CLK_SPI1_SCK   

            
         
        
set_location_assignment PIN_77    -to 	GPS_1S              
set_location_assignment PIN_76    -to 	SYNC_1Hz_From_STM32    
set_location_assignment PIN_74    -to 	FPGA_INT1 

        
set_location_assignment PIN_75    -to 	ADH_PDWN               
 
          
set_location_assignment PIN_73    -to 	S_DB[6]               
set_location_assignment PIN_72    -to 	S_DQM[0]              
set_location_assignment PIN_71    -to 	S_DB[7]               
set_location_assignment PIN_70    -to 	S_NWE               
set_location_assignment PIN_69    -to 	S_NCAS              
set_location_assignment PIN_68    -to 	S_NRAS              
set_location_assignment PIN_67    -to 	S_NCS               
set_location_assignment PIN_66    -to 	S_BA[0]               
set_location_assignment PIN_65    -to 	S_BA[1]               
set_location_assignment PIN_64    -to 	S_A[10]              
set_location_assignment PIN_60    -to 	S_A[0]                
set_location_assignment PIN_59    -to 	S_A[1]                
set_location_assignment PIN_58    -to 	S_A[2]                
set_location_assignment PIN_55    -to 	S_A[3]                
set_location_assignment PIN_54    -to 	S_DB[5]               
set_location_assignment PIN_53    -to 	S_DB[4]               
set_location_assignment PIN_52    -to 	S_DB[3]               
set_location_assignment PIN_51    -to 	S_DB[2]               
set_location_assignment PIN_50    -to 	S_DB[1]               
set_location_assignment PIN_49    -to 	S_DB[0]               
set_location_assignment PIN_46    -to 	S_DB[15]              
set_location_assignment PIN_44    -to 	S_DB[14]              
set_location_assignment PIN_43    -to 	S_DB[13]              
set_location_assignment PIN_42    -to 	S_DB[12]              
set_location_assignment PIN_39    -to 	S_DB[11]              
set_location_assignment PIN_38    -to 	S_DB[10]              
set_location_assignment PIN_34    -to 	S_DB[9]               
set_location_assignment PIN_33    -to 	S_DB[8]               
set_location_assignment PIN_32    -to 	S_DQM[1]              
set_location_assignment PIN_31    -to 	S_CLK               
set_location_assignment PIN_30    -to 	S_CKE               
set_location_assignment PIN_28    -to 	S_A[11]               
set_location_assignment PIN_2     -to 	S_A[9]                
set_location_assignment PIN_1     -to 	S_A[8]                
             
set_location_assignment PIN_20    -to 	FTDO                
set_location_assignment PIN_18    -to 	FTMS                
set_location_assignment PIN_16    -to 	FTCK                
set_location_assignment PIN_15    -to 	FTDI                
set_location_assignment PIN_14    -to 	FCFG_NCFG           
set_location_assignment PIN_13    -to 	FCFG_DATA0          
set_location_assignment PIN_12    -to 	FCFG_DCLK           
set_location_assignment PIN_11    -to 	S_A[7]                
set_location_assignment PIN_10    -to 	S_A[6]                
set_location_assignment PIN_9     -to 	FCFG_NSTS           
set_location_assignment PIN_8     -to 	S_A[5]                
set_location_assignment PIN_7     -to 	S_A[4]                
set_location_assignment PIN_3     -to 	I_OVER3     


set_location_assignment PIN_86    -to 	ADG_SYNC    
set_location_assignment PIN_92    -to 	FCFG_DONE          
