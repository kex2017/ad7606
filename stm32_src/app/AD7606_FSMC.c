/*
 * acquisition_trigger.c
 *
 *  Created on: Dec 29, 2017
 *      Author: chenzy
 */
#include "AD7606_FSMC.h"


void AD7606_EXIT4_IRQHandler(void *arg);

void fsmc_bus_pins_init(void)
{
   /* --------SRAM Data lines configuration ------------*/
   /* Set PD.00(D2), PD.01(D3), PD.08(D13), PD.09(D14), PD.10(D15), PD.14(D0), PD.15(D1) */
   gpio_init_af(GPIO_PIN(PORT_D, 0), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 1), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 8), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 9), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 10), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 14), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 15), GPIO_AF_OUT_PP);

   /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10), PE.14(D11), PE.15(D12)  */
   gpio_init_af(GPIO_PIN(PORT_E, 7), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 8), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 9), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 10), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 11), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 12), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 13), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 14), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 15), GPIO_AF_OUT_PP);

   /*-------- SRAM Address lines configuration -----------*/
   gpio_init_af(GPIO_PIN(PORT_F, 0), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 1), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 2), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 3), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 4), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 5), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 12), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 13), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 14), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_F, 15), GPIO_AF_OUT_PP);

   gpio_init_af(GPIO_PIN(PORT_G, 0), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 1), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 2), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 3), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 4), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 5), GPIO_AF_OUT_PP);

   /* Set PD11(A16)  PD12(A17)  PD13(A18)) */
   gpio_init_af(GPIO_PIN(PORT_D, 11), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 12), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 13), GPIO_AF_OUT_PP);
   /* PE3,PE4 用于A19, A20 */
   /* PE5,PE6 用于A21, A22 */
   gpio_init_af(GPIO_PIN(PORT_E, 3), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 4), GPIO_AF_OUT_PP);

   /*NOE and NWE configuration */
   // PD.04(NOE), PD.05(NWE),
   gpio_init_af(GPIO_PIN(PORT_D, 4), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_D, 5), GPIO_AF_OUT_PP);

   /* NE1, NE2, NE4 configuration */
   //PD.7(NE1 )  PG.9 NE2  PG.10 NE3  PG.12 NE4
   gpio_init_af(GPIO_PIN(PORT_D, 7), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 9), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 10), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_G, 12), GPIO_AF_OUT_PP);

   /* NBL0, NBL1 configuration*/
   gpio_init_af(GPIO_PIN(PORT_E, 0), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 1), GPIO_AF_OUT_PP);
}

void AD7606_IO_Init(void)
{
    gpio_init(OS0_PIN, GPIO_OUT);
    gpio_init(OS1_PIN, GPIO_OUT);
    gpio_init(OS2_PIN, GPIO_OUT);

    gpio_init(RST_PIN, GPIO_OUT);
    gpio_init(RANGE_PIN, GPIO_OUT);
    gpio_init(CNAB_PIN, GPIO_OUT);

    gpio_set(RST_PIN);

    gpio_init_int(BUSY_PIN, GPIO_IN_PU, GPIO_FALLING, AD7606_EXIT4_IRQHandler, NULL);

    gpio_set(RANGE_PIN); //量程范围选择。0表示正负5V, 1表示正负10V.
//    gpio_clear(RANGE_PIN);
/*
 * 000表示无过采样，最大200Ksps采样速率。
 * 001表示2倍过采样， 也就是硬件内部采集2个样本求平均
 * 010表示4倍过采样， 也就是硬件内部采集4个样本求平均
 * 011表示8倍过采样， 也就是硬件内部采集8个样本求平均
 * 100表示16倍过采样， 也就是硬件内部采集16个样本求平均
 * 101表示32倍过采样， 也就是硬件内部采集32个样本求平均
 * 110表示64倍过采样， 也就是硬件内部采集64个样本求平均
 */
    gpio_clear(OS0_PIN);
    gpio_clear(OS1_PIN);
    gpio_clear(OS2_PIN);

//    gpio_set(CNAB_PIN);
    gpio_clear(RST_PIN);//reset AD7606
}

void AD7606_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef    FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  FSMC_NORSRAMTimingInitStructure;

    /*-- FSMC Configuration ------------------------------------------------------*/
    /*----------------------- SRAM Bank 4 ----------------------------------------*/
    /* FSMC_Bank1_NORSRAM4 configuration */
    FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 3;//1;//3;
    FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;
    FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 6;//2;//6;
    FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 1;//0;// 1;
    FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0;
    FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0;
    FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;


    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;//;FSMC_Bank1_NORSRAM2;//FSMC_Bank1_NORSRAM4
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //地址数据不复用
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;//FSMC_MemoryType_SRAM;            //FSMC_MemoryType_SRAM存储器类型为SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;   //存储器数据宽度为16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;// 关闭突发模式访问
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//等待信号优先级，只有在使能突发访问模式才有效
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;//关闭Wrapped burst access mode，只有在使能突发访问模式才有效
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//等待信号设置，只有在使能突发访问模式才有效
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;  //存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;        //使能/关闭等待信息设置，只在使能突发访问模式才有效
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;    // 读写使用相同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;//读写操作时序参数
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;//读写同样时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);//初始化FSMC配置

    /* - BANK 3 (of NOR/SRAM Bank 0~3) is enabled */
//  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
        FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);// 使能BANK3
}


void conv_ab(void)
{
    gpio_clear(CNAB_PIN);
    gpio_set(CNAB_PIN);
}

void AD7606_Init(void)
{
    fsmc_clk_enable();
    fsmc_bus_pins_init();

    AD7606_IO_Init();

    AD7606_FSMCConfig();
}

/*******************************************************************************
 * AD7606_EXIT4_IRQHandler
 * AD7606中断处理函数
 *******************************************************************************/
AD7606Para AD7606 ;//={{0},0,0};
uint8_t conv_over_flag = 0;
void AD7606_EXIT4_IRQHandler(void *arg)
{
    (void)arg;

    gpio_init(GPIO_PIN(PORT_F, 10), GPIO_OUT);
    gpio_clear(GPIO_PIN(PORT_F, 10));//on LED1

//    printf("ad7606 data ready...\r\n");
    AD7606.CHn[0][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[1][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[2][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[3][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[4][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[5][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[6][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.CHn[7][AD7606.W_index] = FSMC_AD7606_RD();
    AD7606.W_index = (AD7606.W_index == (DATA_LEN - 1)) ? 0 : AD7606.W_index + 1;
//    printf("ad7606 read data over...\r\n");
    conv_over_flag = 1;
    return;
}

void print_AD7606(void)
{
    for(uint8_t chn = 0; chn < 8; chn++){
        printf("[AD7606]:channel %d ad data is: %d \r\n", chn, AD7606.CHn[chn][AD7606.W_index-1]);
    }
}

uint16_t FSMC_AD7606_RD(void)
{
    return AD7606_FSMC->AD7606_Read;
}

