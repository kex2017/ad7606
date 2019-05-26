/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_fsmc
 * @{
 *
 * @file
 * @brief       Low-level FSMC driver implementation
 *
 * @author      XuBingcan <xubingcan@klec.com.cn>
 *
 * @}
 */
#include "cpu.h"
#include "board.h"
#include "periph/fsmc.h"
#include "periph_cpu.h"
#include "periph_conf.h"

void fsmc_clk_enable(void)
{
   /* Enable AHB Bus Clock */
   periph_clk_en(AHB, RCC_AHBENR_FSMCEN);

   /* Enable GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
   periph_clk_en(APB2, (RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN | RCC_APB2ENR_IOPFEN | RCC_APB2ENR_IOPGEN | RCC_APB2ENR_AFIOEN));
}

void fsmc_clk_disable(void)
{
   /* Disable AHB Bus Clock */
   periph_clk_dis(AHB, RCC_AHBENR_FSMCEN);

   /* Disable GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
   periph_clk_dis(APB2, (RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN | RCC_APB2ENR_IOPFEN | RCC_APB2ENR_IOPGEN | RCC_APB2ENR_AFIOEN));
}

void fsmc_bank1_bus_pins_init(void)
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
   gpio_init_af(GPIO_PIN(PORT_G, 12), GPIO_AF_OUT_PP);

   /* NBL0, NBL1 configuration*/
   gpio_init_af(GPIO_PIN(PORT_E, 0), GPIO_AF_OUT_PP);
   gpio_init_af(GPIO_PIN(PORT_E, 1), GPIO_AF_OUT_PP);
}



/** @defgroup FSMC_Private_Defines
  * @{
  */

/* --------------------- FSMC registers bit mask ---------------------------- */

/* FSMC BCRx Mask */
#define BCR_MBKEN_Set                       ((uint32_t)0x00000001)
#define BCR_MBKEN_Reset                     ((uint32_t)0x000FFFFE)
#define BCR_FACCEN_Set                      ((uint32_t)0x00000040)

/* FSMC PCRx Mask */
#define PCR_PBKEN_Set                       ((uint32_t)0x00000004)
#define PCR_PBKEN_Reset                     ((uint32_t)0x000FFFFB)
#define PCR_ECCEN_Set                       ((uint32_t)0x00000040)
#define PCR_ECCEN_Reset                     ((uint32_t)0x000FFFBF)
#define PCR_MemoryType_NAND                 ((uint32_t)0x00000008)

/**
  * @}
  */

/**
  * @brief  Initializes the FSMC NOR/SRAM Banks according to the specified
  *         parameters in the FSMC_NORSRAMInitStruct.
  * @param  FSMC_NORSRAMInitStruct : pointer to a FSMC_NORSRAMInitTypeDef
  *         structure that contains the configuration information for
  *        the FSMC NOR/SRAM specified Banks.
  * @retval None
  */
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct)
{
  /* Bank1 NOR/SRAM control register configuration */
  FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] =
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_DataAddressMux |
            FSMC_NORSRAMInitStruct->FSMC_MemoryType |
            FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth |
            FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode |
            FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity |
            FSMC_NORSRAMInitStruct->FSMC_WrapMode |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive |
            FSMC_NORSRAMInitStruct->FSMC_WriteOperation |
            FSMC_NORSRAMInitStruct->FSMC_WaitSignal |
            FSMC_NORSRAMInitStruct->FSMC_ExtendedMode |
            FSMC_NORSRAMInitStruct->FSMC_WriteBurst;

  if(FSMC_NORSRAMInitStruct->FSMC_MemoryType == FSMC_MemoryType_NOR)
  {
    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] |= (uint32_t)BCR_FACCEN_Set;
  }

  /* Bank1 NOR/SRAM timing register configuration */
  FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank+1] =
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime << 4) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime << 8) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration << 16) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision << 20) |
            (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency << 24) |
             FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode;


  /* Bank1 NOR/SRAM timing register for write configuration, if extended mode is used */
  if(FSMC_NORSRAMInitStruct->FSMC_ExtendedMode == FSMC_ExtendedMode_Enable)
  {
    FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] =
              (uint32_t)FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime |
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime << 4 )|
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime << 8) |
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision << 20) |
              (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency << 24) |
               FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode;
  }
  else
  {
    FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 0x0FFFFFFF;
  }
}


/**
  * @brief  Enables or disables the specified NOR/SRAM Memory Bank.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank1_NORSRAM1: FSMC Bank1 NOR/SRAM1
  *     @arg FSMC_Bank1_NORSRAM2: FSMC Bank1 NOR/SRAM2
  *     @arg FSMC_Bank1_NORSRAM3: FSMC Bank1 NOR/SRAM3
  *     @arg FSMC_Bank1_NORSRAM4: FSMC Bank1 NOR/SRAM4
  * @param  NewState: new state of the FSMC_Bank. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected NOR/SRAM Bank by setting the PBKEN bit in the BCRx register */
    FSMC_Bank1->BTCR[FSMC_Bank] |= BCR_MBKEN_Set;
  }
  else
  {
    /* Disable the selected NOR/SRAM Bank by clearing the PBKEN bit in the BCRx register */
    FSMC_Bank1->BTCR[FSMC_Bank] &= BCR_MBKEN_Reset;
  }
}

void fsmc_bank1_region2_sram_config(void)
{
   FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
   FSMC_NORSRAMTimingInitTypeDef p;

   /*-- FSMC Configuration ------------------------------------------------------*/
   p.FSMC_AddressSetupTime = 0;
   p.FSMC_AddressHoldTime = 0;
   p.FSMC_DataSetupTime = 14;
   p.FSMC_BusTurnAroundDuration = 0;
   p.FSMC_CLKDivision = 0;
   p.FSMC_DataLatency = 0;
   p.FSMC_AccessMode = FSMC_AccessMode_A;

   FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
   FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
   FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
   FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
   FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
   FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
   FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
   FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
   FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
   FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
   FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
   FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
   FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
   FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
   FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

   FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

   /*!< Enable FSMC Bank1_SRAM Bank */
   FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);

}

static void fsmc_bank1_region1_fpga_config(void)
{
   FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
   FSMC_NORSRAMTimingInitTypeDef  p;

   /*-- FSMC Configuration ------------------------------------------------------*/
   /*----------------------- SRAM Bank 1 ----------------------------------------*/
   /* FSMC_Bank1_NORSRAM1 configuration */
   p.FSMC_AddressSetupTime = 0;
   p.FSMC_AddressHoldTime = 0;
   p.FSMC_DataSetupTime = 14;    //xiewei
   p.FSMC_BusTurnAroundDuration = 0;
   p.FSMC_CLKDivision = 0;
   p.FSMC_DataLatency = 0;
   p.FSMC_AccessMode = FSMC_AccessMode_A;

   /* Color LCD configuration ------------------------------------
    LCD configured as follow:
       - Data/Address MUX = Disable
       - Memory Type = SRAM
       - Data Width = 16bit
       - Write Operation = Enable
       - Extended Mode = Enable
       - Asynchronous Wait = Disable */
   FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
   FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
   FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
   FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
   FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
   FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
   FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
   FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
   FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
   FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
   FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
   FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
   FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
   FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
   FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

   FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

   /* - BANK 4 (of NOR/SRAM Bank 0~3) is enabled */
   FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

//void fsmc_bank1_region4_dm9000_config(void)
//{
//   FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
//   FSMC_NORSRAMTimingInitTypeDef p;
//
//   /*-- FSMC Configuration ------------------------------------------------------*/
//   p.FSMC_AddressSetupTime = 0;
//   p.FSMC_AddressHoldTime = 0;
//   p.FSMC_DataSetupTime = 4;
//   p.FSMC_BusTurnAroundDuration = 0;
//   p.FSMC_CLKDivision = 0;
//   p.FSMC_DataLatency = 0;
//   p.FSMC_AccessMode = FSMC_AccessMode_A;
//
//   FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
//   FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//   FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//   FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//   FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//   FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//   FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
//   FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
//   FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
//
//   FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
//
//   /*!< Enable FSMC Bank1_SRAM Bank */
//   FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
//
//}

void fsmc_init(void)
{
#ifdef BOARD_KL_STM32_CG
   fsmc_clk_enable();
   fsmc_bank1_bus_pins_init();
   fsmc_bank1_region1_fpga_config();
   fsmc_bank1_region2_sram_config();
#endif
//   fsmc_bank1_region4_dm9000_config();
}
