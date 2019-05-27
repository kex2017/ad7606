#include "periph/stm32f10x_std_periph.h"
#include "periph/gpio.h"
#include <stdio.h>
#include <stdint.h>

#define ENABLE_DEBUG (0)
#include "debug.h"


//STD COMMON
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

#define assert_param(expr) ((void)0)

//STD RCC
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020)
#define RCC_APB1Periph_TIM12             ((uint32_t)0x00000040)
#define RCC_APB1Periph_TIM13             ((uint32_t)0x00000080)
#define RCC_APB1Periph_TIM14             ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000)
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000)
#define RCC_APB1Periph_USB               ((uint32_t)0x00800000)
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000)
#define RCC_APB1Periph_BKP               ((uint32_t)0x08000000)
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000)
#define RCC_APB1Periph_CEC               ((uint32_t)0x40000000)


#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB             ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020)
#define RCC_APB2Periph_GPIOE             ((uint32_t)0x00000040)
#define RCC_APB2Periph_GPIOF             ((uint32_t)0x00000080)
#define RCC_APB2Periph_GPIOG             ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC2              ((uint32_t)0x00000400)
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_TIM8              ((uint32_t)0x00002000)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000)
#define RCC_APB2Periph_ADC3              ((uint32_t)0x00008000)
#define RCC_APB2Periph_TIM15             ((uint32_t)0x00010000)
#define RCC_APB2Periph_TIM16             ((uint32_t)0x00020000)
#define RCC_APB2Periph_TIM17             ((uint32_t)0x00040000)
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00080000)
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00100000)
#define RCC_APB2Periph_TIM11             ((uint32_t)0x00200000)


#define RCC_PCLK2_Div2                   ((uint32_t)0x00000000)
#define RCC_PCLK2_Div4                   ((uint32_t)0x00004000)
#define RCC_PCLK2_Div6                   ((uint32_t)0x00008000)
#define RCC_PCLK2_Div8                   ((uint32_t)0x0000C000)

#define CFGR_PLLMull_Mask         ((uint32_t)0x003C0000)
#define CFGR_PLLSRC_Mask          ((uint32_t)0x00010000)
#define CFGR_PLLXTPRE_Mask        ((uint32_t)0x00020000)
#define CFGR_SWS_Mask             ((uint32_t)0x0000000C)
#define CFGR_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)
#define CFGR_HPRE_Set_Mask        ((uint32_t)0x000000F0)
#define CFGR_PPRE1_Reset_Mask     ((uint32_t)0xFFFFF8FF)
#define CFGR_PPRE1_Set_Mask       ((uint32_t)0x00000700)
#define CFGR_PPRE2_Reset_Mask     ((uint32_t)0xFFFFC7FF)
#define CFGR_PPRE2_Set_Mask       ((uint32_t)0x00003800)
#define CFGR_ADCPRE_Reset_Mask    ((uint32_t)0xFFFF3FFF)
#define CFGR_ADCPRE_Set_Mask      ((uint32_t)0x0000C000)


#define RCC_AHBPeriph_DMA1               ((uint32_t)0x00000001)
#define RCC_AHBPeriph_DMA2               ((uint32_t)0x00000002)
#define RCC_AHBPeriph_SRAM               ((uint32_t)0x00000004)
#define RCC_AHBPeriph_FLITF              ((uint32_t)0x00000010)
#define RCC_AHBPeriph_CRC                ((uint32_t)0x00000040)



void RCC_ADCCLKConfig(uint32_t RCC_PCLK2)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_RCC_ADCCLK(RCC_PCLK2));
  tmpreg = RCC->CFGR;
  /* Clear ADCPRE[1:0] bits */
  tmpreg &= CFGR_ADCPRE_Reset_Mask;
  /* Set ADCPRE[1:0] bits according to RCC_PCLK2 value */
  tmpreg |= RCC_PCLK2;
  /* Store the new value */
  RCC->CFGR = tmpreg;
}


//STD ADC
typedef struct
{
  uint32_t ADC_Mode;                      /*!< Configures the ADC to operate in independent or
                                               dual mode.
                                               This parameter can be a value of @ref ADC_mode */

  FunctionalState ADC_ScanConvMode;       /*!< Specifies whether the conversion is performed in
                                               Scan (multichannels) or Single (one channel) mode.
                                               This parameter can be set to ENABLE or DISABLE */

  FunctionalState ADC_ContinuousConvMode; /*!< Specifies whether the conversion is performed in
                                               Continuous or Single mode.
                                               This parameter can be set to ENABLE or DISABLE. */

  uint32_t ADC_ExternalTrigConv;          /*!< Defines the external trigger used to start the analog
                                               to digital conversion of regular channels. This parameter
                                               can be a value of @ref ADC_external_trigger_sources_for_regular_channels_conversion */

  uint32_t ADC_DataAlign;                 /*!< Specifies whether the ADC data alignment is left or right.
                                               This parameter can be a value of @ref ADC_data_align */

  uint8_t ADC_NbrOfChannel;               /*!< Specifies the number of ADC channels that will be converted
                                               using the sequencer for regular channel group.
                                               This parameter must range from 1 to 16. */
}STD_ADC_InitTypeDef;

#define IS_ADC_ALL_PERIPH(PERIPH) (((PERIPH) == ADC1) || \
                                   ((PERIPH) == ADC2) || \
                                   ((PERIPH) == ADC3))

#define IS_ADC_DMA_PERIPH(PERIPH) (((PERIPH) == ADC1) || \
                                   ((PERIPH) == ADC3))

/** @defgroup ADC_mode
  * @{
  */

#define ADC_Mode_Independent                       ((uint32_t)0x00000000)
#define ADC_Mode_RegInjecSimult                    ((uint32_t)0x00010000)
#define ADC_Mode_RegSimult_AlterTrig               ((uint32_t)0x00020000)
#define ADC_Mode_InjecSimult_FastInterl            ((uint32_t)0x00030000)
#define ADC_Mode_InjecSimult_SlowInterl            ((uint32_t)0x00040000)
#define ADC_Mode_InjecSimult                       ((uint32_t)0x00050000)
#define ADC_Mode_RegSimult                         ((uint32_t)0x00060000)
#define ADC_Mode_FastInterl                        ((uint32_t)0x00070000)
#define ADC_Mode_SlowInterl                        ((uint32_t)0x00080000)
#define ADC_Mode_AlterTrig                         ((uint32_t)0x00090000)

#define IS_ADC_MODE(MODE) (((MODE) == ADC_Mode_Independent) || \
                           ((MODE) == ADC_Mode_RegInjecSimult) || \
                           ((MODE) == ADC_Mode_RegSimult_AlterTrig) || \
                           ((MODE) == ADC_Mode_InjecSimult_FastInterl) || \
                           ((MODE) == ADC_Mode_InjecSimult_SlowInterl) || \
                           ((MODE) == ADC_Mode_InjecSimult) || \
                           ((MODE) == ADC_Mode_RegSimult) || \
                           ((MODE) == ADC_Mode_FastInterl) || \
                           ((MODE) == ADC_Mode_SlowInterl) || \
                           ((MODE) == ADC_Mode_AlterTrig))
/**
  * @}
  */

/** @defgroup ADC_external_trigger_sources_for_regular_channels_conversion
  * @{
  */

#define ADC_ExternalTrigConv_T1_CC1                ((uint32_t)0x00000000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigConv_T1_CC2                ((uint32_t)0x00020000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigConv_T2_CC2                ((uint32_t)0x00060000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigConv_T3_TRGO               ((uint32_t)0x00080000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigConv_T4_CC4                ((uint32_t)0x000A0000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO    ((uint32_t)0x000C0000) /*!< For ADC1 and ADC2 */

#define ADC_ExternalTrigConv_T1_CC3                ((uint32_t)0x00040000) /*!< For ADC1, ADC2 and ADC3 */
#define ADC_ExternalTrigConv_None                  ((uint32_t)0x000E0000) /*!< For ADC1, ADC2 and ADC3 */

#define ADC_ExternalTrigConv_T3_CC1                ((uint32_t)0x00000000) /*!< For ADC3 only */
#define ADC_ExternalTrigConv_T2_CC3                ((uint32_t)0x00020000) /*!< For ADC3 only */
#define ADC_ExternalTrigConv_T8_CC1                ((uint32_t)0x00060000) /*!< For ADC3 only */
#define ADC_ExternalTrigConv_T8_TRGO               ((uint32_t)0x00080000) /*!< For ADC3 only */
#define ADC_ExternalTrigConv_T5_CC1                ((uint32_t)0x000A0000) /*!< For ADC3 only */
#define ADC_ExternalTrigConv_T5_CC3                ((uint32_t)0x000C0000) /*!< For ADC3 only */

#define IS_ADC_EXT_TRIG(REGTRIG) (((REGTRIG) == ADC_ExternalTrigConv_T1_CC1) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T1_CC2) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T1_CC3) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T2_CC2) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T3_TRGO) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T4_CC4) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_None) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T3_CC1) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T2_CC3) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T8_CC1) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T8_TRGO) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T5_CC1) || \
                                  ((REGTRIG) == ADC_ExternalTrigConv_T5_CC3))
/**
  * @}
  */

/** @defgroup ADC_data_align
  * @{
  */

#define ADC_DataAlign_Right                        ((uint32_t)0x00000000)
#define ADC_DataAlign_Left                         ((uint32_t)0x00000800)
#define IS_ADC_DATA_ALIGN(ALIGN) (((ALIGN) == ADC_DataAlign_Right) || \
                                  ((ALIGN) == ADC_DataAlign_Left))
/**
  * @}
  */

/** @defgroup ADC_channels
  * @{
  */

#define ADC_Channel_0                               ((uint8_t)0x00)
#define ADC_Channel_1                               ((uint8_t)0x01)
#define ADC_Channel_2                               ((uint8_t)0x02)
#define ADC_Channel_3                               ((uint8_t)0x03)
#define ADC_Channel_4                               ((uint8_t)0x04)
#define ADC_Channel_5                               ((uint8_t)0x05)
#define ADC_Channel_6                               ((uint8_t)0x06)
#define ADC_Channel_7                               ((uint8_t)0x07)
#define ADC_Channel_8                               ((uint8_t)0x08)
#define ADC_Channel_9                               ((uint8_t)0x09)
#define ADC_Channel_10                              ((uint8_t)0x0A)
#define ADC_Channel_11                              ((uint8_t)0x0B)
#define ADC_Channel_12                              ((uint8_t)0x0C)
#define ADC_Channel_13                              ((uint8_t)0x0D)
#define ADC_Channel_14                              ((uint8_t)0x0E)
#define ADC_Channel_15                              ((uint8_t)0x0F)
#define ADC_Channel_16                              ((uint8_t)0x10)
#define ADC_Channel_17                              ((uint8_t)0x11)

#define ADC_Channel_TempSensor                      ((uint8_t)ADC_Channel_16)
#define ADC_Channel_Vrefint                         ((uint8_t)ADC_Channel_17)

#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) == ADC_Channel_0) || ((CHANNEL) == ADC_Channel_1) || \
                                 ((CHANNEL) == ADC_Channel_2) || ((CHANNEL) == ADC_Channel_3) || \
                                 ((CHANNEL) == ADC_Channel_4) || ((CHANNEL) == ADC_Channel_5) || \
                                 ((CHANNEL) == ADC_Channel_6) || ((CHANNEL) == ADC_Channel_7) || \
                                 ((CHANNEL) == ADC_Channel_8) || ((CHANNEL) == ADC_Channel_9) || \
                                 ((CHANNEL) == ADC_Channel_10) || ((CHANNEL) == ADC_Channel_11) || \
                                 ((CHANNEL) == ADC_Channel_12) || ((CHANNEL) == ADC_Channel_13) || \
                                 ((CHANNEL) == ADC_Channel_14) || ((CHANNEL) == ADC_Channel_15) || \
                                 ((CHANNEL) == ADC_Channel_16) || ((CHANNEL) == ADC_Channel_17))
/**
  * @}
  */

/** @defgroup ADC_sampling_time
  * @{
  */

#define ADC_SampleTime_1Cycles5                    ((uint8_t)0x00)
#define ADC_SampleTime_7Cycles5                    ((uint8_t)0x01)
#define ADC_SampleTime_13Cycles5                   ((uint8_t)0x02)
#define ADC_SampleTime_28Cycles5                   ((uint8_t)0x03)
#define ADC_SampleTime_41Cycles5                   ((uint8_t)0x04)
#define ADC_SampleTime_55Cycles5                   ((uint8_t)0x05)
#define ADC_SampleTime_71Cycles5                   ((uint8_t)0x06)
#define ADC_SampleTime_239Cycles5                  ((uint8_t)0x07)
#define IS_ADC_SAMPLE_TIME(TIME) (((TIME) == ADC_SampleTime_1Cycles5) || \
                                  ((TIME) == ADC_SampleTime_7Cycles5) || \
                                  ((TIME) == ADC_SampleTime_13Cycles5) || \
                                  ((TIME) == ADC_SampleTime_28Cycles5) || \
                                  ((TIME) == ADC_SampleTime_41Cycles5) || \
                                  ((TIME) == ADC_SampleTime_55Cycles5) || \
                                  ((TIME) == ADC_SampleTime_71Cycles5) || \
                                  ((TIME) == ADC_SampleTime_239Cycles5))
/**
  * @}
  */

/** @defgroup ADC_external_trigger_sources_for_injected_channels_conversion
  * @{
  */

#define ADC_ExternalTrigInjecConv_T2_TRGO           ((uint32_t)0x00002000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_T2_CC1            ((uint32_t)0x00003000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_T3_CC4            ((uint32_t)0x00004000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_T4_TRGO           ((uint32_t)0x00005000) /*!< For ADC1 and ADC2 */
#define ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4 ((uint32_t)0x00006000) /*!< For ADC1 and ADC2 */

#define ADC_ExternalTrigInjecConv_T1_TRGO           ((uint32_t)0x00000000) /*!< For ADC1, ADC2 and ADC3 */
#define ADC_ExternalTrigInjecConv_T1_CC4            ((uint32_t)0x00001000) /*!< For ADC1, ADC2 and ADC3 */
#define ADC_ExternalTrigInjecConv_None              ((uint32_t)0x00007000) /*!< For ADC1, ADC2 and ADC3 */

#define ADC_ExternalTrigInjecConv_T4_CC3            ((uint32_t)0x00002000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T8_CC2            ((uint32_t)0x00003000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T8_CC4            ((uint32_t)0x00004000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T5_TRGO           ((uint32_t)0x00005000) /*!< For ADC3 only */
#define ADC_ExternalTrigInjecConv_T5_CC4            ((uint32_t)0x00006000) /*!< For ADC3 only */

#define IS_ADC_EXT_INJEC_TRIG(INJTRIG) (((INJTRIG) == ADC_ExternalTrigInjecConv_T1_TRGO) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T1_CC4) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T2_TRGO) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T2_CC1) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T3_CC4) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T4_TRGO) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_None) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T4_CC3) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T8_CC2) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T8_CC4) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T5_TRGO) || \
                                        ((INJTRIG) == ADC_ExternalTrigInjecConv_T5_CC4))
/**
  * @}
  */

/** @defgroup ADC_injected_channel_selection
  * @{
  */

#define ADC_InjectedChannel_1                       ((uint8_t)0x14)
#define ADC_InjectedChannel_2                       ((uint8_t)0x18)
#define ADC_InjectedChannel_3                       ((uint8_t)0x1C)
#define ADC_InjectedChannel_4                       ((uint8_t)0x20)
#define IS_ADC_INJECTED_CHANNEL(CHANNEL) (((CHANNEL) == ADC_InjectedChannel_1) || \
                                          ((CHANNEL) == ADC_InjectedChannel_2) || \
                                          ((CHANNEL) == ADC_InjectedChannel_3) || \
                                          ((CHANNEL) == ADC_InjectedChannel_4))
/**
  * @}
  */

/** @defgroup ADC_analog_watchdog_selection
  * @{
  */

#define ADC_AnalogWatchdog_SingleRegEnable         ((uint32_t)0x00800200)
#define ADC_AnalogWatchdog_SingleInjecEnable       ((uint32_t)0x00400200)
#define ADC_AnalogWatchdog_SingleRegOrInjecEnable  ((uint32_t)0x00C00200)
#define ADC_AnalogWatchdog_AllRegEnable            ((uint32_t)0x00800000)
#define ADC_AnalogWatchdog_AllInjecEnable          ((uint32_t)0x00400000)
#define ADC_AnalogWatchdog_AllRegAllInjecEnable    ((uint32_t)0x00C00000)
#define ADC_AnalogWatchdog_None                    ((uint32_t)0x00000000)

#define IS_ADC_ANALOG_WATCHDOG(WATCHDOG) (((WATCHDOG) == ADC_AnalogWatchdog_SingleRegEnable) || \
                                          ((WATCHDOG) == ADC_AnalogWatchdog_SingleInjecEnable) || \
                                          ((WATCHDOG) == ADC_AnalogWatchdog_SingleRegOrInjecEnable) || \
                                          ((WATCHDOG) == ADC_AnalogWatchdog_AllRegEnable) || \
                                          ((WATCHDOG) == ADC_AnalogWatchdog_AllInjecEnable) || \
                                          ((WATCHDOG) == ADC_AnalogWatchdog_AllRegAllInjecEnable) || \
                                          ((WATCHDOG) == ADC_AnalogWatchdog_None))
/**
  * @}
  */

/** @defgroup ADC_interrupts_definition
  * @{
  */

#define ADC_IT_EOC                                 ((uint16_t)0x0220)
#define ADC_IT_AWD                                 ((uint16_t)0x0140)
#define ADC_IT_JEOC                                ((uint16_t)0x0480)

#define IS_ADC_IT(IT) ((((IT) & (uint16_t)0xF81F) == 0x00) && ((IT) != 0x00))

#define IS_ADC_GET_IT(IT) (((IT) == ADC_IT_EOC) || ((IT) == ADC_IT_AWD) || \
                           ((IT) == ADC_IT_JEOC))
/**
  * @}
  */

/** @defgroup ADC_flags_definition
  * @{
  */

#define ADC_FLAG_AWD                               ((uint8_t)0x01)
#define ADC_FLAG_EOC                               ((uint8_t)0x02)
#define ADC_FLAG_JEOC                              ((uint8_t)0x04)
#define ADC_FLAG_JSTRT                             ((uint8_t)0x08)
#define ADC_FLAG_STRT                              ((uint8_t)0x10)
#define IS_ADC_CLEAR_FLAG(FLAG) ((((FLAG) & (uint8_t)0xE0) == 0x00) && ((FLAG) != 0x00))
#define IS_ADC_GET_FLAG(FLAG) (((FLAG) == ADC_FLAG_AWD) || ((FLAG) == ADC_FLAG_EOC) || \
                               ((FLAG) == ADC_FLAG_JEOC) || ((FLAG)== ADC_FLAG_JSTRT) || \
                               ((FLAG) == ADC_FLAG_STRT))
/**
  * @}
  */

/** @defgroup ADC_thresholds
  * @{
  */

#define IS_ADC_THRESHOLD(THRESHOLD) ((THRESHOLD) <= 0xFFF)

/**
  * @}
  */

/** @defgroup ADC_injected_offset
  * @{
  */

#define IS_ADC_OFFSET(OFFSET) ((OFFSET) <= 0xFFF)

/**
  * @}
  */

/** @defgroup ADC_injected_length
  * @{
  */

#define IS_ADC_INJECTED_LENGTH(LENGTH) (((LENGTH) >= 0x1) && ((LENGTH) <= 0x4))

/**
  * @}
  */

/** @defgroup ADC_injected_rank
  * @{
  */

#define IS_ADC_INJECTED_RANK(RANK) (((RANK) >= 0x1) && ((RANK) <= 0x4))

/**
  * @}
  */


/** @defgroup ADC_regular_length
  * @{
  */

#define IS_ADC_REGULAR_LENGTH(LENGTH) (((LENGTH) >= 0x1) && ((LENGTH) <= 0x10))
/**
  * @}
  */

/** @defgroup ADC_regular_rank
  * @{
  */

#define IS_ADC_REGULAR_RANK(RANK) (((RANK) >= 0x1) && ((RANK) <= 0x10))

/**
  * @}
  */

/** @defgroup ADC_regular_discontinuous_mode_number
  * @{
  */

#define IS_ADC_REGULAR_DISC_NUMBER(NUMBER) (((NUMBER) >= 0x1) && ((NUMBER) <= 0x8))


/* ADC DISCNUM mask */
#define CR1_DISCNUM_Reset           ((uint32_t)0xFFFF1FFF)

/* ADC DISCEN mask */
#define CR1_DISCEN_Set              ((uint32_t)0x00000800)
#define CR1_DISCEN_Reset            ((uint32_t)0xFFFFF7FF)

/* ADC JAUTO mask */
#define CR1_JAUTO_Set               ((uint32_t)0x00000400)
#define CR1_JAUTO_Reset             ((uint32_t)0xFFFFFBFF)

/* ADC JDISCEN mask */
#define CR1_JDISCEN_Set             ((uint32_t)0x00001000)
#define CR1_JDISCEN_Reset           ((uint32_t)0xFFFFEFFF)

/* ADC AWDCH mask */
#define CR1_AWDCH_Reset             ((uint32_t)0xFFFFFFE0)

/* ADC Analog watchdog enable mode mask */
#define CR1_AWDMode_Reset           ((uint32_t)0xFF3FFDFF)

/* CR1 register Mask */
#define CR1_CLEAR_Mask              ((uint32_t)0xFFF0FEFF)

/* ADC ADON mask */
#define CR2_ADON_Set                ((uint32_t)0x00000001)
#define CR2_ADON_Reset              ((uint32_t)0xFFFFFFFE)

/* ADC DMA mask */
#define CR2_DMA_Set                 ((uint32_t)0x00000100)
#define CR2_DMA_Reset               ((uint32_t)0xFFFFFEFF)

/* ADC RSTCAL mask */
#define CR2_RSTCAL_Set              ((uint32_t)0x00000008)

/* ADC CAL mask */
#define CR2_CAL_Set                 ((uint32_t)0x00000004)

/* ADC SWSTART mask */
#define CR2_SWSTART_Set             ((uint32_t)0x00400000)

/* ADC EXTTRIG mask */
#define CR2_EXTTRIG_Set             ((uint32_t)0x00100000)
#define CR2_EXTTRIG_Reset           ((uint32_t)0xFFEFFFFF)

/* ADC Software start mask */
#define CR2_EXTTRIG_SWSTART_Set     ((uint32_t)0x00500000)
#define CR2_EXTTRIG_SWSTART_Reset   ((uint32_t)0xFFAFFFFF)

/* ADC JEXTSEL mask */
#define CR2_JEXTSEL_Reset           ((uint32_t)0xFFFF8FFF)

/* ADC JEXTTRIG mask */
#define CR2_JEXTTRIG_Set            ((uint32_t)0x00008000)
#define CR2_JEXTTRIG_Reset          ((uint32_t)0xFFFF7FFF)

/* ADC JSWSTART mask */
#define CR2_JSWSTART_Set            ((uint32_t)0x00200000)

/* ADC injected software start mask */
#define CR2_JEXTTRIG_JSWSTART_Set   ((uint32_t)0x00208000)
#define CR2_JEXTTRIG_JSWSTART_Reset ((uint32_t)0xFFDF7FFF)

/* ADC TSPD mask */
#define CR2_TSVREFE_Set             ((uint32_t)0x00800000)
#define CR2_TSVREFE_Reset           ((uint32_t)0xFF7FFFFF)

/* CR2 register Mask */
#define CR2_CLEAR_Mask              ((uint32_t)0xFFF1F7FD)

/* ADC SQx mask */
#define SQR3_SQ_Set                 ((uint32_t)0x0000001F)
#define SQR2_SQ_Set                 ((uint32_t)0x0000001F)
#define SQR1_SQ_Set                 ((uint32_t)0x0000001F)

/* SQR1 register Mask */
#define SQR1_CLEAR_Mask             ((uint32_t)0xFF0FFFFF)

/* ADC JSQx mask */
#define JSQR_JSQ_Set                ((uint32_t)0x0000001F)

/* ADC JL mask */
#define JSQR_JL_Set                 ((uint32_t)0x00300000)
#define JSQR_JL_Reset               ((uint32_t)0xFFCFFFFF)

/* ADC SMPx mask */
#define SMPR1_SMP_Set               ((uint32_t)0x00000007)
#define SMPR2_SMP_Set               ((uint32_t)0x00000007)

/* ADC JDRx registers offset */
#define JDR_Offset                  ((uint8_t)0x28)

/* ADC1 DR register base address */
#define DR_ADDRESS                  ((uint32_t)0x4001244C)


/**
  * @brief  Deinitializes the ADCx peripheral registers to their default reset values.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef* ADCx)
{
	assert_param(IS_ADC_ALL_PERIPH(ADCx));

	if (ADCx == ADC1)
	{
	/* Enable ADC1 reset state */
	  periph_clk_en(APB2,RCC_APB2Periph_ADC1);
	/* Release ADC1 from reset state */
	  periph_clk_dis(APB2,RCC_APB2Periph_ADC1);
	}
	else if (ADCx == ADC2)
	{
	/* Enable ADC2 reset state */
	  periph_clk_en(APB2,RCC_APB2Periph_ADC2);
	/* Release ADC2 from reset state */
	  periph_clk_dis(APB2,RCC_APB2Periph_ADC2);
	}
	else
	{
		if (ADCx == ADC3)
		{
		  /* Enable ADC3 reset state */
			periph_clk_en(APB2,RCC_APB2Periph_ADC3);
		  /* Release ADC3 from reset state */
			periph_clk_dis(APB2,RCC_APB2Periph_ADC3);
		}
	}
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure that contains
  *         the configuration information for the specified ADC peripheral.
  * @retval None
  */
void ADC_Init(ADC_TypeDef* ADCx, STD_ADC_InitTypeDef* ADC_InitStruct)
{
  uint32_t tmpreg1 = 0;
  uint8_t tmpreg2 = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_MODE(ADC_InitStruct->ADC_Mode));
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ScanConvMode));
  assert_param(IS_FUNCTIONAL_STATE(ADC_InitStruct->ADC_ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG(ADC_InitStruct->ADC_ExternalTrigConv));
  assert_param(IS_ADC_DATA_ALIGN(ADC_InitStruct->ADC_DataAlign));
  assert_param(IS_ADC_REGULAR_LENGTH(ADC_InitStruct->ADC_NbrOfChannel));

  /*---------------------------- ADCx CR1 Configuration -----------------*/
  /* Get the ADCx CR1 value */
  tmpreg1 = ADCx->CR1;
  /* Clear DUALMOD and SCAN bits */
  tmpreg1 &= CR1_CLEAR_Mask;
  /* Configure ADCx: Dual mode and scan conversion mode */
  /* Set DUALMOD bits according to ADC_Mode value */
  /* Set SCAN bit according to ADC_ScanConvMode value */
  tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_Mode | ((uint32_t)ADC_InitStruct->ADC_ScanConvMode << 8));
  /* Write to ADCx CR1 */
  ADCx->CR1 = tmpreg1;

  /*---------------------------- ADCx CR2 Configuration -----------------*/
  /* Get the ADCx CR2 value */
  tmpreg1 = ADCx->CR2;
  /* Clear CONT, ALIGN and EXTSEL bits */
  tmpreg1 &= CR2_CLEAR_Mask;
  /* Configure ADCx: external trigger event and continuous conversion mode */
  /* Set ALIGN bit according to ADC_DataAlign value */
  /* Set EXTSEL bits according to ADC_ExternalTrigConv value */
  /* Set CONT bit according to ADC_ContinuousConvMode value */
  tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_DataAlign | ADC_InitStruct->ADC_ExternalTrigConv |
            ((uint32_t)ADC_InitStruct->ADC_ContinuousConvMode << 1));
  /* Write to ADCx CR2 */
  ADCx->CR2 = tmpreg1;

  /*---------------------------- ADCx SQR1 Configuration -----------------*/
  /* Get the ADCx SQR1 value */
  tmpreg1 = ADCx->SQR1;
  /* Clear L bits */
  tmpreg1 &= SQR1_CLEAR_Mask;
  /* Configure ADCx: regular channel sequence length */
  /* Set L bits according to ADC_NbrOfChannel value */
  tmpreg2 |= (uint8_t) (ADC_InitStruct->ADC_NbrOfChannel - (uint8_t)1);
  tmpreg1 |= (uint32_t)tmpreg2 << 20;
  /* Write to ADCx SQR1 */
  ADCx->SQR1 = tmpreg1;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct : pointer to an ADC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void ADC_StructInit(STD_ADC_InitTypeDef* ADC_InitStruct)
{
  /* Reset ADC init structure parameters values */
  /* Initialize the ADC_Mode member */
  ADC_InitStruct->ADC_Mode = ADC_Mode_Independent;
  /* initialize the ADC_ScanConvMode member */
  ADC_InitStruct->ADC_ScanConvMode = DISABLE;
  /* Initialize the ADC_ContinuousConvMode member */
  ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;
  /* Initialize the ADC_ExternalTrigConv member */
  ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  /* Initialize the ADC_DataAlign member */
  ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;
  /* Initialize the ADC_NbrOfChannel member */
  ADC_InitStruct->ADC_NbrOfChannel = 1;
}

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the ADCx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the ADON bit to wake up the ADC from power down mode */
    ADCx->CR2 |= CR2_ADON_Set;
  }
  else
  {
    /* Disable the selected ADC peripheral */
    ADCx->CR2 &= CR2_ADON_Reset;
  }
}

/**
  * @brief  Enables or disables the specified ADC DMA request.
  * @param  ADCx: where x can be 1 or 3 to select the ADC peripheral.
  *   Note: ADC2 hasn't a DMA capability.
  * @param  NewState: new state of the selected ADC DMA transfer.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_DMA_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC DMA request */
    ADCx->CR2 |= CR2_DMA_Set;
  }
  else
  {
    /* Disable the selected ADC DMA request */
    ADCx->CR2 &= CR2_DMA_Reset;
  }
}

/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_IT_EOC: End of conversion interrupt mask
  *     @arg ADC_IT_AWD: Analog watchdog interrupt mask
  *     @arg ADC_IT_JEOC: End of injected conversion interrupt mask
  * @param  NewState: new state of the specified ADC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState)
{
  uint8_t itmask = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_ADC_IT(ADC_IT));
  /* Get the ADC IT index */
  itmask = (uint8_t)ADC_IT;
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC interrupts */
    ADCx->CR1 |= itmask;
  }
  else
  {
    /* Disable the selected ADC interrupts */
    ADCx->CR1 &= (~(uint32_t)itmask);
  }
}

/**
  * @brief  Resets the selected ADC calibration registers.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval None
  */
void ADC_ResetCalibration(ADC_TypeDef* ADCx)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Resets the selected ADC calibration registers */
  ADCx->CR2 |= CR2_RSTCAL_Set;
}

/**
  * @brief  Gets the selected ADC reset calibration registers status.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The new state of ADC reset calibration registers (SET or RESET).
  */
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Check the status of RSTCAL bit */
  if ((ADCx->CR2 & CR2_RSTCAL_Set) != (uint32_t)RESET)
  {
    /* RSTCAL bit is set */
    bitstatus = SET;
  }
  else
  {
    /* RSTCAL bit is reset */
    bitstatus = RESET;
  }
  /* Return the RSTCAL bit status */
  return  bitstatus;
}

/**
  * @brief  Starts the selected ADC calibration process.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval None
  */
void ADC_StartCalibration(ADC_TypeDef* ADCx)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Enable the selected ADC calibration process */
  ADCx->CR2 |= CR2_CAL_Set;
}

/**
  * @brief  Gets the selected ADC calibration status.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The new state of ADC calibration (SET or RESET).
  */
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Check the status of CAL bit */
  if ((ADCx->CR2 & CR2_CAL_Set) != (uint32_t)RESET)
  {
    /* CAL bit is set: calibration on going */
    bitstatus = SET;
  }
  else
  {
    /* CAL bit is reset: end of calibration */
    bitstatus = RESET;
  }
  /* Return the CAL bit status */
  return  bitstatus;
}

/**
  * @brief  Enables or disables the selected ADC software start conversion .
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC software start conversion.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC conversion on external event and start the selected
       ADC conversion */
    ADCx->CR2 |= CR2_EXTTRIG_SWSTART_Set;
  }
  else
  {
    /* Disable the selected ADC conversion on external event and stop the selected
       ADC conversion */
    ADCx->CR2 &= CR2_EXTTRIG_SWSTART_Reset;
  }
}

/**
  * @brief  Gets the selected ADC Software start conversion Status.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The new state of ADC software start conversion (SET or RESET).
  */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Check the status of SWSTART bit */
  if ((ADCx->CR2 & CR2_SWSTART_Set) != (uint32_t)RESET)
  {
    /* SWSTART bit is set */
    bitstatus = SET;
  }
  else
  {
    /* SWSTART bit is reset */
    bitstatus = RESET;
  }
  /* Return the SWSTART bit status */
  return  bitstatus;
}

/**
  * @brief  Configures the discontinuous mode for the selected ADC regular
  *         group channel.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  Number: specifies the discontinuous mode regular channel
  *         count value. This number must be between 1 and 8.
  * @retval None
  */
void ADC_DiscModeChannelCountConfig(ADC_TypeDef* ADCx, uint8_t Number)
{
  uint32_t tmpreg1 = 0;
  uint32_t tmpreg2 = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_REGULAR_DISC_NUMBER(Number));
  /* Get the old register value */
  tmpreg1 = ADCx->CR1;
  /* Clear the old discontinuous mode channel count */
  tmpreg1 &= CR1_DISCNUM_Reset;
  /* Set the discontinuous mode channel count */
  tmpreg2 = Number - 1;
  tmpreg1 |= tmpreg2 << 13;
  /* Store the new register value */
  ADCx->CR1 = tmpreg1;
}

/**
  * @brief  Enables or disables the discontinuous mode on regular group
  *         channel for the specified ADC
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC discontinuous mode
  *         on regular group channel.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC regular discontinuous mode */
    ADCx->CR1 |= CR1_DISCEN_Set;
  }
  else
  {
    /* Disable the selected ADC regular discontinuous mode */
    ADCx->CR1 &= CR1_DISCEN_Reset;
  }
}

/**
  * @brief  Configures for the selected ADC regular channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure.
  *   This parameter can be one of the following values:
  *     @arg ADC_Channel_0: ADC Channel0 selected
  *     @arg ADC_Channel_1: ADC Channel1 selected
  *     @arg ADC_Channel_2: ADC Channel2 selected
  *     @arg ADC_Channel_3: ADC Channel3 selected
  *     @arg ADC_Channel_4: ADC Channel4 selected
  *     @arg ADC_Channel_5: ADC Channel5 selected
  *     @arg ADC_Channel_6: ADC Channel6 selected
  *     @arg ADC_Channel_7: ADC Channel7 selected
  *     @arg ADC_Channel_8: ADC Channel8 selected
  *     @arg ADC_Channel_9: ADC Channel9 selected
  *     @arg ADC_Channel_10: ADC Channel10 selected
  *     @arg ADC_Channel_11: ADC Channel11 selected
  *     @arg ADC_Channel_12: ADC Channel12 selected
  *     @arg ADC_Channel_13: ADC Channel13 selected
  *     @arg ADC_Channel_14: ADC Channel14 selected
  *     @arg ADC_Channel_15: ADC Channel15 selected
  *     @arg ADC_Channel_16: ADC Channel16 selected
  *     @arg ADC_Channel_17: ADC Channel17 selected
  * @param  Rank: The rank in the regular group sequencer. This parameter must be between 1 to 16.
  * @param  ADC_SampleTime: The sample time value to be set for the selected channel.
  *   This parameter can be one of the following values:
  *     @arg ADC_SampleTime_1Cycles5: Sample time equal to 1.5 cycles
  *     @arg ADC_SampleTime_7Cycles5: Sample time equal to 7.5 cycles
  *     @arg ADC_SampleTime_13Cycles5: Sample time equal to 13.5 cycles
  *     @arg ADC_SampleTime_28Cycles5: Sample time equal to 28.5 cycles
  *     @arg ADC_SampleTime_41Cycles5: Sample time equal to 41.5 cycles
  *     @arg ADC_SampleTime_55Cycles5: Sample time equal to 55.5 cycles
  *     @arg ADC_SampleTime_71Cycles5: Sample time equal to 71.5 cycles
  *     @arg ADC_SampleTime_239Cycles5: Sample time equal to 239.5 cycles
  * @retval None
  */
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_CHANNEL(ADC_Channel));
  assert_param(IS_ADC_REGULAR_RANK(Rank));
  assert_param(IS_ADC_SAMPLE_TIME(ADC_SampleTime));
  /* if ADC_Channel_10 ... ADC_Channel_17 is selected */
  if (ADC_Channel > ADC_Channel_9)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR1;
    /* Calculate the mask to clear */
    tmpreg2 = SMPR1_SMP_Set << (3 * (ADC_Channel - 10));
    /* Clear the old channel sample time */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
    /* Set the new channel sample time */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SMPR1 = tmpreg1;
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR2;
    /* Calculate the mask to clear */
    tmpreg2 = SMPR2_SMP_Set << (3 * ADC_Channel);
    /* Clear the old channel sample time */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
    /* Set the new channel sample time */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SMPR2 = tmpreg1;
  }
  /* For Rank 1 to 6 */
  if (Rank < 7)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR3;
    /* Calculate the mask to clear */
    tmpreg2 = SQR3_SQ_Set << (5 * (Rank - 1));
    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));
    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SQR3 = tmpreg1;
  }
  /* For Rank 7 to 12 */
  else if (Rank < 13)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR2;
    /* Calculate the mask to clear */
    tmpreg2 = SQR2_SQ_Set << (5 * (Rank - 7));
    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SQR2 = tmpreg1;
  }
  /* For Rank 13 to 16 */
  else
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR1;
    /* Calculate the mask to clear */
    tmpreg2 = SQR1_SQ_Set << (5 * (Rank - 13));
    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SQR1 = tmpreg1;
  }
}

/**
  * @brief  Enables or disables the ADCx conversion through external trigger.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC external trigger start of conversion.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_ExternalTrigConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC conversion on external event */
    ADCx->CR2 |= CR2_EXTTRIG_Set;
  }
  else
  {
    /* Disable the selected ADC conversion on external event */
    ADCx->CR2 &= CR2_EXTTRIG_Reset;
  }
}

/**
  * @brief  Returns the last ADCx conversion result data for regular channel.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The Data conversion value.
  */
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Return the selected ADC conversion value */
  return (uint16_t) ADCx->DR;
}

/**
  * @brief  Returns the last ADC1 and ADC2 conversion result data in dual mode.
  * @retval The Data conversion value.
  */
uint32_t ADC_GetDualModeConversionValue(void)
{
  /* Return the dual mode conversion value */
  return (*(__IO uint32_t *) DR_ADDRESS);
}

/**
  * @brief  Enables or disables the selected ADC automatic injected group
  *         conversion after regular one.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC auto injected conversion
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_AutoInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC automatic injected group conversion */
    ADCx->CR1 |= CR1_JAUTO_Set;
  }
  else
  {
    /* Disable the selected ADC automatic injected group conversion */
    ADCx->CR1 &= CR1_JAUTO_Reset;
  }
}

/**
  * @brief  Enables or disables the discontinuous mode for injected group
  *         channel for the specified ADC
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC discontinuous mode
  *         on injected group channel.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_InjectedDiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC injected discontinuous mode */
    ADCx->CR1 |= CR1_JDISCEN_Set;
  }
  else
  {
    /* Disable the selected ADC injected discontinuous mode */
    ADCx->CR1 &= CR1_JDISCEN_Reset;
  }
}

/**
  * @brief  Configures the ADCx external trigger for injected channels conversion.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_ExternalTrigInjecConv: specifies the ADC trigger to start injected conversion.
  *   This parameter can be one of the following values:
  *     @arg ADC_ExternalTrigInjecConv_T1_TRGO: Timer1 TRGO event selected (for ADC1, ADC2 and ADC3)
  *     @arg ADC_ExternalTrigInjecConv_T1_CC4: Timer1 capture compare4 selected (for ADC1, ADC2 and ADC3)
  *     @arg ADC_ExternalTrigInjecConv_T2_TRGO: Timer2 TRGO event selected (for ADC1 and ADC2)
  *     @arg ADC_ExternalTrigInjecConv_T2_CC1: Timer2 capture compare1 selected (for ADC1 and ADC2)
  *     @arg ADC_ExternalTrigInjecConv_T3_CC4: Timer3 capture compare4 selected (for ADC1 and ADC2)
  *     @arg ADC_ExternalTrigInjecConv_T4_TRGO: Timer4 TRGO event selected (for ADC1 and ADC2)
  *     @arg ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4: External interrupt line 15 or Timer8
  *                                                       capture compare4 event selected (for ADC1 and ADC2)
  *     @arg ADC_ExternalTrigInjecConv_T4_CC3: Timer4 capture compare3 selected (for ADC3 only)
  *     @arg ADC_ExternalTrigInjecConv_T8_CC2: Timer8 capture compare2 selected (for ADC3 only)
  *     @arg ADC_ExternalTrigInjecConv_T8_CC4: Timer8 capture compare4 selected (for ADC3 only)
  *     @arg ADC_ExternalTrigInjecConv_T5_TRGO: Timer5 TRGO event selected (for ADC3 only)
  *     @arg ADC_ExternalTrigInjecConv_T5_CC4: Timer5 capture compare4 selected (for ADC3 only)
  *     @arg ADC_ExternalTrigInjecConv_None: Injected conversion started by software and not
  *                                          by external trigger (for ADC1, ADC2 and ADC3)
  * @retval None
  */
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConv)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_EXT_INJEC_TRIG(ADC_ExternalTrigInjecConv));
  /* Get the old register value */
  tmpreg = ADCx->CR2;
  /* Clear the old external event selection for injected group */
  tmpreg &= CR2_JEXTSEL_Reset;
  /* Set the external event selection for injected group */
  tmpreg |= ADC_ExternalTrigInjecConv;
  /* Store the new register value */
  ADCx->CR2 = tmpreg;
}

/**
  * @brief  Enables or disables the ADCx injected channels conversion through
  *         external trigger
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC external trigger start of
  *         injected conversion.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC external event selection for injected group */
    ADCx->CR2 |= CR2_JEXTTRIG_Set;
  }
  else
  {
    /* Disable the selected ADC external event selection for injected group */
    ADCx->CR2 &= CR2_JEXTTRIG_Reset;
  }
}

/**
  * @brief  Enables or disables the selected ADC start of the injected
  *         channels conversion.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  NewState: new state of the selected ADC software start injected conversion.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected ADC conversion for injected group on external event and start the selected
       ADC injected conversion */
    ADCx->CR2 |= CR2_JEXTTRIG_JSWSTART_Set;
  }
  else
  {
    /* Disable the selected ADC conversion on external event for injected group and stop the selected
       ADC injected conversion */
    ADCx->CR2 &= CR2_JEXTTRIG_JSWSTART_Reset;
  }
}

/**
  * @brief  Gets the selected ADC Software start injected conversion Status.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @retval The new state of ADC software start injected conversion (SET or RESET).
  */
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef* ADCx)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  /* Check the status of JSWSTART bit */
  if ((ADCx->CR2 & CR2_JSWSTART_Set) != (uint32_t)RESET)
  {
    /* JSWSTART bit is set */
    bitstatus = SET;
  }
  else
  {
    /* JSWSTART bit is reset */
    bitstatus = RESET;
  }
  /* Return the JSWSTART bit status */
  return  bitstatus;
}

/**
  * @brief  Configures for the selected ADC injected channel its corresponding
  *         rank in the sequencer and its sample time.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure.
  *   This parameter can be one of the following values:
  *     @arg ADC_Channel_0: ADC Channel0 selected
  *     @arg ADC_Channel_1: ADC Channel1 selected
  *     @arg ADC_Channel_2: ADC Channel2 selected
  *     @arg ADC_Channel_3: ADC Channel3 selected
  *     @arg ADC_Channel_4: ADC Channel4 selected
  *     @arg ADC_Channel_5: ADC Channel5 selected
  *     @arg ADC_Channel_6: ADC Channel6 selected
  *     @arg ADC_Channel_7: ADC Channel7 selected
  *     @arg ADC_Channel_8: ADC Channel8 selected
  *     @arg ADC_Channel_9: ADC Channel9 selected
  *     @arg ADC_Channel_10: ADC Channel10 selected
  *     @arg ADC_Channel_11: ADC Channel11 selected
  *     @arg ADC_Channel_12: ADC Channel12 selected
  *     @arg ADC_Channel_13: ADC Channel13 selected
  *     @arg ADC_Channel_14: ADC Channel14 selected
  *     @arg ADC_Channel_15: ADC Channel15 selected
  *     @arg ADC_Channel_16: ADC Channel16 selected
  *     @arg ADC_Channel_17: ADC Channel17 selected
  * @param  Rank: The rank in the injected group sequencer. This parameter must be between 1 and 4.
  * @param  ADC_SampleTime: The sample time value to be set for the selected channel.
  *   This parameter can be one of the following values:
  *     @arg ADC_SampleTime_1Cycles5: Sample time equal to 1.5 cycles
  *     @arg ADC_SampleTime_7Cycles5: Sample time equal to 7.5 cycles
  *     @arg ADC_SampleTime_13Cycles5: Sample time equal to 13.5 cycles
  *     @arg ADC_SampleTime_28Cycles5: Sample time equal to 28.5 cycles
  *     @arg ADC_SampleTime_41Cycles5: Sample time equal to 41.5 cycles
  *     @arg ADC_SampleTime_55Cycles5: Sample time equal to 55.5 cycles
  *     @arg ADC_SampleTime_71Cycles5: Sample time equal to 71.5 cycles
  *     @arg ADC_SampleTime_239Cycles5: Sample time equal to 239.5 cycles
  * @retval None
  */
void ADC_InjectedChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0, tmpreg3 = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_CHANNEL(ADC_Channel));
  assert_param(IS_ADC_INJECTED_RANK(Rank));
  assert_param(IS_ADC_SAMPLE_TIME(ADC_SampleTime));
  /* if ADC_Channel_10 ... ADC_Channel_17 is selected */
  if (ADC_Channel > ADC_Channel_9)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR1;
    /* Calculate the mask to clear */
    tmpreg2 = SMPR1_SMP_Set << (3*(ADC_Channel - 10));
    /* Clear the old channel sample time */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3*(ADC_Channel - 10));
    /* Set the new channel sample time */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SMPR1 = tmpreg1;
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR2;
    /* Calculate the mask to clear */
    tmpreg2 = SMPR2_SMP_Set << (3 * ADC_Channel);
    /* Clear the old channel sample time */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
    /* Set the new channel sample time */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SMPR2 = tmpreg1;
  }
  /* Rank configuration */
  /* Get the old register value */
  tmpreg1 = ADCx->JSQR;
  /* Get JL value: Number = JL+1 */
  tmpreg3 =  (tmpreg1 & JSQR_JL_Set)>> 20;
  /* Calculate the mask to clear: ((Rank-1)+(4-JL-1)) */
  tmpreg2 = JSQR_JSQ_Set << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
  /* Clear the old JSQx bits for the selected rank */
  tmpreg1 &= ~tmpreg2;
  /* Calculate the mask to set: ((Rank-1)+(4-JL-1)) */
  tmpreg2 = (uint32_t)ADC_Channel << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
  /* Set the JSQx bits for the selected rank */
  tmpreg1 |= tmpreg2;
  /* Store the new register value */
  ADCx->JSQR = tmpreg1;
}

/**
  * @brief  Configures the sequencer length for injected channels
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  Length: The sequencer length.
  *   This parameter must be a number between 1 to 4.
  * @retval None
  */
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef* ADCx, uint8_t Length)
{
  uint32_t tmpreg1 = 0;
  uint32_t tmpreg2 = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_INJECTED_LENGTH(Length));

  /* Get the old register value */
  tmpreg1 = ADCx->JSQR;
  /* Clear the old injected sequnence lenght JL bits */
  tmpreg1 &= JSQR_JL_Reset;
  /* Set the injected sequnence lenght JL bits */
  tmpreg2 = Length - 1;
  tmpreg1 |= tmpreg2 << 20;
  /* Store the new register value */
  ADCx->JSQR = tmpreg1;
}

/**
  * @brief  Set the injected channels conversion value offset
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_InjectedChannel: the ADC injected channel to set its offset.
  *   This parameter can be one of the following values:
  *     @arg ADC_InjectedChannel_1: Injected Channel1 selected
  *     @arg ADC_InjectedChannel_2: Injected Channel2 selected
  *     @arg ADC_InjectedChannel_3: Injected Channel3 selected
  *     @arg ADC_InjectedChannel_4: Injected Channel4 selected
  * @param  Offset: the offset value for the selected ADC injected channel
  *   This parameter must be a 12bit value.
  * @retval None
  */
void ADC_SetInjectedOffset(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_INJECTED_CHANNEL(ADC_InjectedChannel));
  assert_param(IS_ADC_OFFSET(Offset));

  tmp = (uint32_t)ADCx;
  tmp += ADC_InjectedChannel;

  /* Set the selected injected channel data offset */
  *(__IO uint32_t *) tmp = (uint32_t)Offset;
}

/**
  * @brief  Returns the ADC injected channel conversion result
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_InjectedChannel: the converted ADC injected channel.
  *   This parameter can be one of the following values:
  *     @arg ADC_InjectedChannel_1: Injected Channel1 selected
  *     @arg ADC_InjectedChannel_2: Injected Channel2 selected
  *     @arg ADC_InjectedChannel_3: Injected Channel3 selected
  *     @arg ADC_InjectedChannel_4: Injected Channel4 selected
  * @retval The Data conversion value.
  */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_INJECTED_CHANNEL(ADC_InjectedChannel));

  tmp = (uint32_t)ADCx;
  tmp += ADC_InjectedChannel + JDR_Offset;

  /* Returns the selected injected channel conversion data value */
  return (uint16_t) (*(__IO uint32_t*)  tmp);
}

/**
  * @brief  Enables or disables the analog watchdog on single/all regular
  *         or injected channels
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_AnalogWatchdog: the ADC analog watchdog configuration.
  *   This parameter can be one of the following values:
  *     @arg ADC_AnalogWatchdog_SingleRegEnable: Analog watchdog on a single regular channel
  *     @arg ADC_AnalogWatchdog_SingleInjecEnable: Analog watchdog on a single injected channel
  *     @arg ADC_AnalogWatchdog_SingleRegOrInjecEnable: Analog watchdog on a single regular or injected channel
  *     @arg ADC_AnalogWatchdog_AllRegEnable: Analog watchdog on  all regular channel
  *     @arg ADC_AnalogWatchdog_AllInjecEnable: Analog watchdog on  all injected channel
  *     @arg ADC_AnalogWatchdog_AllRegAllInjecEnable: Analog watchdog on all regular and injected channels
  *     @arg ADC_AnalogWatchdog_None: No channel guarded by the analog watchdog
  * @retval None
  */
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_ANALOG_WATCHDOG(ADC_AnalogWatchdog));
  /* Get the old register value */
  tmpreg = ADCx->CR1;
  /* Clear AWDEN, AWDENJ and AWDSGL bits */
  tmpreg &= CR1_AWDMode_Reset;
  /* Set the analog watchdog enable mode */
  tmpreg |= ADC_AnalogWatchdog;
  /* Store the new register value */
  ADCx->CR1 = tmpreg;
}

/**
  * @brief  Configures the high and low thresholds of the analog watchdog.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  HighThreshold: the ADC analog watchdog High threshold value.
  *   This parameter must be a 12bit value.
  * @param  LowThreshold: the ADC analog watchdog Low threshold value.
  *   This parameter must be a 12bit value.
  * @retval None
  */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold,
                                        uint16_t LowThreshold)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_THRESHOLD(HighThreshold));
  assert_param(IS_ADC_THRESHOLD(LowThreshold));
  /* Set the ADCx high threshold */
  ADCx->HTR = HighThreshold;
  /* Set the ADCx low threshold */
  ADCx->LTR = LowThreshold;
}

/**
  * @brief  Configures the analog watchdog guarded single channel
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure for the analog watchdog.
  *   This parameter can be one of the following values:
  *     @arg ADC_Channel_0: ADC Channel0 selected
  *     @arg ADC_Channel_1: ADC Channel1 selected
  *     @arg ADC_Channel_2: ADC Channel2 selected
  *     @arg ADC_Channel_3: ADC Channel3 selected
  *     @arg ADC_Channel_4: ADC Channel4 selected
  *     @arg ADC_Channel_5: ADC Channel5 selected
  *     @arg ADC_Channel_6: ADC Channel6 selected
  *     @arg ADC_Channel_7: ADC Channel7 selected
  *     @arg ADC_Channel_8: ADC Channel8 selected
  *     @arg ADC_Channel_9: ADC Channel9 selected
  *     @arg ADC_Channel_10: ADC Channel10 selected
  *     @arg ADC_Channel_11: ADC Channel11 selected
  *     @arg ADC_Channel_12: ADC Channel12 selected
  *     @arg ADC_Channel_13: ADC Channel13 selected
  *     @arg ADC_Channel_14: ADC Channel14 selected
  *     @arg ADC_Channel_15: ADC Channel15 selected
  *     @arg ADC_Channel_16: ADC Channel16 selected
  *     @arg ADC_Channel_17: ADC Channel17 selected
  * @retval None
  */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_CHANNEL(ADC_Channel));
  /* Get the old register value */
  tmpreg = ADCx->CR1;
  /* Clear the Analog watchdog channel select bits */
  tmpreg &= CR1_AWDCH_Reset;
  /* Set the Analog watchdog channel */
  tmpreg |= ADC_Channel;
  /* Store the new register value */
  ADCx->CR1 = tmpreg;
}

/**
  * @brief  Enables or disables the temperature sensor and Vrefint channel.
  * @param  NewState: new state of the temperature sensor.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_TempSensorVrefintCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the temperature sensor and Vrefint channel*/
    ADC1->CR2 |= CR2_TSVREFE_Set;
  }
  else
  {
    /* Disable the temperature sensor and Vrefint channel*/
    ADC1->CR2 &= CR2_TSVREFE_Reset;
  }
}

/**
  * @brief  Checks whether the specified ADC flag is set or not.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg ADC_FLAG_AWD: Analog watchdog flag
  *     @arg ADC_FLAG_EOC: End of conversion flag
  *     @arg ADC_FLAG_JEOC: End of injected group conversion flag
  *     @arg ADC_FLAG_JSTRT: Start of injected group conversion flag
  *     @arg ADC_FLAG_STRT: Start of regular group conversion flag
  * @retval The new state of ADC_FLAG (SET or RESET).
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_GET_FLAG(ADC_FLAG));
  /* Check the status of the specified ADC flag */
  if ((ADCx->SR & ADC_FLAG) != (uint8_t)RESET)
  {
    /* ADC_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* ADC_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the ADC_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Clears the ADCx's pending flags.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_FLAG: specifies the flag to clear.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_FLAG_AWD: Analog watchdog flag
  *     @arg ADC_FLAG_EOC: End of conversion flag
  *     @arg ADC_FLAG_JEOC: End of injected group conversion flag
  *     @arg ADC_FLAG_JSTRT: Start of injected group conversion flag
  *     @arg ADC_FLAG_STRT: Start of regular group conversion flag
  * @retval None
  */
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG)
{
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_CLEAR_FLAG(ADC_FLAG));
  /* Clear the selected ADC flags */
  ADCx->SR = ~(uint32_t)ADC_FLAG;
}

/**
  * @brief  Checks whether the specified ADC interrupt has occurred or not.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg ADC_IT_EOC: End of conversion interrupt mask
  *     @arg ADC_IT_AWD: Analog watchdog interrupt mask
  *     @arg ADC_IT_JEOC: End of injected conversion interrupt mask
  * @retval The new state of ADC_IT (SET or RESET).
  */
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t itmask = 0, enablestatus = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_GET_IT(ADC_IT));
  /* Get the ADC IT index */
  itmask = ADC_IT >> 8;
  /* Get the ADC_IT enable bit status */
  enablestatus = (ADCx->CR1 & (uint8_t)ADC_IT) ;
  /* Check the status of the specified ADC interrupt */
  if (((ADCx->SR & itmask) != (uint32_t)RESET) && enablestatus)
  {
    /* ADC_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* ADC_IT is reset */
    bitstatus = RESET;
  }
  /* Return the ADC_IT status */
  return  bitstatus;
}

/**
  * @brief  Clears the ADCx's interrupt pending bits.
  * @param  ADCx: where x can be 1, 2 or 3 to select the ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_IT_EOC: End of conversion interrupt mask
  *     @arg ADC_IT_AWD: Analog watchdog interrupt mask
  *     @arg ADC_IT_JEOC: End of injected conversion interrupt mask
  * @retval None
  */
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT)
{
  uint8_t itmask = 0;
  /* Check the parameters */
  assert_param(IS_ADC_ALL_PERIPH(ADCx));
  assert_param(IS_ADC_IT(ADC_IT));
  /* Get the ADC IT index */
  itmask = (uint8_t)(ADC_IT >> 8);
  /* Clear the selected ADC interrupt pending bits */
  ADCx->SR = ~(uint32_t)itmask;
}


//STD DMA
/**
  * @brief  DMA Init structure definition
  */

typedef struct
{
  uint32_t DMA_PeripheralBaseAddr; /*!< Specifies the peripheral base address for DMAy Channelx. */

  uint32_t DMA_MemoryBaseAddr;     /*!< Specifies the memory base address for DMAy Channelx. */

  uint32_t DMA_DIR;                /*!< Specifies if the peripheral is the source or destination.
                                        This parameter can be a value of @ref DMA_data_transfer_direction */

  uint32_t DMA_BufferSize;         /*!< Specifies the buffer size, in data unit, of the specified Channel.
                                        The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                        or DMA_MemoryDataSize members depending in the transfer direction. */

  uint32_t DMA_PeripheralInc;      /*!< Specifies whether the Peripheral address register is incremented or not.
                                        This parameter can be a value of @ref DMA_peripheral_incremented_mode */

  uint32_t DMA_MemoryInc;          /*!< Specifies whether the memory address register is incremented or not.
                                        This parameter can be a value of @ref DMA_memory_incremented_mode */

  uint32_t DMA_PeripheralDataSize; /*!< Specifies the Peripheral data width.
                                        This parameter can be a value of @ref DMA_peripheral_data_size */

  uint32_t DMA_MemoryDataSize;     /*!< Specifies the Memory data width.
                                        This parameter can be a value of @ref DMA_memory_data_size */

  uint32_t DMA_Mode;               /*!< Specifies the operation mode of the DMAy Channelx.
                                        This parameter can be a value of @ref DMA_circular_normal_mode.
                                        @note: The circular buffer mode cannot be used if the memory-to-memory
                                              data transfer is configured on the selected Channel */

  uint32_t DMA_Priority;           /*!< Specifies the software priority for the DMAy Channelx.
                                        This parameter can be a value of @ref DMA_priority_level */

  uint32_t DMA_M2M;                /*!< Specifies if the DMAy Channelx will be used in memory-to-memory transfer.
                                        This parameter can be a value of @ref DMA_memory_to_memory */
}STD_DMA_InitTypeDef;

/**
  * @}
  */

/** @defgroup DMA_Exported_Constants
  * @{
  */

#define IS_DMA_ALL_PERIPH(PERIPH) (((PERIPH) == DMA1_Channel1) || \
                                   ((PERIPH) == DMA1_Channel2) || \
                                   ((PERIPH) == DMA1_Channel3) || \
                                   ((PERIPH) == DMA1_Channel4) || \
                                   ((PERIPH) == DMA1_Channel5) || \
                                   ((PERIPH) == DMA1_Channel6) || \
                                   ((PERIPH) == DMA1_Channel7) || \
                                   ((PERIPH) == DMA2_Channel1) || \
                                   ((PERIPH) == DMA2_Channel2) || \
                                   ((PERIPH) == DMA2_Channel3) || \
                                   ((PERIPH) == DMA2_Channel4) || \
                                   ((PERIPH) == DMA2_Channel5))

/** @defgroup DMA_data_transfer_direction
  * @{
  */

#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_DIR_PeripheralSRC              ((uint32_t)0x00000000)
#define IS_DMA_DIR(DIR) (((DIR) == DMA_DIR_PeripheralDST) || \
                         ((DIR) == DMA_DIR_PeripheralSRC))
/**
  * @}
  */

/** @defgroup DMA_peripheral_incremented_mode
  * @{
  */

#define DMA_PeripheralInc_Enable           ((uint32_t)0x00000040)
#define DMA_PeripheralInc_Disable          ((uint32_t)0x00000000)
#define IS_DMA_PERIPHERAL_INC_STATE(STATE) (((STATE) == DMA_PeripheralInc_Enable) || \
                                            ((STATE) == DMA_PeripheralInc_Disable))
/**
  * @}
  */

/** @defgroup DMA_memory_incremented_mode
  * @{
  */

#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)
#define DMA_MemoryInc_Disable              ((uint32_t)0x00000000)
#define IS_DMA_MEMORY_INC_STATE(STATE) (((STATE) == DMA_MemoryInc_Enable) || \
                                        ((STATE) == DMA_MemoryInc_Disable))
/**
  * @}
  */

/** @defgroup DMA_peripheral_data_size
  * @{
  */

#define DMA_PeripheralDataSize_Byte        ((uint32_t)0x00000000)
#define DMA_PeripheralDataSize_HalfWord    ((uint32_t)0x00000100)
#define DMA_PeripheralDataSize_Word        ((uint32_t)0x00000200)
#define IS_DMA_PERIPHERAL_DATA_SIZE(SIZE) (((SIZE) == DMA_PeripheralDataSize_Byte) || \
                                           ((SIZE) == DMA_PeripheralDataSize_HalfWord) || \
                                           ((SIZE) == DMA_PeripheralDataSize_Word))
/**
  * @}
  */

/** @defgroup DMA_memory_data_size
  * @{
  */

#define DMA_MemoryDataSize_Byte            ((uint32_t)0x00000000)
#define DMA_MemoryDataSize_HalfWord        ((uint32_t)0x00000400)
#define DMA_MemoryDataSize_Word            ((uint32_t)0x00000800)
#define IS_DMA_MEMORY_DATA_SIZE(SIZE) (((SIZE) == DMA_MemoryDataSize_Byte) || \
                                       ((SIZE) == DMA_MemoryDataSize_HalfWord) || \
                                       ((SIZE) == DMA_MemoryDataSize_Word))
/**
  * @}
  */

/** @defgroup DMA_circular_normal_mode
  * @{
  */

#define DMA_Mode_Circular                  ((uint32_t)0x00000020)
#define DMA_Mode_Normal                    ((uint32_t)0x00000000)
#define IS_DMA_MODE(MODE) (((MODE) == DMA_Mode_Circular) || ((MODE) == DMA_Mode_Normal))
/**
  * @}
  */

/** @defgroup DMA_priority_level
  * @{
  */

#define DMA_Priority_VeryHigh              ((uint32_t)0x00003000)
#define DMA_Priority_High                  ((uint32_t)0x00002000)
#define DMA_Priority_Medium                ((uint32_t)0x00001000)
#define DMA_Priority_Low                   ((uint32_t)0x00000000)
#define IS_DMA_PRIORITY(PRIORITY) (((PRIORITY) == DMA_Priority_VeryHigh) || \
                                   ((PRIORITY) == DMA_Priority_High) || \
                                   ((PRIORITY) == DMA_Priority_Medium) || \
                                   ((PRIORITY) == DMA_Priority_Low))
/**
  * @}
  */

/** @defgroup DMA_memory_to_memory
  * @{
  */

#define DMA_M2M_Enable                     ((uint32_t)0x00004000)
#define DMA_M2M_Disable                    ((uint32_t)0x00000000)
#define IS_DMA_M2M_STATE(STATE) (((STATE) == DMA_M2M_Enable) || ((STATE) == DMA_M2M_Disable))

/**
  * @}
  */

/** @defgroup DMA_interrupts_definition
  * @{
  */

#define DMA_IT_TC                          ((uint32_t)0x00000002)
#define DMA_IT_HT                          ((uint32_t)0x00000004)
#define DMA_IT_TE                          ((uint32_t)0x00000008)
#define IS_DMA_CONFIG_IT(IT) ((((IT) & 0xFFFFFFF1) == 0x00) && ((IT) != 0x00))

#define DMA1_IT_GL1                        ((uint32_t)0x00000001)
#define DMA1_IT_TC1                        ((uint32_t)0x00000002)
#define DMA1_IT_HT1                        ((uint32_t)0x00000004)
#define DMA1_IT_TE1                        ((uint32_t)0x00000008)
#define DMA1_IT_GL2                        ((uint32_t)0x00000010)
#define DMA1_IT_TC2                        ((uint32_t)0x00000020)
#define DMA1_IT_HT2                        ((uint32_t)0x00000040)
#define DMA1_IT_TE2                        ((uint32_t)0x00000080)
#define DMA1_IT_GL3                        ((uint32_t)0x00000100)
#define DMA1_IT_TC3                        ((uint32_t)0x00000200)
#define DMA1_IT_HT3                        ((uint32_t)0x00000400)
#define DMA1_IT_TE3                        ((uint32_t)0x00000800)
#define DMA1_IT_GL4                        ((uint32_t)0x00001000)
#define DMA1_IT_TC4                        ((uint32_t)0x00002000)
#define DMA1_IT_HT4                        ((uint32_t)0x00004000)
#define DMA1_IT_TE4                        ((uint32_t)0x00008000)
#define DMA1_IT_GL5                        ((uint32_t)0x00010000)
#define DMA1_IT_TC5                        ((uint32_t)0x00020000)
#define DMA1_IT_HT5                        ((uint32_t)0x00040000)
#define DMA1_IT_TE5                        ((uint32_t)0x00080000)
#define DMA1_IT_GL6                        ((uint32_t)0x00100000)
#define DMA1_IT_TC6                        ((uint32_t)0x00200000)
#define DMA1_IT_HT6                        ((uint32_t)0x00400000)
#define DMA1_IT_TE6                        ((uint32_t)0x00800000)
#define DMA1_IT_GL7                        ((uint32_t)0x01000000)
#define DMA1_IT_TC7                        ((uint32_t)0x02000000)
#define DMA1_IT_HT7                        ((uint32_t)0x04000000)
#define DMA1_IT_TE7                        ((uint32_t)0x08000000)

#define DMA2_IT_GL1                        ((uint32_t)0x10000001)
#define DMA2_IT_TC1                        ((uint32_t)0x10000002)
#define DMA2_IT_HT1                        ((uint32_t)0x10000004)
#define DMA2_IT_TE1                        ((uint32_t)0x10000008)
#define DMA2_IT_GL2                        ((uint32_t)0x10000010)
#define DMA2_IT_TC2                        ((uint32_t)0x10000020)
#define DMA2_IT_HT2                        ((uint32_t)0x10000040)
#define DMA2_IT_TE2                        ((uint32_t)0x10000080)
#define DMA2_IT_GL3                        ((uint32_t)0x10000100)
#define DMA2_IT_TC3                        ((uint32_t)0x10000200)
#define DMA2_IT_HT3                        ((uint32_t)0x10000400)
#define DMA2_IT_TE3                        ((uint32_t)0x10000800)
#define DMA2_IT_GL4                        ((uint32_t)0x10001000)
#define DMA2_IT_TC4                        ((uint32_t)0x10002000)
#define DMA2_IT_HT4                        ((uint32_t)0x10004000)
#define DMA2_IT_TE4                        ((uint32_t)0x10008000)
#define DMA2_IT_GL5                        ((uint32_t)0x10010000)
#define DMA2_IT_TC5                        ((uint32_t)0x10020000)
#define DMA2_IT_HT5                        ((uint32_t)0x10040000)
#define DMA2_IT_TE5                        ((uint32_t)0x10080000)

#define IS_DMA_CLEAR_IT(IT) (((((IT) & 0xF0000000) == 0x00) || (((IT) & 0xEFF00000) == 0x00)) && ((IT) != 0x00))

#define IS_DMA_GET_IT(IT) (((IT) == DMA1_IT_GL1) || ((IT) == DMA1_IT_TC1) || \
                           ((IT) == DMA1_IT_HT1) || ((IT) == DMA1_IT_TE1) || \
                           ((IT) == DMA1_IT_GL2) || ((IT) == DMA1_IT_TC2) || \
                           ((IT) == DMA1_IT_HT2) || ((IT) == DMA1_IT_TE2) || \
                           ((IT) == DMA1_IT_GL3) || ((IT) == DMA1_IT_TC3) || \
                           ((IT) == DMA1_IT_HT3) || ((IT) == DMA1_IT_TE3) || \
                           ((IT) == DMA1_IT_GL4) || ((IT) == DMA1_IT_TC4) || \
                           ((IT) == DMA1_IT_HT4) || ((IT) == DMA1_IT_TE4) || \
                           ((IT) == DMA1_IT_GL5) || ((IT) == DMA1_IT_TC5) || \
                           ((IT) == DMA1_IT_HT5) || ((IT) == DMA1_IT_TE5) || \
                           ((IT) == DMA1_IT_GL6) || ((IT) == DMA1_IT_TC6) || \
                           ((IT) == DMA1_IT_HT6) || ((IT) == DMA1_IT_TE6) || \
                           ((IT) == DMA1_IT_GL7) || ((IT) == DMA1_IT_TC7) || \
                           ((IT) == DMA1_IT_HT7) || ((IT) == DMA1_IT_TE7) || \
                           ((IT) == DMA2_IT_GL1) || ((IT) == DMA2_IT_TC1) || \
                           ((IT) == DMA2_IT_HT1) || ((IT) == DMA2_IT_TE1) || \
                           ((IT) == DMA2_IT_GL2) || ((IT) == DMA2_IT_TC2) || \
                           ((IT) == DMA2_IT_HT2) || ((IT) == DMA2_IT_TE2) || \
                           ((IT) == DMA2_IT_GL3) || ((IT) == DMA2_IT_TC3) || \
                           ((IT) == DMA2_IT_HT3) || ((IT) == DMA2_IT_TE3) || \
                           ((IT) == DMA2_IT_GL4) || ((IT) == DMA2_IT_TC4) || \
                           ((IT) == DMA2_IT_HT4) || ((IT) == DMA2_IT_TE4) || \
                           ((IT) == DMA2_IT_GL5) || ((IT) == DMA2_IT_TC5) || \
                           ((IT) == DMA2_IT_HT5) || ((IT) == DMA2_IT_TE5))

/**
  * @}
  */

/** @defgroup DMA_flags_definition
  * @{
  */
#define DMA1_FLAG_GL1                      ((uint32_t)0x00000001)
#define DMA1_FLAG_TC1                      ((uint32_t)0x00000002)
#define DMA1_FLAG_HT1                      ((uint32_t)0x00000004)
#define DMA1_FLAG_TE1                      ((uint32_t)0x00000008)
#define DMA1_FLAG_GL2                      ((uint32_t)0x00000010)
#define DMA1_FLAG_TC2                      ((uint32_t)0x00000020)
#define DMA1_FLAG_HT2                      ((uint32_t)0x00000040)
#define DMA1_FLAG_TE2                      ((uint32_t)0x00000080)
#define DMA1_FLAG_GL3                      ((uint32_t)0x00000100)
#define DMA1_FLAG_TC3                      ((uint32_t)0x00000200)
#define DMA1_FLAG_HT3                      ((uint32_t)0x00000400)
#define DMA1_FLAG_TE3                      ((uint32_t)0x00000800)
#define DMA1_FLAG_GL4                      ((uint32_t)0x00001000)
#define DMA1_FLAG_TC4                      ((uint32_t)0x00002000)
#define DMA1_FLAG_HT4                      ((uint32_t)0x00004000)
#define DMA1_FLAG_TE4                      ((uint32_t)0x00008000)
#define DMA1_FLAG_GL5                      ((uint32_t)0x00010000)
#define DMA1_FLAG_TC5                      ((uint32_t)0x00020000)
#define DMA1_FLAG_HT5                      ((uint32_t)0x00040000)
#define DMA1_FLAG_TE5                      ((uint32_t)0x00080000)
#define DMA1_FLAG_GL6                      ((uint32_t)0x00100000)
#define DMA1_FLAG_TC6                      ((uint32_t)0x00200000)
#define DMA1_FLAG_HT6                      ((uint32_t)0x00400000)
#define DMA1_FLAG_TE6                      ((uint32_t)0x00800000)
#define DMA1_FLAG_GL7                      ((uint32_t)0x01000000)
#define DMA1_FLAG_TC7                      ((uint32_t)0x02000000)
#define DMA1_FLAG_HT7                      ((uint32_t)0x04000000)
#define DMA1_FLAG_TE7                      ((uint32_t)0x08000000)

#define DMA2_FLAG_GL1                      ((uint32_t)0x10000001)
#define DMA2_FLAG_TC1                      ((uint32_t)0x10000002)
#define DMA2_FLAG_HT1                      ((uint32_t)0x10000004)
#define DMA2_FLAG_TE1                      ((uint32_t)0x10000008)
#define DMA2_FLAG_GL2                      ((uint32_t)0x10000010)
#define DMA2_FLAG_TC2                      ((uint32_t)0x10000020)
#define DMA2_FLAG_HT2                      ((uint32_t)0x10000040)
#define DMA2_FLAG_TE2                      ((uint32_t)0x10000080)
#define DMA2_FLAG_GL3                      ((uint32_t)0x10000100)
#define DMA2_FLAG_TC3                      ((uint32_t)0x10000200)
#define DMA2_FLAG_HT3                      ((uint32_t)0x10000400)
#define DMA2_FLAG_TE3                      ((uint32_t)0x10000800)
#define DMA2_FLAG_GL4                      ((uint32_t)0x10001000)
#define DMA2_FLAG_TC4                      ((uint32_t)0x10002000)
#define DMA2_FLAG_HT4                      ((uint32_t)0x10004000)
#define DMA2_FLAG_TE4                      ((uint32_t)0x10008000)
#define DMA2_FLAG_GL5                      ((uint32_t)0x10010000)
#define DMA2_FLAG_TC5                      ((uint32_t)0x10020000)
#define DMA2_FLAG_HT5                      ((uint32_t)0x10040000)
#define DMA2_FLAG_TE5                      ((uint32_t)0x10080000)

#define IS_DMA_CLEAR_FLAG(FLAG) (((((FLAG) & 0xF0000000) == 0x00) || (((FLAG) & 0xEFF00000) == 0x00)) && ((FLAG) != 0x00))

#define IS_DMA_GET_FLAG(FLAG) (((FLAG) == DMA1_FLAG_GL1) || ((FLAG) == DMA1_FLAG_TC1) || \
                               ((FLAG) == DMA1_FLAG_HT1) || ((FLAG) == DMA1_FLAG_TE1) || \
                               ((FLAG) == DMA1_FLAG_GL2) || ((FLAG) == DMA1_FLAG_TC2) || \
                               ((FLAG) == DMA1_FLAG_HT2) || ((FLAG) == DMA1_FLAG_TE2) || \
                               ((FLAG) == DMA1_FLAG_GL3) || ((FLAG) == DMA1_FLAG_TC3) || \
                               ((FLAG) == DMA1_FLAG_HT3) || ((FLAG) == DMA1_FLAG_TE3) || \
                               ((FLAG) == DMA1_FLAG_GL4) || ((FLAG) == DMA1_FLAG_TC4) || \
                               ((FLAG) == DMA1_FLAG_HT4) || ((FLAG) == DMA1_FLAG_TE4) || \
                               ((FLAG) == DMA1_FLAG_GL5) || ((FLAG) == DMA1_FLAG_TC5) || \
                               ((FLAG) == DMA1_FLAG_HT5) || ((FLAG) == DMA1_FLAG_TE5) || \
                               ((FLAG) == DMA1_FLAG_GL6) || ((FLAG) == DMA1_FLAG_TC6) || \
                               ((FLAG) == DMA1_FLAG_HT6) || ((FLAG) == DMA1_FLAG_TE6) || \
                               ((FLAG) == DMA1_FLAG_GL7) || ((FLAG) == DMA1_FLAG_TC7) || \
                               ((FLAG) == DMA1_FLAG_HT7) || ((FLAG) == DMA1_FLAG_TE7) || \
                               ((FLAG) == DMA2_FLAG_GL1) || ((FLAG) == DMA2_FLAG_TC1) || \
                               ((FLAG) == DMA2_FLAG_HT1) || ((FLAG) == DMA2_FLAG_TE1) || \
                               ((FLAG) == DMA2_FLAG_GL2) || ((FLAG) == DMA2_FLAG_TC2) || \
                               ((FLAG) == DMA2_FLAG_HT2) || ((FLAG) == DMA2_FLAG_TE2) || \
                               ((FLAG) == DMA2_FLAG_GL3) || ((FLAG) == DMA2_FLAG_TC3) || \
                               ((FLAG) == DMA2_FLAG_HT3) || ((FLAG) == DMA2_FLAG_TE3) || \
                               ((FLAG) == DMA2_FLAG_GL4) || ((FLAG) == DMA2_FLAG_TC4) || \
                               ((FLAG) == DMA2_FLAG_HT4) || ((FLAG) == DMA2_FLAG_TE4) || \
                               ((FLAG) == DMA2_FLAG_GL5) || ((FLAG) == DMA2_FLAG_TC5) || \
                               ((FLAG) == DMA2_FLAG_HT5) || ((FLAG) == DMA2_FLAG_TE5))
/**
  * @}
  */

/** @defgroup DMA_Buffer_Size
  * @{
  */

#define IS_DMA_BUFFER_SIZE(SIZE) (((SIZE) >= 0x1) && ((SIZE) < 0x10000))


/* DMA1 Channelx interrupt pending bit masks */
#define DMA1_Channel1_IT_Mask    ((uint32_t)(DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_HTIF1 | DMA_ISR_TEIF1))
#define DMA1_Channel2_IT_Mask    ((uint32_t)(DMA_ISR_GIF2 | DMA_ISR_TCIF2 | DMA_ISR_HTIF2 | DMA_ISR_TEIF2))
#define DMA1_Channel3_IT_Mask    ((uint32_t)(DMA_ISR_GIF3 | DMA_ISR_TCIF3 | DMA_ISR_HTIF3 | DMA_ISR_TEIF3))
#define DMA1_Channel4_IT_Mask    ((uint32_t)(DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4))
#define DMA1_Channel5_IT_Mask    ((uint32_t)(DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5))
#define DMA1_Channel6_IT_Mask    ((uint32_t)(DMA_ISR_GIF6 | DMA_ISR_TCIF6 | DMA_ISR_HTIF6 | DMA_ISR_TEIF6))
#define DMA1_Channel7_IT_Mask    ((uint32_t)(DMA_ISR_GIF7 | DMA_ISR_TCIF7 | DMA_ISR_HTIF7 | DMA_ISR_TEIF7))

/* DMA2 Channelx interrupt pending bit masks */
#define DMA2_Channel1_IT_Mask    ((uint32_t)(DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_HTIF1 | DMA_ISR_TEIF1))
#define DMA2_Channel2_IT_Mask    ((uint32_t)(DMA_ISR_GIF2 | DMA_ISR_TCIF2 | DMA_ISR_HTIF2 | DMA_ISR_TEIF2))
#define DMA2_Channel3_IT_Mask    ((uint32_t)(DMA_ISR_GIF3 | DMA_ISR_TCIF3 | DMA_ISR_HTIF3 | DMA_ISR_TEIF3))
#define DMA2_Channel4_IT_Mask    ((uint32_t)(DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4))
#define DMA2_Channel5_IT_Mask    ((uint32_t)(DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5))

/* DMA2 FLAG mask */
#define FLAG_Mask                ((uint32_t)0x10000000)

/* DMA registers Masks */
#define CCR_CLEAR_Mask           ((uint32_t)0xFFFF800F)

/*******************  Bit definition for DMA_CCR1 register  *******************/
#define  DMA_CCR1_EN                         ((uint16_t)0x0001)            /*!< Channel enable*/
#define  DMA_CCR1_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR1_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR1_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR1_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR1_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR1_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR1_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR1_PSIZE                      ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR1_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR1_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR1_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR1_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR1_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR1_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits(Channel Priority level) */
#define  DMA_CCR1_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR1_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR1_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode */

/*******************  Bit definition for DMA_CCR2 register  *******************/
#define  DMA_CCR2_EN                         ((uint16_t)0x0001)            /*!< Channel enable */
#define  DMA_CCR2_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR2_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR2_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR2_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR2_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR2_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR2_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR2_PSIZE                      ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR2_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR2_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR2_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR2_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR2_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR2_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits (Channel Priority level) */
#define  DMA_CCR2_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR2_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR2_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode */

/*******************  Bit definition for DMA_CCR3 register  *******************/
#define  DMA_CCR3_EN                         ((uint16_t)0x0001)            /*!< Channel enable */
#define  DMA_CCR3_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR3_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR3_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR3_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR3_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR3_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR3_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR3_PSIZE                      ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR3_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR3_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR3_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR3_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR3_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR3_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits (Channel Priority level) */
#define  DMA_CCR3_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR3_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR3_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode */

/*!<******************  Bit definition for DMA_CCR4 register  *******************/
#define  DMA_CCR4_EN                         ((uint16_t)0x0001)            /*!< Channel enable */
#define  DMA_CCR4_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR4_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR4_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR4_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR4_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR4_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR4_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR4_PSIZE                      ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR4_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR4_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR4_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR4_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR4_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR4_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits (Channel Priority level) */
#define  DMA_CCR4_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR4_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR4_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode */

/******************  Bit definition for DMA_CCR5 register  *******************/
#define  DMA_CCR5_EN                         ((uint16_t)0x0001)            /*!< Channel enable */
#define  DMA_CCR5_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR5_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR5_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR5_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR5_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR5_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR5_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR5_PSIZE                      ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR5_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR5_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR5_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR5_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR5_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR5_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits (Channel Priority level) */
#define  DMA_CCR5_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR5_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR5_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode enable */

/*******************  Bit definition for DMA_CCR6 register  *******************/
#define  DMA_CCR6_EN                         ((uint16_t)0x0001)            /*!< Channel enable */
#define  DMA_CCR6_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR6_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR6_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR6_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR6_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR6_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR6_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR6_PSIZE                      ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR6_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR6_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR6_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR6_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR6_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR6_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits (Channel Priority level) */
#define  DMA_CCR6_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR6_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR6_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode */

/*******************  Bit definition for DMA_CCR7 register  *******************/
#define  DMA_CCR7_EN                         ((uint16_t)0x0001)            /*!< Channel enable */
#define  DMA_CCR7_TCIE                       ((uint16_t)0x0002)            /*!< Transfer complete interrupt enable */
#define  DMA_CCR7_HTIE                       ((uint16_t)0x0004)            /*!< Half Transfer interrupt enable */
#define  DMA_CCR7_TEIE                       ((uint16_t)0x0008)            /*!< Transfer error interrupt enable */
#define  DMA_CCR7_DIR                        ((uint16_t)0x0010)            /*!< Data transfer direction */
#define  DMA_CCR7_CIRC                       ((uint16_t)0x0020)            /*!< Circular mode */
#define  DMA_CCR7_PINC                       ((uint16_t)0x0040)            /*!< Peripheral increment mode */
#define  DMA_CCR7_MINC                       ((uint16_t)0x0080)            /*!< Memory increment mode */

#define  DMA_CCR7_PSIZE            ,         ((uint16_t)0x0300)            /*!< PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CCR7_PSIZE_0                    ((uint16_t)0x0100)            /*!< Bit 0 */
#define  DMA_CCR7_PSIZE_1                    ((uint16_t)0x0200)            /*!< Bit 1 */

#define  DMA_CCR7_MSIZE                      ((uint16_t)0x0C00)            /*!< MSIZE[1:0] bits (Memory size) */
#define  DMA_CCR7_MSIZE_0                    ((uint16_t)0x0400)            /*!< Bit 0 */
#define  DMA_CCR7_MSIZE_1                    ((uint16_t)0x0800)            /*!< Bit 1 */

#define  DMA_CCR7_PL                         ((uint16_t)0x3000)            /*!< PL[1:0] bits (Channel Priority level) */
#define  DMA_CCR7_PL_0                       ((uint16_t)0x1000)            /*!< Bit 0 */
#define  DMA_CCR7_PL_1                       ((uint16_t)0x2000)            /*!< Bit 1 */

#define  DMA_CCR7_MEM2MEM                    ((uint16_t)0x4000)            /*!< Memory to memory mode enable */


/**
  * @brief  Deinitializes the DMAy Channelx registers to their default reset
  *         values.
  * @param  DMAy_Channelx: where y can be 1 or 2 to select the DMA and
  *   x can be 1 to 7 for DMA1 and 1 to 5 for DMA2 to select the DMA Channel.
  * @retval None
  */
void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Channelx));

  /* Disable the selected DMAy Channelx */
  DMAy_Channelx->CCR &= (uint16_t)(~DMA_CCR1_EN);

  /* Reset DMAy Channelx control register */
  DMAy_Channelx->CCR  = 0;

  /* Reset DMAy Channelx remaining bytes register */
  DMAy_Channelx->CNDTR = 0;

  /* Reset DMAy Channelx peripheral address register */
  DMAy_Channelx->CPAR  = 0;

  /* Reset DMAy Channelx memory address register */
  DMAy_Channelx->CMAR = 0;

  if (DMAy_Channelx == DMA1_Channel1)
  {
    /* Reset interrupt pending bits for DMA1 Channel1 */
    DMA1->IFCR |= DMA1_Channel1_IT_Mask;
  }
  else if (DMAy_Channelx == DMA1_Channel2)
  {
    /* Reset interrupt pending bits for DMA1 Channel2 */
    DMA1->IFCR |= DMA1_Channel2_IT_Mask;
  }
  else if (DMAy_Channelx == DMA1_Channel3)
  {
    /* Reset interrupt pending bits for DMA1 Channel3 */
    DMA1->IFCR |= DMA1_Channel3_IT_Mask;
  }
  else if (DMAy_Channelx == DMA1_Channel4)
  {
    /* Reset interrupt pending bits for DMA1 Channel4 */
    DMA1->IFCR |= DMA1_Channel4_IT_Mask;
  }
  else if (DMAy_Channelx == DMA1_Channel5)
  {
    /* Reset interrupt pending bits for DMA1 Channel5 */
    DMA1->IFCR |= DMA1_Channel5_IT_Mask;
  }
  else if (DMAy_Channelx == DMA1_Channel6)
  {
    /* Reset interrupt pending bits for DMA1 Channel6 */
    DMA1->IFCR |= DMA1_Channel6_IT_Mask;
  }
  else if (DMAy_Channelx == DMA1_Channel7)
  {
    /* Reset interrupt pending bits for DMA1 Channel7 */
    DMA1->IFCR |= DMA1_Channel7_IT_Mask;
  }
  else if (DMAy_Channelx == DMA2_Channel1)
  {
    /* Reset interrupt pending bits for DMA2 Channel1 */
    DMA2->IFCR |= DMA2_Channel1_IT_Mask;
  }
  else if (DMAy_Channelx == DMA2_Channel2)
  {
    /* Reset interrupt pending bits for DMA2 Channel2 */
    DMA2->IFCR |= DMA2_Channel2_IT_Mask;
  }
  else if (DMAy_Channelx == DMA2_Channel3)
  {
    /* Reset interrupt pending bits for DMA2 Channel3 */
    DMA2->IFCR |= DMA2_Channel3_IT_Mask;
  }
  else if (DMAy_Channelx == DMA2_Channel4)
  {
    /* Reset interrupt pending bits for DMA2 Channel4 */
    DMA2->IFCR |= DMA2_Channel4_IT_Mask;
  }
  else
  {
    if (DMAy_Channelx == DMA2_Channel5)
    {
      /* Reset interrupt pending bits for DMA2 Channel5 */
      DMA2->IFCR |= DMA2_Channel5_IT_Mask;
    }
  }
}

/**
  * @brief  Initializes the DMAy Channelx according to the specified
  *         parameters in the DMA_InitStruct.
  * @param  DMAy_Channelx: where y can be 1 or 2 to select the DMA and
  *   x can be 1 to 7 for DMA1 and 1 to 5 for DMA2 to select the DMA Channel.
  * @param  DMA_InitStruct: pointer to a DMA_InitTypeDef structure that
  *         contains the configuration information for the specified DMA Channel.
  * @retval None
  */
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, STD_DMA_InitTypeDef* DMA_InitStruct)
{
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Channelx));
  assert_param(IS_DMA_DIR(DMA_InitStruct->DMA_DIR));
  assert_param(IS_DMA_BUFFER_SIZE(DMA_InitStruct->DMA_BufferSize));
  assert_param(IS_DMA_PERIPHERAL_INC_STATE(DMA_InitStruct->DMA_PeripheralInc));
  assert_param(IS_DMA_MEMORY_INC_STATE(DMA_InitStruct->DMA_MemoryInc));
  assert_param(IS_DMA_PERIPHERAL_DATA_SIZE(DMA_InitStruct->DMA_PeripheralDataSize));
  assert_param(IS_DMA_MEMORY_DATA_SIZE(DMA_InitStruct->DMA_MemoryDataSize));
  assert_param(IS_DMA_MODE(DMA_InitStruct->DMA_Mode));
  assert_param(IS_DMA_PRIORITY(DMA_InitStruct->DMA_Priority));
  assert_param(IS_DMA_M2M_STATE(DMA_InitStruct->DMA_M2M));

/*--------------------------- DMAy Channelx CCR Configuration -----------------*/
  /* Get the DMAy_Channelx CCR value */
  tmpreg = DMAy_Channelx->CCR;
  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  tmpreg &= CCR_CLEAR_Mask;
  /* Configure DMAy Channelx: data transfer, data size, priority level and mode */
  /* Set DIR bit according to DMA_DIR value */
  /* Set CIRC bit according to DMA_Mode value */
  /* Set PINC bit according to DMA_PeripheralInc value */
  /* Set MINC bit according to DMA_MemoryInc value */
  /* Set PSIZE bits according to DMA_PeripheralDataSize value */
  /* Set MSIZE bits according to DMA_MemoryDataSize value */
  /* Set PL bits according to DMA_Priority value */
  /* Set the MEM2MEM bit according to DMA_M2M value */
  tmpreg |= DMA_InitStruct->DMA_DIR | DMA_InitStruct->DMA_Mode |
            DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
            DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
            DMA_InitStruct->DMA_Priority | DMA_InitStruct->DMA_M2M;

  /* Write to DMAy Channelx CCR */
  DMAy_Channelx->CCR = tmpreg;

/*--------------------------- DMAy Channelx CNDTR Configuration ---------------*/
  /* Write to DMAy Channelx CNDTR */
  DMAy_Channelx->CNDTR = DMA_InitStruct->DMA_BufferSize;

/*--------------------------- DMAy Channelx CPAR Configuration ----------------*/
  /* Write to DMAy Channelx CPAR */
  DMAy_Channelx->CPAR = DMA_InitStruct->DMA_PeripheralBaseAddr;

/*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
  /* Write to DMAy Channelx CMAR */
  DMAy_Channelx->CMAR = DMA_InitStruct->DMA_MemoryBaseAddr;
}

/**
  * @brief  Fills each DMA_InitStruct member with its default value.
  * @param  DMA_InitStruct : pointer to a DMA_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void DMA_StructInit(STD_DMA_InitTypeDef* DMA_InitStruct)
{
/*-------------- Reset DMA init structure parameters values ------------------*/
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStruct->DMA_PeripheralBaseAddr = 0;
  /* Initialize the DMA_MemoryBaseAddr member */
  DMA_InitStruct->DMA_MemoryBaseAddr = 0;
  /* Initialize the DMA_DIR member */
  DMA_InitStruct->DMA_DIR = DMA_DIR_PeripheralSRC;
  /* Initialize the DMA_BufferSize member */
  DMA_InitStruct->DMA_BufferSize = 0;
  /* Initialize the DMA_PeripheralInc member */
  DMA_InitStruct->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStruct->DMA_MemoryInc = DMA_MemoryInc_Disable;
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStruct->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStruct->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* Initialize the DMA_Mode member */
  DMA_InitStruct->DMA_Mode = DMA_Mode_Normal;
  /* Initialize the DMA_Priority member */
  DMA_InitStruct->DMA_Priority = DMA_Priority_Low;
  /* Initialize the DMA_M2M member */
  DMA_InitStruct->DMA_M2M = DMA_M2M_Disable;
}

/**
  * @brief  Enables or disables the specified DMAy Channelx.
  * @param  DMAy_Channelx: where y can be 1 or 2 to select the DMA and
  *   x can be 1 to 7 for DMA1 and 1 to 5 for DMA2 to select the DMA Channel.
  * @param  NewState: new state of the DMAy Channelx.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Channelx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected DMAy Channelx */
    DMAy_Channelx->CCR |= DMA_CCR1_EN;
  }
  else
  {
    /* Disable the selected DMAy Channelx */
    DMAy_Channelx->CCR &= (uint16_t)(~DMA_CCR1_EN);
  }
}

/**
  * @brief  Enables or disables the specified DMAy Channelx interrupts.
  * @param  DMAy_Channelx: where y can be 1 or 2 to select the DMA and
  *   x can be 1 to 7 for DMA1 and 1 to 5 for DMA2 to select the DMA Channel.
  * @param  DMA_IT: specifies the DMA interrupts sources to be enabled
  *   or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg DMA_IT_TC:  Transfer complete interrupt mask
  *     @arg DMA_IT_HT:  Half transfer interrupt mask
  *     @arg DMA_IT_TE:  Transfer error interrupt mask
  * @param  NewState: new state of the specified DMA interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Channelx));
  assert_param(IS_DMA_CONFIG_IT(DMA_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected DMA interrupts */
    DMAy_Channelx->CCR |= DMA_IT;
  }
  else
  {
    /* Disable the selected DMA interrupts */
    DMAy_Channelx->CCR &= ~DMA_IT;
  }
}

/**
  * @brief  Sets the number of data units in the current DMAy Channelx transfer.
  * @param  DMAy_Channelx: where y can be 1 or 2 to select the DMA and
  *         x can be 1 to 7 for DMA1 and 1 to 5 for DMA2 to select the DMA Channel.
  * @param  DataNumber: The number of data units in the current DMAy Channelx
  *         transfer.
  * @note   This function can only be used when the DMAy_Channelx is disabled.
  * @retval None.
  */
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Channelx));

/*--------------------------- DMAy Channelx CNDTR Configuration ---------------*/
  /* Write to DMAy Channelx CNDTR */
  DMAy_Channelx->CNDTR = DataNumber;
}

/**
  * @brief  Returns the number of remaining data units in the current
  *         DMAy Channelx transfer.
  * @param  DMAy_Channelx: where y can be 1 or 2 to select the DMA and
  *   x can be 1 to 7 for DMA1 and 1 to 5 for DMA2 to select the DMA Channel.
  * @retval The number of remaining data units in the current DMAy Channelx
  *         transfer.
  */
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Channelx));
  /* Return the number of remaining data units for DMAy Channelx */
  return ((uint16_t)(DMAy_Channelx->CNDTR));
}

/**
  * @brief  Checks whether the specified DMAy Channelx flag is set or not.
  * @param  DMAy_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg DMA1_FLAG_GL1: DMA1 Channel1 global flag.
  *     @arg DMA1_FLAG_TC1: DMA1 Channel1 transfer complete flag.
  *     @arg DMA1_FLAG_HT1: DMA1 Channel1 half transfer flag.
  *     @arg DMA1_FLAG_TE1: DMA1 Channel1 transfer error flag.
  *     @arg DMA1_FLAG_GL2: DMA1 Channel2 global flag.
  *     @arg DMA1_FLAG_TC2: DMA1 Channel2 transfer complete flag.
  *     @arg DMA1_FLAG_HT2: DMA1 Channel2 half transfer flag.
  *     @arg DMA1_FLAG_TE2: DMA1 Channel2 transfer error flag.
  *     @arg DMA1_FLAG_GL3: DMA1 Channel3 global flag.
  *     @arg DMA1_FLAG_TC3: DMA1 Channel3 transfer complete flag.
  *     @arg DMA1_FLAG_HT3: DMA1 Channel3 half transfer flag.
  *     @arg DMA1_FLAG_TE3: DMA1 Channel3 transfer error flag.
  *     @arg DMA1_FLAG_GL4: DMA1 Channel4 global flag.
  *     @arg DMA1_FLAG_TC4: DMA1 Channel4 transfer complete flag.
  *     @arg DMA1_FLAG_HT4: DMA1 Channel4 half transfer flag.
  *     @arg DMA1_FLAG_TE4: DMA1 Channel4 transfer error flag.
  *     @arg DMA1_FLAG_GL5: DMA1 Channel5 global flag.
  *     @arg DMA1_FLAG_TC5: DMA1 Channel5 transfer complete flag.
  *     @arg DMA1_FLAG_HT5: DMA1 Channel5 half transfer flag.
  *     @arg DMA1_FLAG_TE5: DMA1 Channel5 transfer error flag.
  *     @arg DMA1_FLAG_GL6: DMA1 Channel6 global flag.
  *     @arg DMA1_FLAG_TC6: DMA1 Channel6 transfer complete flag.
  *     @arg DMA1_FLAG_HT6: DMA1 Channel6 half transfer flag.
  *     @arg DMA1_FLAG_TE6: DMA1 Channel6 transfer error flag.
  *     @arg DMA1_FLAG_GL7: DMA1 Channel7 global flag.
  *     @arg DMA1_FLAG_TC7: DMA1 Channel7 transfer complete flag.
  *     @arg DMA1_FLAG_HT7: DMA1 Channel7 half transfer flag.
  *     @arg DMA1_FLAG_TE7: DMA1 Channel7 transfer error flag.
  *     @arg DMA2_FLAG_GL1: DMA2 Channel1 global flag.
  *     @arg DMA2_FLAG_TC1: DMA2 Channel1 transfer complete flag.
  *     @arg DMA2_FLAG_HT1: DMA2 Channel1 half transfer flag.
  *     @arg DMA2_FLAG_TE1: DMA2 Channel1 transfer error flag.
  *     @arg DMA2_FLAG_GL2: DMA2 Channel2 global flag.
  *     @arg DMA2_FLAG_TC2: DMA2 Channel2 transfer complete flag.
  *     @arg DMA2_FLAG_HT2: DMA2 Channel2 half transfer flag.
  *     @arg DMA2_FLAG_TE2: DMA2 Channel2 transfer error flag.
  *     @arg DMA2_FLAG_GL3: DMA2 Channel3 global flag.
  *     @arg DMA2_FLAG_TC3: DMA2 Channel3 transfer complete flag.
  *     @arg DMA2_FLAG_HT3: DMA2 Channel3 half transfer flag.
  *     @arg DMA2_FLAG_TE3: DMA2 Channel3 transfer error flag.
  *     @arg DMA2_FLAG_GL4: DMA2 Channel4 global flag.
  *     @arg DMA2_FLAG_TC4: DMA2 Channel4 transfer complete flag.
  *     @arg DMA2_FLAG_HT4: DMA2 Channel4 half transfer flag.
  *     @arg DMA2_FLAG_TE4: DMA2 Channel4 transfer error flag.
  *     @arg DMA2_FLAG_GL5: DMA2 Channel5 global flag.
  *     @arg DMA2_FLAG_TC5: DMA2 Channel5 transfer complete flag.
  *     @arg DMA2_FLAG_HT5: DMA2 Channel5 half transfer flag.
  *     @arg DMA2_FLAG_TE5: DMA2 Channel5 transfer error flag.
  * @retval The new state of DMAy_FLAG (SET or RESET).
  */
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_DMA_GET_FLAG(DMAy_FLAG));

  /* Calculate the used DMAy */
  if ((DMAy_FLAG & FLAG_Mask) != (uint32_t)RESET)
  {
    /* Get DMA2 ISR register value */
    tmpreg = DMA2->ISR ;
  }
  else
  {
    /* Get DMA1 ISR register value */
    tmpreg = DMA1->ISR ;
  }

  /* Check the status of the specified DMAy flag */
  if ((tmpreg & DMAy_FLAG) != (uint32_t)RESET)
  {
    /* DMAy_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* DMAy_FLAG is reset */
    bitstatus = RESET;
  }

  /* Return the DMAy_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Clears the DMAy Channelx's pending flags.
  * @param  DMAy_FLAG: specifies the flag to clear.
  *   This parameter can be any combination (for the same DMA) of the following values:
  *     @arg DMA1_FLAG_GL1: DMA1 Channel1 global flag.
  *     @arg DMA1_FLAG_TC1: DMA1 Channel1 transfer complete flag.
  *     @arg DMA1_FLAG_HT1: DMA1 Channel1 half transfer flag.
  *     @arg DMA1_FLAG_TE1: DMA1 Channel1 transfer error flag.
  *     @arg DMA1_FLAG_GL2: DMA1 Channel2 global flag.
  *     @arg DMA1_FLAG_TC2: DMA1 Channel2 transfer complete flag.
  *     @arg DMA1_FLAG_HT2: DMA1 Channel2 half transfer flag.
  *     @arg DMA1_FLAG_TE2: DMA1 Channel2 transfer error flag.
  *     @arg DMA1_FLAG_GL3: DMA1 Channel3 global flag.
  *     @arg DMA1_FLAG_TC3: DMA1 Channel3 transfer complete flag.
  *     @arg DMA1_FLAG_HT3: DMA1 Channel3 half transfer flag.
  *     @arg DMA1_FLAG_TE3: DMA1 Channel3 transfer error flag.
  *     @arg DMA1_FLAG_GL4: DMA1 Channel4 global flag.
  *     @arg DMA1_FLAG_TC4: DMA1 Channel4 transfer complete flag.
  *     @arg DMA1_FLAG_HT4: DMA1 Channel4 half transfer flag.
  *     @arg DMA1_FLAG_TE4: DMA1 Channel4 transfer error flag.
  *     @arg DMA1_FLAG_GL5: DMA1 Channel5 global flag.
  *     @arg DMA1_FLAG_TC5: DMA1 Channel5 transfer complete flag.
  *     @arg DMA1_FLAG_HT5: DMA1 Channel5 half transfer flag.
  *     @arg DMA1_FLAG_TE5: DMA1 Channel5 transfer error flag.
  *     @arg DMA1_FLAG_GL6: DMA1 Channel6 global flag.
  *     @arg DMA1_FLAG_TC6: DMA1 Channel6 transfer complete flag.
  *     @arg DMA1_FLAG_HT6: DMA1 Channel6 half transfer flag.
  *     @arg DMA1_FLAG_TE6: DMA1 Channel6 transfer error flag.
  *     @arg DMA1_FLAG_GL7: DMA1 Channel7 global flag.
  *     @arg DMA1_FLAG_TC7: DMA1 Channel7 transfer complete flag.
  *     @arg DMA1_FLAG_HT7: DMA1 Channel7 half transfer flag.
  *     @arg DMA1_FLAG_TE7: DMA1 Channel7 transfer error flag.
  *     @arg DMA2_FLAG_GL1: DMA2 Channel1 global flag.
  *     @arg DMA2_FLAG_TC1: DMA2 Channel1 transfer complete flag.
  *     @arg DMA2_FLAG_HT1: DMA2 Channel1 half transfer flag.
  *     @arg DMA2_FLAG_TE1: DMA2 Channel1 transfer error flag.
  *     @arg DMA2_FLAG_GL2: DMA2 Channel2 global flag.
  *     @arg DMA2_FLAG_TC2: DMA2 Channel2 transfer complete flag.
  *     @arg DMA2_FLAG_HT2: DMA2 Channel2 half transfer flag.
  *     @arg DMA2_FLAG_TE2: DMA2 Channel2 transfer error flag.
  *     @arg DMA2_FLAG_GL3: DMA2 Channel3 global flag.
  *     @arg DMA2_FLAG_TC3: DMA2 Channel3 transfer complete flag.
  *     @arg DMA2_FLAG_HT3: DMA2 Channel3 half transfer flag.
  *     @arg DMA2_FLAG_TE3: DMA2 Channel3 transfer error flag.
  *     @arg DMA2_FLAG_GL4: DMA2 Channel4 global flag.
  *     @arg DMA2_FLAG_TC4: DMA2 Channel4 transfer complete flag.
  *     @arg DMA2_FLAG_HT4: DMA2 Channel4 half transfer flag.
  *     @arg DMA2_FLAG_TE4: DMA2 Channel4 transfer error flag.
  *     @arg DMA2_FLAG_GL5: DMA2 Channel5 global flag.
  *     @arg DMA2_FLAG_TC5: DMA2 Channel5 transfer complete flag.
  *     @arg DMA2_FLAG_HT5: DMA2 Channel5 half transfer flag.
  *     @arg DMA2_FLAG_TE5: DMA2 Channel5 transfer error flag.
  * @retval None
  */
void DMA_ClearFlag(uint32_t DMAy_FLAG)
{
  /* Check the parameters */
  assert_param(IS_DMA_CLEAR_FLAG(DMAy_FLAG));

  /* Calculate the used DMAy */
  if ((DMAy_FLAG & FLAG_Mask) != (uint32_t)RESET)
  {
    /* Clear the selected DMAy flags */
    DMA2->IFCR = DMAy_FLAG;
  }
  else
  {
    /* Clear the selected DMAy flags */
    DMA1->IFCR = DMAy_FLAG;
  }
}

/**
  * @brief  Checks whether the specified DMAy Channelx interrupt has occurred or not.
  * @param  DMAy_IT: specifies the DMAy interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg DMA1_IT_GL1: DMA1 Channel1 global interrupt.
  *     @arg DMA1_IT_TC1: DMA1 Channel1 transfer complete interrupt.
  *     @arg DMA1_IT_HT1: DMA1 Channel1 half transfer interrupt.
  *     @arg DMA1_IT_TE1: DMA1 Channel1 transfer error interrupt.
  *     @arg DMA1_IT_GL2: DMA1 Channel2 global interrupt.
  *     @arg DMA1_IT_TC2: DMA1 Channel2 transfer complete interrupt.
  *     @arg DMA1_IT_HT2: DMA1 Channel2 half transfer interrupt.
  *     @arg DMA1_IT_TE2: DMA1 Channel2 transfer error interrupt.
  *     @arg DMA1_IT_GL3: DMA1 Channel3 global interrupt.
  *     @arg DMA1_IT_TC3: DMA1 Channel3 transfer complete interrupt.
  *     @arg DMA1_IT_HT3: DMA1 Channel3 half transfer interrupt.
  *     @arg DMA1_IT_TE3: DMA1 Channel3 transfer error interrupt.
  *     @arg DMA1_IT_GL4: DMA1 Channel4 global interrupt.
  *     @arg DMA1_IT_TC4: DMA1 Channel4 transfer complete interrupt.
  *     @arg DMA1_IT_HT4: DMA1 Channel4 half transfer interrupt.
  *     @arg DMA1_IT_TE4: DMA1 Channel4 transfer error interrupt.
  *     @arg DMA1_IT_GL5: DMA1 Channel5 global interrupt.
  *     @arg DMA1_IT_TC5: DMA1 Channel5 transfer complete interrupt.
  *     @arg DMA1_IT_HT5: DMA1 Channel5 half transfer interrupt.
  *     @arg DMA1_IT_TE5: DMA1 Channel5 transfer error interrupt.
  *     @arg DMA1_IT_GL6: DMA1 Channel6 global interrupt.
  *     @arg DMA1_IT_TC6: DMA1 Channel6 transfer complete interrupt.
  *     @arg DMA1_IT_HT6: DMA1 Channel6 half transfer interrupt.
  *     @arg DMA1_IT_TE6: DMA1 Channel6 transfer error interrupt.
  *     @arg DMA1_IT_GL7: DMA1 Channel7 global interrupt.
  *     @arg DMA1_IT_TC7: DMA1 Channel7 transfer complete interrupt.
  *     @arg DMA1_IT_HT7: DMA1 Channel7 half transfer interrupt.
  *     @arg DMA1_IT_TE7: DMA1 Channel7 transfer error interrupt.
  *     @arg DMA2_IT_GL1: DMA2 Channel1 global interrupt.
  *     @arg DMA2_IT_TC1: DMA2 Channel1 transfer complete interrupt.
  *     @arg DMA2_IT_HT1: DMA2 Channel1 half transfer interrupt.
  *     @arg DMA2_IT_TE1: DMA2 Channel1 transfer error interrupt.
  *     @arg DMA2_IT_GL2: DMA2 Channel2 global interrupt.
  *     @arg DMA2_IT_TC2: DMA2 Channel2 transfer complete interrupt.
  *     @arg DMA2_IT_HT2: DMA2 Channel2 half transfer interrupt.
  *     @arg DMA2_IT_TE2: DMA2 Channel2 transfer error interrupt.
  *     @arg DMA2_IT_GL3: DMA2 Channel3 global interrupt.
  *     @arg DMA2_IT_TC3: DMA2 Channel3 transfer complete interrupt.
  *     @arg DMA2_IT_HT3: DMA2 Channel3 half transfer interrupt.
  *     @arg DMA2_IT_TE3: DMA2 Channel3 transfer error interrupt.
  *     @arg DMA2_IT_GL4: DMA2 Channel4 global interrupt.
  *     @arg DMA2_IT_TC4: DMA2 Channel4 transfer complete interrupt.
  *     @arg DMA2_IT_HT4: DMA2 Channel4 half transfer interrupt.
  *     @arg DMA2_IT_TE4: DMA2 Channel4 transfer error interrupt.
  *     @arg DMA2_IT_GL5: DMA2 Channel5 global interrupt.
  *     @arg DMA2_IT_TC5: DMA2 Channel5 transfer complete interrupt.
  *     @arg DMA2_IT_HT5: DMA2 Channel5 half transfer interrupt.
  *     @arg DMA2_IT_TE5: DMA2 Channel5 transfer error interrupt.
  * @retval The new state of DMAy_IT (SET or RESET).
  */
ITStatus DMA_GetITStatus(uint32_t DMAy_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_DMA_GET_IT(DMAy_IT));

  /* Calculate the used DMA */
  if ((DMAy_IT & FLAG_Mask) != (uint32_t)RESET)
  {
    /* Get DMA2 ISR register value */
    tmpreg = DMA2->ISR;
  }
  else
  {
    /* Get DMA1 ISR register value */
    tmpreg = DMA1->ISR;
  }

  /* Check the status of the specified DMAy interrupt */
  if ((tmpreg & DMAy_IT) != (uint32_t)RESET)
  {
    /* DMAy_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* DMAy_IT is reset */
    bitstatus = RESET;
  }
  /* Return the DMA_IT status */
  return  bitstatus;
}

/**
  * @brief  Clears the DMAy Channelx's interrupt pending bits.
  * @param  DMAy_IT: specifies the DMAy interrupt pending bit to clear.
  *   This parameter can be any combination (for the same DMA) of the following values:
  *     @arg DMA1_IT_GL1: DMA1 Channel1 global interrupt.
  *     @arg DMA1_IT_TC1: DMA1 Channel1 transfer complete interrupt.
  *     @arg DMA1_IT_HT1: DMA1 Channel1 half transfer interrupt.
  *     @arg DMA1_IT_TE1: DMA1 Channel1 transfer error interrupt.
  *     @arg DMA1_IT_GL2: DMA1 Channel2 global interrupt.
  *     @arg DMA1_IT_TC2: DMA1 Channel2 transfer complete interrupt.
  *     @arg DMA1_IT_HT2: DMA1 Channel2 half transfer interrupt.
  *     @arg DMA1_IT_TE2: DMA1 Channel2 transfer error interrupt.
  *     @arg DMA1_IT_GL3: DMA1 Channel3 global interrupt.
  *     @arg DMA1_IT_TC3: DMA1 Channel3 transfer complete interrupt.
  *     @arg DMA1_IT_HT3: DMA1 Channel3 half transfer interrupt.
  *     @arg DMA1_IT_TE3: DMA1 Channel3 transfer error interrupt.
  *     @arg DMA1_IT_GL4: DMA1 Channel4 global interrupt.
  *     @arg DMA1_IT_TC4: DMA1 Channel4 transfer complete interrupt.
  *     @arg DMA1_IT_HT4: DMA1 Channel4 half transfer interrupt.
  *     @arg DMA1_IT_TE4: DMA1 Channel4 transfer error interrupt.
  *     @arg DMA1_IT_GL5: DMA1 Channel5 global interrupt.
  *     @arg DMA1_IT_TC5: DMA1 Channel5 transfer complete interrupt.
  *     @arg DMA1_IT_HT5: DMA1 Channel5 half transfer interrupt.
  *     @arg DMA1_IT_TE5: DMA1 Channel5 transfer error interrupt.
  *     @arg DMA1_IT_GL6: DMA1 Channel6 global interrupt.
  *     @arg DMA1_IT_TC6: DMA1 Channel6 transfer complete interrupt.
  *     @arg DMA1_IT_HT6: DMA1 Channel6 half transfer interrupt.
  *     @arg DMA1_IT_TE6: DMA1 Channel6 transfer error interrupt.
  *     @arg DMA1_IT_GL7: DMA1 Channel7 global interrupt.
  *     @arg DMA1_IT_TC7: DMA1 Channel7 transfer complete interrupt.
  *     @arg DMA1_IT_HT7: DMA1 Channel7 half transfer interrupt.
  *     @arg DMA1_IT_TE7: DMA1 Channel7 transfer error interrupt.
  *     @arg DMA2_IT_GL1: DMA2 Channel1 global interrupt.
  *     @arg DMA2_IT_TC1: DMA2 Channel1 transfer complete interrupt.
  *     @arg DMA2_IT_HT1: DMA2 Channel1 half transfer interrupt.
  *     @arg DMA2_IT_TE1: DMA2 Channel1 transfer error interrupt.
  *     @arg DMA2_IT_GL2: DMA2 Channel2 global interrupt.
  *     @arg DMA2_IT_TC2: DMA2 Channel2 transfer complete interrupt.
  *     @arg DMA2_IT_HT2: DMA2 Channel2 half transfer interrupt.
  *     @arg DMA2_IT_TE2: DMA2 Channel2 transfer error interrupt.
  *     @arg DMA2_IT_GL3: DMA2 Channel3 global interrupt.
  *     @arg DMA2_IT_TC3: DMA2 Channel3 transfer complete interrupt.
  *     @arg DMA2_IT_HT3: DMA2 Channel3 half transfer interrupt.
  *     @arg DMA2_IT_TE3: DMA2 Channel3 transfer error interrupt.
  *     @arg DMA2_IT_GL4: DMA2 Channel4 global interrupt.
  *     @arg DMA2_IT_TC4: DMA2 Channel4 transfer complete interrupt.
  *     @arg DMA2_IT_HT4: DMA2 Channel4 half transfer interrupt.
  *     @arg DMA2_IT_TE4: DMA2 Channel4 transfer error interrupt.
  *     @arg DMA2_IT_GL5: DMA2 Channel5 global interrupt.
  *     @arg DMA2_IT_TC5: DMA2 Channel5 transfer complete interrupt.
  *     @arg DMA2_IT_HT5: DMA2 Channel5 half transfer interrupt.
  *     @arg DMA2_IT_TE5: DMA2 Channel5 transfer error interrupt.
  * @retval None
  */
void DMA_ClearITPendingBit(uint32_t DMAy_IT)
{
  /* Check the parameters */
  assert_param(IS_DMA_CLEAR_IT(DMAy_IT));

  /* Calculate the used DMAy */
  if ((DMAy_IT & FLAG_Mask) != (uint32_t)RESET)
  {
    /* Clear the selected DMAy interrupt pending bits */
    DMA2->IFCR = DMAy_IT;
  }
  else
  {
    /* Clear the selected DMAy interrupt pending bits */
    DMA1->IFCR = DMAy_IT;
  }
}

//STD TIM

/**
  * @brief TIM
  */
typedef struct
{
  __IO uint16_t CR1;
  uint16_t  RESERVED0;
  __IO uint16_t CR2;
  uint16_t  RESERVED1;
  __IO uint16_t SMCR;
  uint16_t  RESERVED2;
  __IO uint16_t DIER;
  uint16_t  RESERVED3;
  __IO uint16_t SR;
  uint16_t  RESERVED4;
  __IO uint16_t EGR;
  uint16_t  RESERVED5;
  __IO uint16_t CCMR1;
  uint16_t  RESERVED6;
  __IO uint16_t CCMR2;
  uint16_t  RESERVED7;
  __IO uint16_t CCER;
  uint16_t  RESERVED8;
  __IO uint16_t CNT;
  uint16_t  RESERVED9;
  __IO uint16_t PSC;
  uint16_t  RESERVED10;
  __IO uint16_t ARR;
  uint16_t  RESERVED11;
  __IO uint16_t RCR;
  uint16_t  RESERVED12;
  __IO uint16_t CCR1;
  uint16_t  RESERVED13;
  __IO uint16_t CCR2;
  uint16_t  RESERVED14;
  __IO uint16_t CCR3;
  uint16_t  RESERVED15;
  __IO uint16_t CCR4;
  uint16_t  RESERVED16;
  __IO uint16_t BDTR;
  uint16_t  RESERVED17;
  __IO uint16_t DCR;
  uint16_t  RESERVED18;
  __IO uint16_t DMAR;
  uint16_t  RESERVED19;
} STD_TIM_TypeDef;


/**
  * @brief  TIM Time Base Init structure definition
  * @note   This structure is used with all TIMx except for TIM6 and TIM7.
  */

typedef struct
{
  uint16_t TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                       This parameter can be a number between 0x0000 and 0xFFFF */

  uint16_t TIM_CounterMode;       /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Counter_Mode */

  uint16_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFF.  */

  uint16_t TIM_ClockDivision;     /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Division_CKD */

  uint8_t TIM_RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                       reaches zero, an update event is generated and counting restarts
                                       from the RCR value (N).
                                       This means in PWM mode that (N+1) corresponds to:
                                          - the number of PWM periods in edge-aligned mode
                                          - the number of half PWM period in center-aligned mode
                                       This parameter must be a number between 0x00 and 0xFF.
                                       @note This parameter is valid only for TIM1 and TIM8. */
} STD_TIM_TimeBaseInitTypeDef;

/**
  * @brief  TIM Output Compare Init structure definition
  */

typedef struct
{
  uint16_t TIM_OCMode;        /*!< Specifies the TIM mode.
                                   This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  uint16_t TIM_OutputState;   /*!< Specifies the TIM Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_state */

  uint16_t TIM_OutputNState;  /*!< Specifies the TIM complementary Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_state
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint16_t TIM_Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
                                   This parameter can be a number between 0x0000 and 0xFFFF */

  uint16_t TIM_OCPolarity;    /*!< Specifies the output polarity.
                                   This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  uint16_t TIM_OCNPolarity;   /*!< Specifies the complementary output polarity.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint16_t TIM_OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                                   This parameter can be a value of @ref TIM_Output_Compare_Idle_State
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint16_t TIM_OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
                                   @note This parameter is valid only for TIM1 and TIM8. */
} STD_TIM_OCInitTypeDef;

/**
  * @brief  TIM Input Capture Init structure definition
  */

typedef struct
{

  uint16_t TIM_Channel;      /*!< Specifies the TIM channel.
                                  This parameter can be a value of @ref TIM_Channel */

  uint16_t TIM_ICPolarity;   /*!< Specifies the active edge of the input signal.
                                  This parameter can be a value of @ref TIM_Input_Capture_Polarity */

  uint16_t TIM_ICSelection;  /*!< Specifies the input.
                                  This parameter can be a value of @ref TIM_Input_Capture_Selection */

  uint16_t TIM_ICPrescaler;  /*!< Specifies the Input Capture Prescaler.
                                  This parameter can be a value of @ref TIM_Input_Capture_Prescaler */

  uint16_t TIM_ICFilter;     /*!< Specifies the input capture filter.
                                  This parameter can be a number between 0x0 and 0xF */
} STD_TIM_ICInitTypeDef;

/**
  * @brief  BDTR structure definition
  * @note   This structure is used only with TIM1 and TIM8.
  */

typedef struct
{

  uint16_t TIM_OSSRState;        /*!< Specifies the Off-State selection used in Run mode.
                                      This parameter can be a value of @ref OSSR_Off_State_Selection_for_Run_mode_state */

  uint16_t TIM_OSSIState;        /*!< Specifies the Off-State used in Idle state.
                                      This parameter can be a value of @ref OSSI_Off_State_Selection_for_Idle_mode_state */

  uint16_t TIM_LOCKLevel;        /*!< Specifies the LOCK level parameters.
                                      This parameter can be a value of @ref Lock_level */

  uint16_t TIM_DeadTime;         /*!< Specifies the delay time between the switching-off and the
                                      switching-on of the outputs.
                                      This parameter can be a number between 0x00 and 0xFF  */

  uint16_t TIM_Break;            /*!< Specifies whether the TIM Break input is enabled or not.
                                      This parameter can be a value of @ref Break_Input_enable_disable */

  uint16_t TIM_BreakPolarity;    /*!< Specifies the TIM Break Input pin polarity.
                                      This parameter can be a value of @ref Break_Polarity */

  uint16_t TIM_AutomaticOutput;  /*!< Specifies whether the TIM Automatic Output feature is enabled or not.
                                      This parameter can be a value of @ref TIM_AOE_Bit_Set_Reset */
} STD_TIM_BDTRInitTypeDef;

/** @defgroup TIM_Exported_constants
  * @{
  */

#define IS_TIM_ALL_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                   ((PERIPH) == TIM2) || \
                                   ((PERIPH) == TIM3) || \
                                   ((PERIPH) == TIM4) || \
                                   ((PERIPH) == TIM5) || \
                                   ((PERIPH) == TIM6) || \
                                   ((PERIPH) == TIM7) || \
                                   ((PERIPH) == TIM8) || \
                                   ((PERIPH) == TIM9) || \
                                   ((PERIPH) == TIM10)|| \
                                   ((PERIPH) == TIM11)|| \
                                   ((PERIPH) == TIM12)|| \
                                   ((PERIPH) == TIM13)|| \
                                   ((PERIPH) == TIM14)|| \
                                   ((PERIPH) == TIM15)|| \
                                   ((PERIPH) == TIM16)|| \
                                   ((PERIPH) == TIM17))

/* LIST1: TIM 1 and 8 */
#define IS_TIM_LIST1_PERIPH(PERIPH)  (((PERIPH) == TIM1) || \
                                      ((PERIPH) == TIM8))

/* LIST2: TIM 1, 8, 15 16 and 17 */
#define IS_TIM_LIST2_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM15)|| \
                                     ((PERIPH) == TIM16)|| \
                                     ((PERIPH) == TIM17))

/* LIST3: TIM 1, 2, 3, 4, 5 and 8 */
#define IS_TIM_LIST3_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8))

/* LIST4: TIM 1, 2, 3, 4, 5, 8, 15, 16 and 17 */
#define IS_TIM_LIST4_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM15)|| \
                                     ((PERIPH) == TIM16)|| \
                                     ((PERIPH) == TIM17))

/* LIST5: TIM 1, 2, 3, 4, 5, 8 and 15 */
#define IS_TIM_LIST5_PERIPH(PERIPH) (((PERIPH) == TIM1) || \
                                     ((PERIPH) == TIM2) || \
                                     ((PERIPH) == TIM3) || \
                                     ((PERIPH) == TIM4) || \
                                     ((PERIPH) == TIM5) || \
                                     ((PERIPH) == TIM8) || \
                                     ((PERIPH) == TIM15))

/* LIST6: TIM 1, 2, 3, 4, 5, 8, 9, 12 and 15 */
#define IS_TIM_LIST6_PERIPH(PERIPH)  (((PERIPH) == TIM1) || \
                                      ((PERIPH) == TIM2) || \
                                      ((PERIPH) == TIM3) || \
                                      ((PERIPH) == TIM4) || \
                                      ((PERIPH) == TIM5) || \
                                      ((PERIPH) == TIM8) || \
                                      ((PERIPH) == TIM9) || \
									  ((PERIPH) == TIM12)|| \
                                      ((PERIPH) == TIM15))

/* LIST7: TIM 1, 2, 3, 4, 5, 6, 7, 8, 9, 12 and 15 */
#define IS_TIM_LIST7_PERIPH(PERIPH)  (((PERIPH) == TIM1) || \
                                      ((PERIPH) == TIM2) || \
                                      ((PERIPH) == TIM3) || \
                                      ((PERIPH) == TIM4) || \
                                      ((PERIPH) == TIM5) || \
                                      ((PERIPH) == TIM6) || \
                                      ((PERIPH) == TIM7) || \
                                      ((PERIPH) == TIM8) || \
                                      ((PERIPH) == TIM9) || \
                                      ((PERIPH) == TIM12)|| \
                                      ((PERIPH) == TIM15))

/* LIST8: TIM 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15, 16 and 17 */
#define IS_TIM_LIST8_PERIPH(PERIPH)  (((PERIPH) == TIM1) || \
                                      ((PERIPH) == TIM2) || \
                                      ((PERIPH) == TIM3) || \
                                      ((PERIPH) == TIM4) || \
                                      ((PERIPH) == TIM5) || \
                                      ((PERIPH) == TIM8) || \
                                      ((PERIPH) == TIM9) || \
                                      ((PERIPH) == TIM10)|| \
                                      ((PERIPH) == TIM11)|| \
                                      ((PERIPH) == TIM12)|| \
                                      ((PERIPH) == TIM13)|| \
                                      ((PERIPH) == TIM14)|| \
                                      ((PERIPH) == TIM15)|| \
                                      ((PERIPH) == TIM16)|| \
                                      ((PERIPH) == TIM17))

/* LIST9: TIM 1, 2, 3, 4, 5, 6, 7, 8, 15, 16, and 17 */
#define IS_TIM_LIST9_PERIPH(PERIPH)  (((PERIPH) == TIM1) || \
                                      ((PERIPH) == TIM2) || \
                                      ((PERIPH) == TIM3) || \
                                      ((PERIPH) == TIM4) || \
                                      ((PERIPH) == TIM5) || \
                                      ((PERIPH) == TIM6) || \
                                      ((PERIPH) == TIM7) || \
                                      ((PERIPH) == TIM8) || \
                                      ((PERIPH) == TIM15)|| \
                                      ((PERIPH) == TIM16)|| \
                                      ((PERIPH) == TIM17))

/**
  * @}
  */

/** @defgroup TIM_Output_Compare_and_PWM_modes
  * @{
  */

#define TIM_OCMode_Timing                  ((uint16_t)0x0000)
#define TIM_OCMode_Active                  ((uint16_t)0x0010)
#define TIM_OCMode_Inactive                ((uint16_t)0x0020)
#define TIM_OCMode_Toggle                  ((uint16_t)0x0030)
#define TIM_OCMode_PWM1                    ((uint16_t)0x0060)
#define TIM_OCMode_PWM2                    ((uint16_t)0x0070)
#define IS_TIM_OC_MODE(MODE) (((MODE) == TIM_OCMode_Timing) || \
                              ((MODE) == TIM_OCMode_Active) || \
                              ((MODE) == TIM_OCMode_Inactive) || \
                              ((MODE) == TIM_OCMode_Toggle)|| \
                              ((MODE) == TIM_OCMode_PWM1) || \
                              ((MODE) == TIM_OCMode_PWM2))
#define IS_TIM_OCM(MODE) (((MODE) == TIM_OCMode_Timing) || \
                          ((MODE) == TIM_OCMode_Active) || \
                          ((MODE) == TIM_OCMode_Inactive) || \
                          ((MODE) == TIM_OCMode_Toggle)|| \
                          ((MODE) == TIM_OCMode_PWM1) || \
                          ((MODE) == TIM_OCMode_PWM2) ||	\
                          ((MODE) == TIM_ForcedAction_Active) || \
                          ((MODE) == TIM_ForcedAction_InActive))
/**
  * @}
  */

/** @defgroup TIM_One_Pulse_Mode
  * @{
  */

#define TIM_OPMode_Single                  ((uint16_t)0x0008)
#define TIM_OPMode_Repetitive              ((uint16_t)0x0000)
#define IS_TIM_OPM_MODE(MODE) (((MODE) == TIM_OPMode_Single) || \
                               ((MODE) == TIM_OPMode_Repetitive))
/**
  * @}
  */

/** @defgroup TIM_Channel
  * @{
  */

#define TIM_Channel_1                      ((uint16_t)0x0000)
#define TIM_Channel_2                      ((uint16_t)0x0004)
#define TIM_Channel_3                      ((uint16_t)0x0008)
#define TIM_Channel_4                      ((uint16_t)0x000C)
#define IS_TIM_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                 ((CHANNEL) == TIM_Channel_2) || \
                                 ((CHANNEL) == TIM_Channel_3) || \
                                 ((CHANNEL) == TIM_Channel_4))
#define IS_TIM_PWMI_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                      ((CHANNEL) == TIM_Channel_2))
#define IS_TIM_COMPLEMENTARY_CHANNEL(CHANNEL) (((CHANNEL) == TIM_Channel_1) || \
                                               ((CHANNEL) == TIM_Channel_2) || \
                                               ((CHANNEL) == TIM_Channel_3))
/**
  * @}
  */

/** @defgroup TIM_Clock_Division_CKD
  * @{
  */

#define TIM_CKD_DIV1                       ((uint16_t)0x0000)
#define TIM_CKD_DIV2                       ((uint16_t)0x0100)
#define TIM_CKD_DIV4                       ((uint16_t)0x0200)
#define IS_TIM_CKD_DIV(DIV) (((DIV) == TIM_CKD_DIV1) || \
                             ((DIV) == TIM_CKD_DIV2) || \
                             ((DIV) == TIM_CKD_DIV4))
/**
  * @}
  */

/** @defgroup TIM_Counter_Mode
  * @{
  */

#define TIM_CounterMode_Up                 ((uint16_t)0x0000)
#define TIM_CounterMode_Down               ((uint16_t)0x0010)
#define TIM_CounterMode_CenterAligned1     ((uint16_t)0x0020)
#define TIM_CounterMode_CenterAligned2     ((uint16_t)0x0040)
#define TIM_CounterMode_CenterAligned3     ((uint16_t)0x0060)
#define IS_TIM_COUNTER_MODE(MODE) (((MODE) == TIM_CounterMode_Up) ||  \
                                   ((MODE) == TIM_CounterMode_Down) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned1) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned2) || \
                                   ((MODE) == TIM_CounterMode_CenterAligned3))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Polarity
  * @{
  */

#define TIM_OCPolarity_High                ((uint16_t)0x0000)
#define TIM_OCPolarity_Low                 ((uint16_t)0x0002)
#define IS_TIM_OC_POLARITY(POLARITY) (((POLARITY) == TIM_OCPolarity_High) || \
                                      ((POLARITY) == TIM_OCPolarity_Low))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Polarity
  * @{
  */

#define TIM_OCNPolarity_High               ((uint16_t)0x0000)
#define TIM_OCNPolarity_Low                ((uint16_t)0x0008)
#define IS_TIM_OCN_POLARITY(POLARITY) (((POLARITY) == TIM_OCNPolarity_High) || \
                                       ((POLARITY) == TIM_OCNPolarity_Low))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_state
  * @{
  */

#define TIM_OutputState_Disable            ((uint16_t)0x0000)
#define TIM_OutputState_Enable             ((uint16_t)0x0001)
#define IS_TIM_OUTPUT_STATE(STATE) (((STATE) == TIM_OutputState_Disable) || \
                                    ((STATE) == TIM_OutputState_Enable))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_state
  * @{
  */

#define TIM_OutputNState_Disable           ((uint16_t)0x0000)
#define TIM_OutputNState_Enable            ((uint16_t)0x0004)
#define IS_TIM_OUTPUTN_STATE(STATE) (((STATE) == TIM_OutputNState_Disable) || \
                                     ((STATE) == TIM_OutputNState_Enable))
/**
  * @}
  */

/** @defgroup TIM_Capture_Compare_state
  * @{
  */

#define TIM_CCx_Enable                      ((uint16_t)0x0001)
#define TIM_CCx_Disable                     ((uint16_t)0x0000)
#define IS_TIM_CCX(CCX) (((CCX) == TIM_CCx_Enable) || \
                         ((CCX) == TIM_CCx_Disable))
/**
  * @}
  */

/** @defgroup TIM_Capture_Compare_N_state
  * @{
  */

#define TIM_CCxN_Enable                     ((uint16_t)0x0004)
#define TIM_CCxN_Disable                    ((uint16_t)0x0000)
#define IS_TIM_CCXN(CCXN) (((CCXN) == TIM_CCxN_Enable) || \
                           ((CCXN) == TIM_CCxN_Disable))
/**
  * @}
  */

/** @defgroup Break_Input_enable_disable
  * @{
  */

#define TIM_Break_Enable                   ((uint16_t)0x1000)
#define TIM_Break_Disable                  ((uint16_t)0x0000)
#define IS_TIM_BREAK_STATE(STATE) (((STATE) == TIM_Break_Enable) || \
                                   ((STATE) == TIM_Break_Disable))
/**
  * @}
  */

/** @defgroup Break_Polarity
  * @{
  */

#define TIM_BreakPolarity_Low              ((uint16_t)0x0000)
#define TIM_BreakPolarity_High             ((uint16_t)0x2000)
#define IS_TIM_BREAK_POLARITY(POLARITY) (((POLARITY) == TIM_BreakPolarity_Low) || \
                                         ((POLARITY) == TIM_BreakPolarity_High))
/**
  * @}
  */

/** @defgroup TIM_AOE_Bit_Set_Reset
  * @{
  */

#define TIM_AutomaticOutput_Enable         ((uint16_t)0x4000)
#define TIM_AutomaticOutput_Disable        ((uint16_t)0x0000)
#define IS_TIM_AUTOMATIC_OUTPUT_STATE(STATE) (((STATE) == TIM_AutomaticOutput_Enable) || \
                                              ((STATE) == TIM_AutomaticOutput_Disable))
/**
  * @}
  */

/** @defgroup Lock_level
  * @{
  */

#define TIM_LOCKLevel_OFF                  ((uint16_t)0x0000)
#define TIM_LOCKLevel_1                    ((uint16_t)0x0100)
#define TIM_LOCKLevel_2                    ((uint16_t)0x0200)
#define TIM_LOCKLevel_3                    ((uint16_t)0x0300)
#define IS_TIM_LOCK_LEVEL(LEVEL) (((LEVEL) == TIM_LOCKLevel_OFF) || \
                                  ((LEVEL) == TIM_LOCKLevel_1) || \
                                  ((LEVEL) == TIM_LOCKLevel_2) || \
                                  ((LEVEL) == TIM_LOCKLevel_3))
/**
  * @}
  */

/** @defgroup OSSI_Off_State_Selection_for_Idle_mode_state
  * @{
  */

#define TIM_OSSIState_Enable               ((uint16_t)0x0400)
#define TIM_OSSIState_Disable              ((uint16_t)0x0000)
#define IS_TIM_OSSI_STATE(STATE) (((STATE) == TIM_OSSIState_Enable) || \
                                  ((STATE) == TIM_OSSIState_Disable))
/**
  * @}
  */

/** @defgroup OSSR_Off_State_Selection_for_Run_mode_state
  * @{
  */

#define TIM_OSSRState_Enable               ((uint16_t)0x0800)
#define TIM_OSSRState_Disable              ((uint16_t)0x0000)
#define IS_TIM_OSSR_STATE(STATE) (((STATE) == TIM_OSSRState_Enable) || \
                                  ((STATE) == TIM_OSSRState_Disable))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Idle_State
  * @{
  */

#define TIM_OCIdleState_Set                ((uint16_t)0x0100)
#define TIM_OCIdleState_Reset              ((uint16_t)0x0000)
#define IS_TIM_OCIDLE_STATE(STATE) (((STATE) == TIM_OCIdleState_Set) || \
                                    ((STATE) == TIM_OCIdleState_Reset))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_N_Idle_State
  * @{
  */

#define TIM_OCNIdleState_Set               ((uint16_t)0x0200)
#define TIM_OCNIdleState_Reset             ((uint16_t)0x0000)
#define IS_TIM_OCNIDLE_STATE(STATE) (((STATE) == TIM_OCNIdleState_Set) || \
                                     ((STATE) == TIM_OCNIdleState_Reset))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Polarity
  * @{
  */

#define  TIM_ICPolarity_Rising             ((uint16_t)0x0000)
#define  TIM_ICPolarity_Falling            ((uint16_t)0x0002)
#define  TIM_ICPolarity_BothEdge           ((uint16_t)0x000A)
#define IS_TIM_IC_POLARITY(POLARITY) (((POLARITY) == TIM_ICPolarity_Rising) || \
                                      ((POLARITY) == TIM_ICPolarity_Falling))
#define IS_TIM_IC_POLARITY_LITE(POLARITY) (((POLARITY) == TIM_ICPolarity_Rising) || \
                                           ((POLARITY) == TIM_ICPolarity_Falling)|| \
                                           ((POLARITY) == TIM_ICPolarity_BothEdge))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Selection
  * @{
  */

#define TIM_ICSelection_DirectTI           ((uint16_t)0x0001) /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                   connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSelection_IndirectTI         ((uint16_t)0x0002) /*!< TIM Input 1, 2, 3 or 4 is selected to be
                                                                   connected to IC2, IC1, IC4 or IC3, respectively. */
#define TIM_ICSelection_TRC                ((uint16_t)0x0003) /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC. */
#define IS_TIM_IC_SELECTION(SELECTION) (((SELECTION) == TIM_ICSelection_DirectTI) || \
                                        ((SELECTION) == TIM_ICSelection_IndirectTI) || \
                                        ((SELECTION) == TIM_ICSelection_TRC))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Prescaler
  * @{
  */

#define TIM_ICPSC_DIV1                     ((uint16_t)0x0000) /*!< Capture performed each time an edge is detected on the capture input. */
#define TIM_ICPSC_DIV2                     ((uint16_t)0x0004) /*!< Capture performed once every 2 events. */
#define TIM_ICPSC_DIV4                     ((uint16_t)0x0008) /*!< Capture performed once every 4 events. */
#define TIM_ICPSC_DIV8                     ((uint16_t)0x000C) /*!< Capture performed once every 8 events. */
#define IS_TIM_IC_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ICPSC_DIV1) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV2) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV4) || \
                                        ((PRESCALER) == TIM_ICPSC_DIV8))
/**
  * @}
  */

/** @defgroup TIM_interrupt_sources
  * @{
  */

#define TIM_IT_Update                      ((uint16_t)0x0001)
#define TIM_IT_CC1                         ((uint16_t)0x0002)
#define TIM_IT_CC2                         ((uint16_t)0x0004)
#define TIM_IT_CC3                         ((uint16_t)0x0008)
#define TIM_IT_CC4                         ((uint16_t)0x0010)
#define TIM_IT_COM                         ((uint16_t)0x0020)
#define TIM_IT_Trigger                     ((uint16_t)0x0040)
#define TIM_IT_Break                       ((uint16_t)0x0080)
#define IS_TIM_IT(IT) ((((IT) & (uint16_t)0xFF00) == 0x0000) && ((IT) != 0x0000))

#define IS_TIM_GET_IT(IT) (((IT) == TIM_IT_Update) || \
                           ((IT) == TIM_IT_CC1) || \
                           ((IT) == TIM_IT_CC2) || \
                           ((IT) == TIM_IT_CC3) || \
                           ((IT) == TIM_IT_CC4) || \
                           ((IT) == TIM_IT_COM) || \
                           ((IT) == TIM_IT_Trigger) || \
                           ((IT) == TIM_IT_Break))
/**
  * @}
  */

/** @defgroup TIM_DMA_Base_address
  * @{
  */

#define TIM_DMABase_CR1                    ((uint16_t)0x0000)
#define TIM_DMABase_CR2                    ((uint16_t)0x0001)
#define TIM_DMABase_SMCR                   ((uint16_t)0x0002)
#define TIM_DMABase_DIER                   ((uint16_t)0x0003)
#define TIM_DMABase_SR                     ((uint16_t)0x0004)
#define TIM_DMABase_EGR                    ((uint16_t)0x0005)
#define TIM_DMABase_CCMR1                  ((uint16_t)0x0006)
#define TIM_DMABase_CCMR2                  ((uint16_t)0x0007)
#define TIM_DMABase_CCER                   ((uint16_t)0x0008)
#define TIM_DMABase_CNT                    ((uint16_t)0x0009)
#define TIM_DMABase_PSC                    ((uint16_t)0x000A)
#define TIM_DMABase_ARR                    ((uint16_t)0x000B)
#define TIM_DMABase_RCR                    ((uint16_t)0x000C)
#define TIM_DMABase_CCR1                   ((uint16_t)0x000D)
#define TIM_DMABase_CCR2                   ((uint16_t)0x000E)
#define TIM_DMABase_CCR3                   ((uint16_t)0x000F)
#define TIM_DMABase_CCR4                   ((uint16_t)0x0010)
#define TIM_DMABase_BDTR                   ((uint16_t)0x0011)
#define TIM_DMABase_DCR                    ((uint16_t)0x0012)
#define IS_TIM_DMA_BASE(BASE) (((BASE) == TIM_DMABase_CR1) || \
                               ((BASE) == TIM_DMABase_CR2) || \
                               ((BASE) == TIM_DMABase_SMCR) || \
                               ((BASE) == TIM_DMABase_DIER) || \
                               ((BASE) == TIM_DMABase_SR) || \
                               ((BASE) == TIM_DMABase_EGR) || \
                               ((BASE) == TIM_DMABase_CCMR1) || \
                               ((BASE) == TIM_DMABase_CCMR2) || \
                               ((BASE) == TIM_DMABase_CCER) || \
                               ((BASE) == TIM_DMABase_CNT) || \
                               ((BASE) == TIM_DMABase_PSC) || \
                               ((BASE) == TIM_DMABase_ARR) || \
                               ((BASE) == TIM_DMABase_RCR) || \
                               ((BASE) == TIM_DMABase_CCR1) || \
                               ((BASE) == TIM_DMABase_CCR2) || \
                               ((BASE) == TIM_DMABase_CCR3) || \
                               ((BASE) == TIM_DMABase_CCR4) || \
                               ((BASE) == TIM_DMABase_BDTR) || \
                               ((BASE) == TIM_DMABase_DCR))
/**
  * @}
  */

/** @defgroup TIM_DMA_Burst_Length
  * @{
  */

#define TIM_DMABurstLength_1Transfer           ((uint16_t)0x0000)
#define TIM_DMABurstLength_2Transfers          ((uint16_t)0x0100)
#define TIM_DMABurstLength_3Transfers          ((uint16_t)0x0200)
#define TIM_DMABurstLength_4Transfers          ((uint16_t)0x0300)
#define TIM_DMABurstLength_5Transfers          ((uint16_t)0x0400)
#define TIM_DMABurstLength_6Transfers          ((uint16_t)0x0500)
#define TIM_DMABurstLength_7Transfers          ((uint16_t)0x0600)
#define TIM_DMABurstLength_8Transfers          ((uint16_t)0x0700)
#define TIM_DMABurstLength_9Transfers          ((uint16_t)0x0800)
#define TIM_DMABurstLength_10Transfers         ((uint16_t)0x0900)
#define TIM_DMABurstLength_11Transfers         ((uint16_t)0x0A00)
#define TIM_DMABurstLength_12Transfers         ((uint16_t)0x0B00)
#define TIM_DMABurstLength_13Transfers         ((uint16_t)0x0C00)
#define TIM_DMABurstLength_14Transfers         ((uint16_t)0x0D00)
#define TIM_DMABurstLength_15Transfers         ((uint16_t)0x0E00)
#define TIM_DMABurstLength_16Transfers         ((uint16_t)0x0F00)
#define TIM_DMABurstLength_17Transfers         ((uint16_t)0x1000)
#define TIM_DMABurstLength_18Transfers         ((uint16_t)0x1100)
#define IS_TIM_DMA_LENGTH(LENGTH) (((LENGTH) == TIM_DMABurstLength_1Transfer) || \
                                   ((LENGTH) == TIM_DMABurstLength_2Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_3Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_4Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_5Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_6Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_7Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_8Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_9Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_10Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_11Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_12Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_13Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_14Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_15Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_16Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_17Transfers) || \
                                   ((LENGTH) == TIM_DMABurstLength_18Transfers))
/**
  * @}
  */

/** @defgroup TIM_DMA_sources
  * @{
  */

#define TIM_DMA_Update                     ((uint16_t)0x0100)
#define TIM_DMA_CC1                        ((uint16_t)0x0200)
#define TIM_DMA_CC2                        ((uint16_t)0x0400)
#define TIM_DMA_CC3                        ((uint16_t)0x0800)
#define TIM_DMA_CC4                        ((uint16_t)0x1000)
#define TIM_DMA_COM                        ((uint16_t)0x2000)
#define TIM_DMA_Trigger                    ((uint16_t)0x4000)
#define IS_TIM_DMA_SOURCE(SOURCE) ((((SOURCE) & (uint16_t)0x80FF) == 0x0000) && ((SOURCE) != 0x0000))

/**
  * @}
  */

/** @defgroup TIM_External_Trigger_Prescaler
  * @{
  */

#define TIM_ExtTRGPSC_OFF                  ((uint16_t)0x0000)
#define TIM_ExtTRGPSC_DIV2                 ((uint16_t)0x1000)
#define TIM_ExtTRGPSC_DIV4                 ((uint16_t)0x2000)
#define TIM_ExtTRGPSC_DIV8                 ((uint16_t)0x3000)
#define IS_TIM_EXT_PRESCALER(PRESCALER) (((PRESCALER) == TIM_ExtTRGPSC_OFF) || \
                                         ((PRESCALER) == TIM_ExtTRGPSC_DIV2) || \
                                         ((PRESCALER) == TIM_ExtTRGPSC_DIV4) || \
                                         ((PRESCALER) == TIM_ExtTRGPSC_DIV8))
/**
  * @}
  */

/** @defgroup TIM_Internal_Trigger_Selection
  * @{
  */

#define TIM_TS_ITR0                        ((uint16_t)0x0000)
#define TIM_TS_ITR1                        ((uint16_t)0x0010)
#define TIM_TS_ITR2                        ((uint16_t)0x0020)
#define TIM_TS_ITR3                        ((uint16_t)0x0030)
#define TIM_TS_TI1F_ED                     ((uint16_t)0x0040)
#define TIM_TS_TI1FP1                      ((uint16_t)0x0050)
#define TIM_TS_TI2FP2                      ((uint16_t)0x0060)
#define TIM_TS_ETRF                        ((uint16_t)0x0070)
#define IS_TIM_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                             ((SELECTION) == TIM_TS_ITR1) || \
                                             ((SELECTION) == TIM_TS_ITR2) || \
                                             ((SELECTION) == TIM_TS_ITR3) || \
                                             ((SELECTION) == TIM_TS_TI1F_ED) || \
                                             ((SELECTION) == TIM_TS_TI1FP1) || \
                                             ((SELECTION) == TIM_TS_TI2FP2) || \
                                             ((SELECTION) == TIM_TS_ETRF))
#define IS_TIM_INTERNAL_TRIGGER_SELECTION(SELECTION) (((SELECTION) == TIM_TS_ITR0) || \
                                                      ((SELECTION) == TIM_TS_ITR1) || \
                                                      ((SELECTION) == TIM_TS_ITR2) || \
                                                      ((SELECTION) == TIM_TS_ITR3))
/**
  * @}
  */

/** @defgroup TIM_TIx_External_Clock_Source
  * @{
  */

#define TIM_TIxExternalCLK1Source_TI1      ((uint16_t)0x0050)
#define TIM_TIxExternalCLK1Source_TI2      ((uint16_t)0x0060)
#define TIM_TIxExternalCLK1Source_TI1ED    ((uint16_t)0x0040)
#define IS_TIM_TIXCLK_SOURCE(SOURCE) (((SOURCE) == TIM_TIxExternalCLK1Source_TI1) || \
                                      ((SOURCE) == TIM_TIxExternalCLK1Source_TI2) || \
                                      ((SOURCE) == TIM_TIxExternalCLK1Source_TI1ED))
/**
  * @}
  */

/** @defgroup TIM_External_Trigger_Polarity
  * @{
  */
#define TIM_ExtTRGPolarity_Inverted        ((uint16_t)0x8000)
#define TIM_ExtTRGPolarity_NonInverted     ((uint16_t)0x0000)
#define IS_TIM_EXT_POLARITY(POLARITY) (((POLARITY) == TIM_ExtTRGPolarity_Inverted) || \
                                       ((POLARITY) == TIM_ExtTRGPolarity_NonInverted))
/**
  * @}
  */

/** @defgroup TIM_Prescaler_Reload_Mode
  * @{
  */

#define TIM_PSCReloadMode_Update           ((uint16_t)0x0000)
#define TIM_PSCReloadMode_Immediate        ((uint16_t)0x0001)
#define IS_TIM_PRESCALER_RELOAD(RELOAD) (((RELOAD) == TIM_PSCReloadMode_Update) || \
                                         ((RELOAD) == TIM_PSCReloadMode_Immediate))
/**
  * @}
  */

/** @defgroup TIM_Forced_Action
  * @{
  */

#define TIM_ForcedAction_Active            ((uint16_t)0x0050)
#define TIM_ForcedAction_InActive          ((uint16_t)0x0040)
#define IS_TIM_FORCED_ACTION(ACTION) (((ACTION) == TIM_ForcedAction_Active) || \
                                      ((ACTION) == TIM_ForcedAction_InActive))
/**
  * @}
  */

/** @defgroup TIM_Encoder_Mode
  * @{
  */

#define TIM_EncoderMode_TI1                ((uint16_t)0x0001)
#define TIM_EncoderMode_TI2                ((uint16_t)0x0002)
#define TIM_EncoderMode_TI12               ((uint16_t)0x0003)
#define IS_TIM_ENCODER_MODE(MODE) (((MODE) == TIM_EncoderMode_TI1) || \
                                   ((MODE) == TIM_EncoderMode_TI2) || \
                                   ((MODE) == TIM_EncoderMode_TI12))
/**
  * @}
  */


/** @defgroup TIM_Event_Source
  * @{
  */

#define TIM_EventSource_Update             ((uint16_t)0x0001)
#define TIM_EventSource_CC1                ((uint16_t)0x0002)
#define TIM_EventSource_CC2                ((uint16_t)0x0004)
#define TIM_EventSource_CC3                ((uint16_t)0x0008)
#define TIM_EventSource_CC4                ((uint16_t)0x0010)
#define TIM_EventSource_COM                ((uint16_t)0x0020)
#define TIM_EventSource_Trigger            ((uint16_t)0x0040)
#define TIM_EventSource_Break              ((uint16_t)0x0080)
#define IS_TIM_EVENT_SOURCE(SOURCE) ((((SOURCE) & (uint16_t)0xFF00) == 0x0000) && ((SOURCE) != 0x0000))

/**
  * @}
  */

/** @defgroup TIM_Update_Source
  * @{
  */

#define TIM_UpdateSource_Global            ((uint16_t)0x0000) /*!< Source of update is the counter overflow/underflow
                                                                   or the setting of UG bit, or an update generation
                                                                   through the slave mode controller. */
#define TIM_UpdateSource_Regular           ((uint16_t)0x0001) /*!< Source of update is counter overflow/underflow. */
#define IS_TIM_UPDATE_SOURCE(SOURCE) (((SOURCE) == TIM_UpdateSource_Global) || \
                                      ((SOURCE) == TIM_UpdateSource_Regular))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Preload_State
  * @{
  */

#define TIM_OCPreload_Enable               ((uint16_t)0x0008)
#define TIM_OCPreload_Disable              ((uint16_t)0x0000)
#define IS_TIM_OCPRELOAD_STATE(STATE) (((STATE) == TIM_OCPreload_Enable) || \
                                       ((STATE) == TIM_OCPreload_Disable))
/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Fast_State
  * @{
  */

#define TIM_OCFast_Enable                  ((uint16_t)0x0004)
#define TIM_OCFast_Disable                 ((uint16_t)0x0000)
#define IS_TIM_OCFAST_STATE(STATE) (((STATE) == TIM_OCFast_Enable) || \
                                    ((STATE) == TIM_OCFast_Disable))

/**
  * @}
  */

/** @defgroup TIM_Output_Compare_Clear_State
  * @{
  */

#define TIM_OCClear_Enable                 ((uint16_t)0x0080)
#define TIM_OCClear_Disable                ((uint16_t)0x0000)
#define IS_TIM_OCCLEAR_STATE(STATE) (((STATE) == TIM_OCClear_Enable) || \
                                     ((STATE) == TIM_OCClear_Disable))
/**
  * @}
  */

/** @defgroup TIM_Trigger_Output_Source
  * @{
  */

#define TIM_TRGOSource_Reset               ((uint16_t)0x0000)
#define TIM_TRGOSource_Enable              ((uint16_t)0x0010)
#define TIM_TRGOSource_Update              ((uint16_t)0x0020)
#define TIM_TRGOSource_OC1                 ((uint16_t)0x0030)
#define TIM_TRGOSource_OC1Ref              ((uint16_t)0x0040)
#define TIM_TRGOSource_OC2Ref              ((uint16_t)0x0050)
#define TIM_TRGOSource_OC3Ref              ((uint16_t)0x0060)
#define TIM_TRGOSource_OC4Ref              ((uint16_t)0x0070)
#define IS_TIM_TRGO_SOURCE(SOURCE) (((SOURCE) == TIM_TRGOSource_Reset) || \
                                    ((SOURCE) == TIM_TRGOSource_Enable) || \
                                    ((SOURCE) == TIM_TRGOSource_Update) || \
                                    ((SOURCE) == TIM_TRGOSource_OC1) || \
                                    ((SOURCE) == TIM_TRGOSource_OC1Ref) || \
                                    ((SOURCE) == TIM_TRGOSource_OC2Ref) || \
                                    ((SOURCE) == TIM_TRGOSource_OC3Ref) || \
                                    ((SOURCE) == TIM_TRGOSource_OC4Ref))
/**
  * @}
  */

/** @defgroup TIM_Slave_Mode
  * @{
  */

#define TIM_SlaveMode_Reset                ((uint16_t)0x0004)
#define TIM_SlaveMode_Gated                ((uint16_t)0x0005)
#define TIM_SlaveMode_Trigger              ((uint16_t)0x0006)
#define TIM_SlaveMode_External1            ((uint16_t)0x0007)
#define IS_TIM_SLAVE_MODE(MODE) (((MODE) == TIM_SlaveMode_Reset) || \
                                 ((MODE) == TIM_SlaveMode_Gated) || \
                                 ((MODE) == TIM_SlaveMode_Trigger) || \
                                 ((MODE) == TIM_SlaveMode_External1))
/**
  * @}
  */

/** @defgroup TIM_Master_Slave_Mode
  * @{
  */

#define TIM_MasterSlaveMode_Enable         ((uint16_t)0x0080)
#define TIM_MasterSlaveMode_Disable        ((uint16_t)0x0000)
#define IS_TIM_MSM_STATE(STATE) (((STATE) == TIM_MasterSlaveMode_Enable) || \
                                 ((STATE) == TIM_MasterSlaveMode_Disable))
/**
  * @}
  */

/** @defgroup TIM_Flags
  * @{
  */

#define TIM_FLAG_Update                    ((uint16_t)0x0001)
#define TIM_FLAG_CC1                       ((uint16_t)0x0002)
#define TIM_FLAG_CC2                       ((uint16_t)0x0004)
#define TIM_FLAG_CC3                       ((uint16_t)0x0008)
#define TIM_FLAG_CC4                       ((uint16_t)0x0010)
#define TIM_FLAG_COM                       ((uint16_t)0x0020)
#define TIM_FLAG_Trigger                   ((uint16_t)0x0040)
#define TIM_FLAG_Break                     ((uint16_t)0x0080)
#define TIM_FLAG_CC1OF                     ((uint16_t)0x0200)
#define TIM_FLAG_CC2OF                     ((uint16_t)0x0400)
#define TIM_FLAG_CC3OF                     ((uint16_t)0x0800)
#define TIM_FLAG_CC4OF                     ((uint16_t)0x1000)
#define IS_TIM_GET_FLAG(FLAG) (((FLAG) == TIM_FLAG_Update) || \
                               ((FLAG) == TIM_FLAG_CC1) || \
                               ((FLAG) == TIM_FLAG_CC2) || \
                               ((FLAG) == TIM_FLAG_CC3) || \
                               ((FLAG) == TIM_FLAG_CC4) || \
                               ((FLAG) == TIM_FLAG_COM) || \
                               ((FLAG) == TIM_FLAG_Trigger) || \
                               ((FLAG) == TIM_FLAG_Break) || \
                               ((FLAG) == TIM_FLAG_CC1OF) || \
                               ((FLAG) == TIM_FLAG_CC2OF) || \
                               ((FLAG) == TIM_FLAG_CC3OF) || \
                               ((FLAG) == TIM_FLAG_CC4OF))


#define IS_TIM_CLEAR_FLAG(TIM_FLAG) ((((TIM_FLAG) & (uint16_t)0xE100) == 0x0000) && ((TIM_FLAG) != 0x0000))
/**
  * @}
  */

/** @defgroup TIM_Input_Capture_Filer_Value
  * @{
  */

#define IS_TIM_IC_FILTER(ICFILTER) ((ICFILTER) <= 0xF)
/**
  * @}
  */

/** @defgroup TIM_External_Trigger_Filter
  * @{
  */

#define IS_TIM_EXT_FILTER(EXTFILTER) ((EXTFILTER) <= 0xF)
/**
  * @}
  */

/** @defgroup TIM_Legacy
  * @{
  */

#define TIM_DMABurstLength_1Byte           TIM_DMABurstLength_1Transfer
#define TIM_DMABurstLength_2Bytes          TIM_DMABurstLength_2Transfers
#define TIM_DMABurstLength_3Bytes          TIM_DMABurstLength_3Transfers
#define TIM_DMABurstLength_4Bytes          TIM_DMABurstLength_4Transfers
#define TIM_DMABurstLength_5Bytes          TIM_DMABurstLength_5Transfers
#define TIM_DMABurstLength_6Bytes          TIM_DMABurstLength_6Transfers
#define TIM_DMABurstLength_7Bytes          TIM_DMABurstLength_7Transfers
#define TIM_DMABurstLength_8Bytes          TIM_DMABurstLength_8Transfers
#define TIM_DMABurstLength_9Bytes          TIM_DMABurstLength_9Transfers
#define TIM_DMABurstLength_10Bytes         TIM_DMABurstLength_10Transfers
#define TIM_DMABurstLength_11Bytes         TIM_DMABurstLength_11Transfers
#define TIM_DMABurstLength_12Bytes         TIM_DMABurstLength_12Transfers
#define TIM_DMABurstLength_13Bytes         TIM_DMABurstLength_13Transfers
#define TIM_DMABurstLength_14Bytes         TIM_DMABurstLength_14Transfers
#define TIM_DMABurstLength_15Bytes         TIM_DMABurstLength_15Transfers
#define TIM_DMABurstLength_16Bytes         TIM_DMABurstLength_16Transfers
#define TIM_DMABurstLength_17Bytes         TIM_DMABurstLength_17Transfers
#define TIM_DMABurstLength_18Bytes         TIM_DMABurstLength_18Transfers

/** @defgroup TIM_Private_Defines
  * @{
  */

/* ---------------------- TIM registers bit mask ------------------------ */
#define SMCR_ETR_Mask               ((uint16_t)0x00FF)
#define CCMR_Offset                 ((uint16_t)0x0018)
#define CCER_CCE_Set                ((uint16_t)0x0001)
#define	CCER_CCNE_Set               ((uint16_t)0x0004)

static void TI1_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI2_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI3_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI4_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);


#define STD_TIM1_BASE             (APB2PERIPH_BASE + 0x2C00)
#define STD_TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define STD_TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define STD_TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define STD_TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define STD_TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define STD_TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
#define STD_TIM8_BASE             (APB2PERIPH_BASE + 0x3400)
#define STD_TIM9_BASE             (APB2PERIPH_BASE + 0x4C00)
#define STD_TIM10_BASE            (APB2PERIPH_BASE + 0x5000)
#define STD_TIM11_BASE            (APB2PERIPH_BASE + 0x5400)
#define STD_TIM12_BASE            (APB1PERIPH_BASE + 0x1800)
#define STD_TIM13_BASE            (APB1PERIPH_BASE + 0x1C00)
#define STD_TIM14_BASE            (APB1PERIPH_BASE + 0x2000)
#define STD_TIM15_BASE            (APB2PERIPH_BASE + 0x4000)
#define STD_TIM16_BASE            (APB2PERIPH_BASE + 0x4400)
#define STD_TIM17_BASE            (APB2PERIPH_BASE + 0x4800)


#define STD_TIM1				  ((STD_TIM_TypeDef *) STD_TIM1_BASE)
#define STD_TIM2				  ((STD_TIM_TypeDef *) STD_TIM2_BASE)
#define STD_TIM3				  ((STD_TIM_TypeDef *) STD_TIM3_BASE)
#define STD_TIM4				  ((STD_TIM_TypeDef *) STD_TIM4_BASE)
#define STD_TIM5				  ((STD_TIM_TypeDef *) STD_TIM5_BASE)
#define STD_TIM6				  ((STD_TIM_TypeDef *) STD_TIM6_BASE)
#define STD_TIM7				  ((STD_TIM_TypeDef *) STD_TIM7_BASE)
#define STD_TIM8				  ((STD_TIM_TypeDef *) STD_TIM8_BASE)
#define STD_TIM9				  ((STD_TIM_TypeDef *) STD_TIM9_BASE)
#define STD_TIM10				  ((STD_TIM_TypeDef *) STD_TIM10_BASE)
#define STD_TIM11				  ((STD_TIM_TypeDef *) STD_TIM11_BASE)
#define STD_TIM12				  ((STD_TIM_TypeDef *) STD_TIM12_BASE)
#define STD_TIM13				  ((STD_TIM_TypeDef *) STD_TIM13_BASE)
#define STD_TIM14				  ((STD_TIM_TypeDef *) STD_TIM14_BASE)
#define STD_TIM15               ((STD_TIM_TypeDef *) STD_TIM15_BASE)
#define STD_TIM16               ((STD_TIM_TypeDef *) STD_TIM16_BASE)
#define STD_TIM17               ((STD_TIM_TypeDef *) STD_TIM17_BASE)

void TIM_DeInit(STD_TIM_TypeDef* TIMx);
void TIM_TimeBaseInit(STD_TIM_TypeDef* TIMx, STD_TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_OC1Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_ICInit(STD_TIM_TypeDef* TIMx, STD_TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_PWMIConfig(STD_TIM_TypeDef* TIMx, STD_TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_BDTRConfig(STD_TIM_TypeDef* TIMx, STD_TIM_BDTRInitTypeDef *TIM_BDTRInitStruct);
void TIM_TimeBaseStructInit(STD_TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_OCStructInit(STD_TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_ICStructInit(STD_TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_BDTRStructInit(STD_TIM_BDTRInitTypeDef* TIM_BDTRInitStruct);
void TIM_Cmd(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_CtrlPWMOutputs(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_ITConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);
void TIM_GenerateEvent(STD_TIM_TypeDef* TIMx, uint16_t TIM_EventSource);
void TIM_DMAConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);
void TIM_DMACmd(STD_TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState);
void TIM_InternalClockConfig(STD_TIM_TypeDef* TIMx);
void TIM_ITRxExternalClockConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);
void TIM_TIxExternalClockConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter);
void TIM_ETRClockMode1Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter);
void TIM_ETRClockMode2Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);
void TIM_ETRConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                   uint16_t ExtTRGFilter);
void TIM_PrescalerConfig(STD_TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);
void TIM_CounterModeConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_CounterMode);
void TIM_SelectInputTrigger(STD_TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);
void TIM_EncoderInterfaceConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);
void TIM_ForcedOC1Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC2Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC3Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC4Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ARRPreloadConfig(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectCOM(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectCCDMA(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_CCPreloadControl(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_OC1PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC2PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC1FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC2FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC3FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC4FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_ClearOC1Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC2Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC3Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC4Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_OC1PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);
void TIM_OC1NPolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);
void TIM_OC2PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);
void TIM_OC2NPolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);
void TIM_OC3PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);
void TIM_OC3NPolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);
void TIM_OC4PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);
void TIM_CCxCmd(STD_TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx);
void TIM_CCxNCmd(STD_TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN);
void TIM_SelectOCxM(STD_TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode);
void TIM_UpdateDisableConfig(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_UpdateRequestConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource);
void TIM_SelectHallSensor(STD_TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_SelectOnePulseMode(STD_TIM_TypeDef* TIMx, uint16_t TIM_OPMode);
void TIM_SelectOutputTrigger(STD_TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);
void TIM_SelectSlaveMode(STD_TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode);
void TIM_SelectMasterSlaveMode(STD_TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode);
void TIM_SetCounter(STD_TIM_TypeDef* TIMx, uint16_t Counter);
void TIM_SetAutoreload(STD_TIM_TypeDef* TIMx, uint16_t Autoreload);
void TIM_SetCompare1(STD_TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(STD_TIM_TypeDef* TIMx, uint16_t Compare2);
void TIM_SetCompare3(STD_TIM_TypeDef* TIMx, uint16_t Compare3);
void TIM_SetCompare4(STD_TIM_TypeDef* TIMx, uint16_t Compare4);
void TIM_SetIC1Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC2Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC3Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC4Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetClockDivision(STD_TIM_TypeDef* TIMx, uint16_t TIM_CKD);
uint16_t TIM_GetCapture1(STD_TIM_TypeDef* TIMx);
uint16_t TIM_GetCapture2(STD_TIM_TypeDef* TIMx);
uint16_t TIM_GetCapture3(STD_TIM_TypeDef* TIMx);
uint16_t TIM_GetCapture4(STD_TIM_TypeDef* TIMx);
uint16_t TIM_GetCounter(STD_TIM_TypeDef* TIMx);
uint16_t TIM_GetPrescaler(STD_TIM_TypeDef* TIMx);
FlagStatus TIM_GetFlagStatus(STD_TIM_TypeDef* TIMx, uint16_t TIM_FLAG);
void TIM_ClearFlag(STD_TIM_TypeDef* TIMx, uint16_t TIM_FLAG);
ITStatus TIM_GetITStatus(STD_TIM_TypeDef* TIMx, uint16_t TIM_IT);
void TIM_ClearITPendingBit(STD_TIM_TypeDef* TIMx, uint16_t TIM_IT);


/**
  * @brief  Deinitializes the TIMx peripheral registers to their default reset values.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @retval None
  */
void TIM_DeInit(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));

  if (TIMx == STD_TIM1)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM1);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM1);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);
  }
  else if (TIMx == STD_TIM2)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM2);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM2);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);
  }
  else if (TIMx == STD_TIM3)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM3);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM3);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);
  }
  else if (TIMx == STD_TIM4)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM4);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM4);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE);
  }
  else if (TIMx == STD_TIM5)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM5);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM5);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5, DISABLE);
  }
  else if (TIMx == STD_TIM6)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM6);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM6);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, DISABLE);
  }
  else if (TIMx == STD_TIM7)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM7);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM7);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, DISABLE);
  }
  else if (TIMx == STD_TIM8)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM8);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM8);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, DISABLE);
  }
  else if (TIMx == STD_TIM9)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM9);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM9);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM9, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM9, DISABLE);
   }
  else if (TIMx == STD_TIM10)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM10);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM10);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM10, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM10, DISABLE);
  }
  else if (TIMx == STD_TIM11)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM11);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM11);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM11, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM11, DISABLE);
  }
  else if (TIMx == STD_TIM12)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM12);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM12);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM12, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM12, DISABLE);
  }
  else if (TIMx == STD_TIM13)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM13);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM13);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM13, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM13, DISABLE);
  }
  else if (TIMx == STD_TIM14)
  {
	  periph_clk_en(APB1,RCC_APB1Periph_TIM14);
	  periph_clk_dis(APB1,RCC_APB1Periph_TIM14);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM14, ENABLE);
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM14, DISABLE);
  }
  else if (TIMx == STD_TIM15)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM15);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM15);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM15, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM15, DISABLE);
  }
  else if (TIMx == STD_TIM16)
  {
	  periph_clk_en(APB2,RCC_APB2Periph_TIM16);
	  periph_clk_dis(APB2,RCC_APB2Periph_TIM16);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM16, ENABLE);
//    RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM16, DISABLE);
  }
  else
  {
    if (TIMx == STD_TIM17)
    {
  	  periph_clk_en(APB2,RCC_APB2Periph_TIM17);
  	  periph_clk_dis(APB2,RCC_APB2Periph_TIM17);
//      RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM17, ENABLE);
//      RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM17, DISABLE);
    }
  }
}

/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @retval None
  */
void TIM_TimeBaseInit(STD_TIM_TypeDef* TIMx, STD_TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  uint16_t tmpcr1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_COUNTER_MODE(TIM_TimeBaseInitStruct->TIM_CounterMode));
  assert_param(IS_TIM_CKD_DIV(TIM_TimeBaseInitStruct->TIM_ClockDivision));

  tmpcr1 = TIMx->CR1;

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8)|| (TIMx == STD_TIM2) || (TIMx == STD_TIM3)||
     (TIMx == STD_TIM4) || (TIMx == STD_TIM5))
  {
    /* Select the Counter Mode */
    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode;
  }

  if((TIMx != STD_TIM6) && (TIMx != STD_TIM7))
  {
    /* Set the clock division */
    tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CR1_CKD));
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_ClockDivision;
  }

  TIMx->CR1 = tmpcr1;

  /* Set the Autoreload value */
  TIMx->ARR = TIM_TimeBaseInitStruct->TIM_Period ;

  /* Set the Prescaler value */
  TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;

  if ((TIMx == STD_TIM1) || (TIMx == STD_TIM8)|| (TIMx == STD_TIM15)|| (TIMx == STD_TIM16) || (TIMx == STD_TIM17))
  {
    /* Set the Repetition Counter value */
    TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
  }

  /* Generate an update event to reload the Prescaler and the Repetition counter
     values immediately */
  TIMx->EGR = TIM_PSCReloadMode_Immediate;
}

/**
  * @brief  Initializes the TIMx Channel1 according to the specified
  *         parameters in the TIM_OCInitStruct.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC1Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
 /* Disable the Channel 1: Reset the CC1E Bit */
  TIMx->CCER &= (uint16_t)(~(uint16_t)TIM_CCER_CC1E);
  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMx->CCMR1;

  /* Reset the Output Compare Mode Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_OC1M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_CC1S));

  /* Select the Output Compare Mode */
  tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;

  /* Reset the Output Polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1P));
  /* Set the Output Compare Polarity */
  tmpccer |= TIM_OCInitStruct->TIM_OCPolarity;

  /* Set the Output State */
  tmpccer |= TIM_OCInitStruct->TIM_OutputState;

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8)|| (TIMx == STD_TIM15)||
     (TIMx == STD_TIM16)|| (TIMx == STD_TIM17))
  {
    assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
    assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

    /* Reset the Output N Polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1NP));
    /* Set the Output N Polarity */
    tmpccer |= TIM_OCInitStruct->TIM_OCNPolarity;

    /* Reset the Output N State */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC1NE));
    /* Set the Output N State */
    tmpccer |= TIM_OCInitStruct->TIM_OutputNState;

    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS1));
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS1N));

    /* Set the Output Idle state */
    tmpcr2 |= TIM_OCInitStruct->TIM_OCIdleState;
    /* Set the Output N Idle state */
    tmpcr2 |= TIM_OCInitStruct->TIM_OCNIdleState;
  }
  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR1 = TIM_OCInitStruct->TIM_Pulse;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel2 according to the specified
  *         parameters in the TIM_OCInitStruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select
  *         the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC2Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
   /* Disable the Channel 2: Reset the CC2E Bit */
  TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CCER_CC2E));

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMx->CCMR1;

  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_OC2M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR1_CC2S));

  /* Select the Output Compare Mode */
  tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8);

  /* Reset the Output Polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2P));
  /* Set the Output Compare Polarity */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 4);

  /* Set the Output State */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 4);

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8))
  {
    assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
    assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

    /* Reset the Output N Polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2NP));
    /* Set the Output N Polarity */
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 4);

    /* Reset the Output N State */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC2NE));
    /* Set the Output N State */
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 4);

    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS2));
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS2N));

    /* Set the Output Idle state */
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 2);
    /* Set the Output N Idle state */
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 2);
  }
  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR2 = TIM_OCInitStruct->TIM_Pulse;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel3 according to the specified
  *         parameters in the TIM_OCInitStruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC3Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
  /* Disable the Channel 2: Reset the CC2E Bit */
  TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CCER_CC3E));

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR2 register value */
  tmpccmrx = TIMx->CCMR2;

  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_OC3M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_CC3S));
  /* Select the Output Compare Mode */
  tmpccmrx |= TIM_OCInitStruct->TIM_OCMode;

  /* Reset the Output Polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3P));
  /* Set the Output Compare Polarity */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 8);

  /* Set the Output State */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 8);

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8))
  {
    assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
    assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
    assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

    /* Reset the Output N Polarity level */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3NP));
    /* Set the Output N Polarity */
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 8);
    /* Reset the Output N State */
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC3NE));

    /* Set the Output N State */
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 8);
    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS3));
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS3N));
    /* Set the Output Idle state */
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 4);
    /* Set the Output N Idle state */
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 4);
  }
  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR2 */
  TIMx->CCMR2 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR3 = TIM_OCInitStruct->TIM_Pulse;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIMx Channel4 according to the specified
  *         parameters in the TIM_OCInitStruct.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCInitStruct: pointer to a TIM_OCInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_OC4Init(STD_TIM_TypeDef* TIMx, STD_TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
  assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
  /* Disable the Channel 2: Reset the CC4E Bit */
  TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CCER_CC4E));

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR2 register value */
  tmpccmrx = TIMx->CCMR2;

  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_OC4M));
  tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CCMR2_CC4S));

  /* Select the Output Compare Mode */
  tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8);

  /* Reset the Output Polarity level */
  tmpccer &= (uint16_t)(~((uint16_t)TIM_CCER_CC4P));
  /* Set the Output Compare Polarity */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 12);

  /* Set the Output State */
  tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 12);

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8))
  {
    assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));
    /* Reset the Output Compare IDLE State */
    tmpcr2 &= (uint16_t)(~((uint16_t)TIM_CR2_OIS4));
    /* Set the Output Idle state */
    tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 6);
  }
  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR2 */
  TIMx->CCMR2 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR4 = TIM_OCInitStruct->TIM_Pulse;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Initializes the TIM peripheral according to the specified
  *         parameters in the TIM_ICInitStruct.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_ICInitStruct: pointer to a TIM_ICInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_ICInit(STD_TIM_TypeDef* TIMx, STD_TIM_ICInitTypeDef* TIM_ICInitStruct)
{
  /* Check the parameters */
  assert_param(IS_TIM_CHANNEL(TIM_ICInitStruct->TIM_Channel));
  assert_param(IS_TIM_IC_SELECTION(TIM_ICInitStruct->TIM_ICSelection));
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICInitStruct->TIM_ICPrescaler));
  assert_param(IS_TIM_IC_FILTER(TIM_ICInitStruct->TIM_ICFilter));

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8) || (TIMx == STD_TIM2) || (TIMx == STD_TIM3) ||
     (TIMx == STD_TIM4) ||(TIMx == STD_TIM5))
  {
    assert_param(IS_TIM_IC_POLARITY(TIM_ICInitStruct->TIM_ICPolarity));
  }
  else
  {
    assert_param(IS_TIM_IC_POLARITY_LITE(TIM_ICInitStruct->TIM_ICPolarity));
  }
  if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
  {
    assert_param(IS_TIM_LIST8_PERIPH(TIMx));
    /* TI1 Configuration */
    TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_2)
  {
    assert_param(IS_TIM_LIST6_PERIPH(TIMx));
    /* TI2 Configuration */
    TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_3)
  {
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    /* TI3 Configuration */
    TI3_Config(TIMx,  TIM_ICInitStruct->TIM_ICPolarity,
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC3Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else
  {
    assert_param(IS_TIM_LIST3_PERIPH(TIMx));
    /* TI4 Configuration */
    TI4_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
               TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC4Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
}

/**
  * @brief  Configures the TIM peripheral according to the specified
  *         parameters in the TIM_ICInitStruct to measure an external PWM signal.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_ICInitStruct: pointer to a TIM_ICInitTypeDef structure
  *         that contains the configuration information for the specified TIM peripheral.
  * @retval None
  */
void TIM_PWMIConfig(STD_TIM_TypeDef* TIMx, STD_TIM_ICInitTypeDef* TIM_ICInitStruct)
{
  uint16_t icoppositepolarity = TIM_ICPolarity_Rising;
  uint16_t icoppositeselection = TIM_ICSelection_DirectTI;
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* Select the Opposite Input Polarity */
  if (TIM_ICInitStruct->TIM_ICPolarity == TIM_ICPolarity_Rising)
  {
    icoppositepolarity = TIM_ICPolarity_Falling;
  }
  else
  {
    icoppositepolarity = TIM_ICPolarity_Rising;
  }
  /* Select the Opposite Input */
  if (TIM_ICInitStruct->TIM_ICSelection == TIM_ICSelection_DirectTI)
  {
    icoppositeselection = TIM_ICSelection_IndirectTI;
  }
  else
  {
    icoppositeselection = TIM_ICSelection_DirectTI;
  }
  if (TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
  {
    /* TI1 Configuration */
    TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity, TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    /* TI2 Configuration */
    TI2_Config(TIMx, icoppositepolarity, icoppositeselection, TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
  else
  {
    /* TI2 Configuration */
    TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity, TIM_ICInitStruct->TIM_ICSelection,
               TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    /* TI1 Configuration */
    TI1_Config(TIMx, icoppositepolarity, icoppositeselection, TIM_ICInitStruct->TIM_ICFilter);
    /* Set the Input Capture Prescaler value */
    TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
  }
}

/**
  * @brief  Configures the: Break feature, dead time, Lock level, the OSSI,
  *         the OSSR State and the AOE(automatic output enable).
  * @param  TIMx: where x can be  1 or 8 to select the TIM
  * @param  TIM_BDTRInitStruct: pointer to a TIM_BDTRInitTypeDef structure that
  *         contains the BDTR Register configuration  information for the TIM peripheral.
  * @retval None
  */
void TIM_BDTRConfig(STD_TIM_TypeDef* TIMx, STD_TIM_BDTRInitTypeDef *TIM_BDTRInitStruct)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_TIM_OSSR_STATE(TIM_BDTRInitStruct->TIM_OSSRState));
  assert_param(IS_TIM_OSSI_STATE(TIM_BDTRInitStruct->TIM_OSSIState));
  assert_param(IS_TIM_LOCK_LEVEL(TIM_BDTRInitStruct->TIM_LOCKLevel));
  assert_param(IS_TIM_BREAK_STATE(TIM_BDTRInitStruct->TIM_Break));
  assert_param(IS_TIM_BREAK_POLARITY(TIM_BDTRInitStruct->TIM_BreakPolarity));
  assert_param(IS_TIM_AUTOMATIC_OUTPUT_STATE(TIM_BDTRInitStruct->TIM_AutomaticOutput));
  /* Set the Lock level, the Break enable Bit and the Ploarity, the OSSR State,
     the OSSI State, the dead time value and the Automatic Output Enable Bit */
  TIMx->BDTR = (uint32_t)TIM_BDTRInitStruct->TIM_OSSRState | TIM_BDTRInitStruct->TIM_OSSIState |
             TIM_BDTRInitStruct->TIM_LOCKLevel | TIM_BDTRInitStruct->TIM_DeadTime |
             TIM_BDTRInitStruct->TIM_Break | TIM_BDTRInitStruct->TIM_BreakPolarity |
             TIM_BDTRInitStruct->TIM_AutomaticOutput;
}

/**
  * @brief  Fills each TIM_TimeBaseInitStruct member with its default value.
  * @param  TIM_TimeBaseInitStruct : pointer to a TIM_TimeBaseInitTypeDef
  *         structure which will be initialized.
  * @retval None
  */
void TIM_TimeBaseStructInit(STD_TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  /* Set the default configuration */
  TIM_TimeBaseInitStruct->TIM_Period = 0xFFFF;
  TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;
  TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000;
}

/**
  * @brief  Fills each TIM_OCInitStruct member with its default value.
  * @param  TIM_OCInitStruct : pointer to a TIM_OCInitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void TIM_OCStructInit(STD_TIM_OCInitTypeDef* TIM_OCInitStruct)
{
  /* Set the default configuration */
  TIM_OCInitStruct->TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStruct->TIM_OutputState = TIM_OutputState_Disable;
  TIM_OCInitStruct->TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStruct->TIM_Pulse = 0x0000;
  TIM_OCInitStruct->TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct->TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct->TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStruct->TIM_OCNIdleState = TIM_OCNIdleState_Reset;
}

/**
  * @brief  Fills each TIM_ICInitStruct member with its default value.
  * @param  TIM_ICInitStruct: pointer to a TIM_ICInitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void TIM_ICStructInit(STD_TIM_ICInitTypeDef* TIM_ICInitStruct)
{
  /* Set the default configuration */
  TIM_ICInitStruct->TIM_Channel = TIM_Channel_1;
  TIM_ICInitStruct->TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStruct->TIM_ICFilter = 0x00;
}

/**
  * @brief  Fills each TIM_BDTRInitStruct member with its default value.
  * @param  TIM_BDTRInitStruct: pointer to a TIM_BDTRInitTypeDef structure which
  *         will be initialized.
  * @retval None
  */
void TIM_BDTRStructInit(STD_TIM_BDTRInitTypeDef* TIM_BDTRInitStruct)
{
  /* Set the default configuration */
  TIM_BDTRInitStruct->TIM_OSSRState = TIM_OSSRState_Disable;
  TIM_BDTRInitStruct->TIM_OSSIState = TIM_OSSIState_Disable;
  TIM_BDTRInitStruct->TIM_LOCKLevel = TIM_LOCKLevel_OFF;
  TIM_BDTRInitStruct->TIM_DeadTime = 0x00;
  TIM_BDTRInitStruct->TIM_Break = TIM_Break_Disable;
  TIM_BDTRInitStruct->TIM_BreakPolarity = TIM_BreakPolarity_Low;
  TIM_BDTRInitStruct->TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
}

/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TIMx: where x can be 1 to 17 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_Cmd(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the TIM Counter */
    TIMx->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    /* Disable the TIM Counter */
    TIMx->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }
}

/**
  * @brief  Enables or disables the TIM peripheral Main Outputs.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the TIMx peripheral.
  * @param  NewState: new state of the TIM peripheral Main Outputs.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_CtrlPWMOutputs(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the TIM Main Output */
    TIMx->BDTR |= TIM_BDTR_MOE;
  }
  else
  {
    /* Disable the TIM Main Output */
    TIMx->BDTR &= (uint16_t)(~((uint16_t)TIM_BDTR_MOE));
  }
}

/**
  * @brief  Enables or disables the specified TIM interrupts.
  * @param  TIMx: where x can be 1 to 17 to select the TIMx peripheral.
  * @param  TIM_IT: specifies the TIM interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg TIM_IT_Update: TIM update Interrupt source
  *     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     @arg TIM_IT_COM: TIM Commutation Interrupt source
  *     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *     @arg TIM_IT_Break: TIM Break Interrupt source
  * @note
  *   - TIM6 and TIM7 can only generate an update interrupt.
  *   - TIM9, TIM12 and TIM15 can have only TIM_IT_Update, TIM_IT_CC1,
  *      TIM_IT_CC2 or TIM_IT_Trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.
  *   - TIM_IT_Break is used only with TIM1, TIM8 and TIM15.
  *   - TIM_IT_COM is used only with TIM1, TIM8, TIM15, TIM16 and TIM17.
  * @param  NewState: new state of the TIM interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_ITConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_IT(TIM_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIMx->DIER |= TIM_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIMx->DIER &= (uint16_t)~TIM_IT;
  }
}

/**
  * @brief  Configures the TIMx event to be generate by software.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_EventSource: specifies the event source.
  *   This parameter can be one or more of the following values:
  *     @arg TIM_EventSource_Update: Timer update Event source
  *     @arg TIM_EventSource_CC1: Timer Capture Compare 1 Event source
  *     @arg TIM_EventSource_CC2: Timer Capture Compare 2 Event source
  *     @arg TIM_EventSource_CC3: Timer Capture Compare 3 Event source
  *     @arg TIM_EventSource_CC4: Timer Capture Compare 4 Event source
  *     @arg TIM_EventSource_COM: Timer COM event source
  *     @arg TIM_EventSource_Trigger: Timer Trigger Event source
  *     @arg TIM_EventSource_Break: Timer Break event source
  * @note
  *   - TIM6 and TIM7 can only generate an update event.
  *   - TIM_EventSource_COM and TIM_EventSource_Break are used only with TIM1 and TIM8.
  * @retval None
  */
void TIM_GenerateEvent(STD_TIM_TypeDef* TIMx, uint16_t TIM_EventSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_EVENT_SOURCE(TIM_EventSource));

  /* Set the event sources */
  TIMx->EGR = TIM_EventSource;
}

/**
  * @brief  Configures the TIMx's DMA interface.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 15, 16 or 17 to select
  *   the TIM peripheral.
  * @param  TIM_DMABase: DMA Base address.
  *   This parameter can be one of the following values:
  *     @arg TIM_DMABase_CR, TIM_DMABase_CR2, TIM_DMABase_SMCR,
  *          TIM_DMABase_DIER, TIM1_DMABase_SR, TIM_DMABase_EGR,
  *          TIM_DMABase_CCMR1, TIM_DMABase_CCMR2, TIM_DMABase_CCER,
  *          TIM_DMABase_CNT, TIM_DMABase_PSC, TIM_DMABase_ARR,
  *          TIM_DMABase_RCR, TIM_DMABase_CCR1, TIM_DMABase_CCR2,
  *          TIM_DMABase_CCR3, TIM_DMABase_CCR4, TIM_DMABase_BDTR,
  *          TIM_DMABase_DCR.
  * @param  TIM_DMABurstLength: DMA Burst length.
  *   This parameter can be one value between:
  *   TIM_DMABurstLength_1Transfer and TIM_DMABurstLength_18Transfers.
  * @retval None
  */
void TIM_DMAConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));
  assert_param(IS_TIM_DMA_BASE(TIM_DMABase));
  assert_param(IS_TIM_DMA_LENGTH(TIM_DMABurstLength));
  /* Set the DMA Base and the DMA Burst Length */
  TIMx->DCR = TIM_DMABase | TIM_DMABurstLength;
}

/**
  * @brief  Enables or disables the TIMx's DMA Requests.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 6, 7, 8, 15, 16 or 17
  *   to select the TIM peripheral.
  * @param  TIM_DMASource: specifies the DMA Request sources.
  *   This parameter can be any combination of the following values:
  *     @arg TIM_DMA_Update: TIM update Interrupt source
  *     @arg TIM_DMA_CC1: TIM Capture Compare 1 DMA source
  *     @arg TIM_DMA_CC2: TIM Capture Compare 2 DMA source
  *     @arg TIM_DMA_CC3: TIM Capture Compare 3 DMA source
  *     @arg TIM_DMA_CC4: TIM Capture Compare 4 DMA source
  *     @arg TIM_DMA_COM: TIM Commutation DMA source
  *     @arg TIM_DMA_Trigger: TIM Trigger DMA source
  * @param  NewState: new state of the DMA Request sources.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_DMACmd(STD_TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST9_PERIPH(TIMx));
  assert_param(IS_TIM_DMA_SOURCE(TIM_DMASource));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the DMA sources */
    TIMx->DIER |= TIM_DMASource;
  }
  else
  {
    /* Disable the DMA sources */
    TIMx->DIER &= (uint16_t)~TIM_DMASource;
  }
}

/**
  * @brief  Configures the TIMx internal Clock
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15
  *         to select the TIM peripheral.
  * @retval None
  */
void TIM_InternalClockConfig(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* Disable slave mode to clock the prescaler directly with the internal clock */
  TIMx->SMCR &=  (uint16_t)(~((uint16_t)TIM_SMCR_SMS));
}

/**
  * @brief  Configures the TIMx Internal Trigger as External Clock
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_ITRSource: Trigger source.
  *   This parameter can be one of the following values:
  * @param  TIM_TS_ITR0: Internal Trigger 0
  * @param  TIM_TS_ITR1: Internal Trigger 1
  * @param  TIM_TS_ITR2: Internal Trigger 2
  * @param  TIM_TS_ITR3: Internal Trigger 3
  * @retval None
  */
void TIM_ITRxExternalClockConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_INTERNAL_TRIGGER_SELECTION(TIM_InputTriggerSource));
  /* Select the Internal Trigger */
  TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource);
  /* Select the External clock mode1 */
  TIMx->SMCR |= TIM_SlaveMode_External1;
}

/**
  * @brief  Configures the TIMx Trigger as External Clock
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_TIxExternalCLKSource: Trigger source.
  *   This parameter can be one of the following values:
  *     @arg TIM_TIxExternalCLK1Source_TI1ED: TI1 Edge Detector
  *     @arg TIM_TIxExternalCLK1Source_TI1: Filtered Timer Input 1
  *     @arg TIM_TIxExternalCLK1Source_TI2: Filtered Timer Input 2
  * @param  TIM_ICPolarity: specifies the TIx Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Rising
  *     @arg TIM_ICPolarity_Falling
  * @param  ICFilter : specifies the filter value.
  *   This parameter must be a value between 0x0 and 0xF.
  * @retval None
  */
void TIM_TIxExternalClockConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_TIXCLK_SOURCE(TIM_TIxExternalCLKSource));
  assert_param(IS_TIM_IC_POLARITY(TIM_ICPolarity));
  assert_param(IS_TIM_IC_FILTER(ICFilter));
  /* Configure the Timer Input Clock Source */
  if (TIM_TIxExternalCLKSource == TIM_TIxExternalCLK1Source_TI2)
  {
    TI2_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
  }
  else
  {
    TI1_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
  }
  /* Select the Trigger source */
  TIM_SelectInputTrigger(TIMx, TIM_TIxExternalCLKSource);
  /* Select the External clock mode1 */
  TIMx->SMCR |= TIM_SlaveMode_External1;
}

/**
  * @brief  Configures the External clock Mode1
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ExtTRGPrescaler: The external Trigger Prescaler.
  *   This parameter can be one of the following values:
  *     @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
  *     @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
  *     @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
  *     @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
  * @param  TIM_ExtTRGPolarity: The external Trigger Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
  *     @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
  * @param  ExtTRGFilter: External Trigger Filter.
  *   This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM_ETRClockMode1Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter)
{
  uint16_t tmpsmcr = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
  assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
  assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));
  /* Configure the ETR Clock source */
  TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);

  /* Get the TIMx SMCR register value */
  tmpsmcr = TIMx->SMCR;
  /* Reset the SMS Bits */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_SMS));
  /* Select the External clock mode1 */
  tmpsmcr |= TIM_SlaveMode_External1;
  /* Select the Trigger selection : ETRF */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_TS));
  tmpsmcr |= TIM_TS_ETRF;
  /* Write to TIMx SMCR */
  TIMx->SMCR = tmpsmcr;
}

/**
  * @brief  Configures the External clock Mode2
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ExtTRGPrescaler: The external Trigger Prescaler.
  *   This parameter can be one of the following values:
  *     @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
  *     @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
  *     @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
  *     @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
  * @param  TIM_ExtTRGPolarity: The external Trigger Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
  *     @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
  * @param  ExtTRGFilter: External Trigger Filter.
  *   This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM_ETRClockMode2Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler,
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
  assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
  assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));
  /* Configure the ETR Clock source */
  TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter);
  /* Enable the External clock mode2 */
  TIMx->SMCR |= TIM_SMCR_ECE;
}

/**
  * @brief  Configures the TIMx External Trigger (ETR).
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ExtTRGPrescaler: The external Trigger Prescaler.
  *   This parameter can be one of the following values:
  *     @arg TIM_ExtTRGPSC_OFF: ETRP Prescaler OFF.
  *     @arg TIM_ExtTRGPSC_DIV2: ETRP frequency divided by 2.
  *     @arg TIM_ExtTRGPSC_DIV4: ETRP frequency divided by 4.
  *     @arg TIM_ExtTRGPSC_DIV8: ETRP frequency divided by 8.
  * @param  TIM_ExtTRGPolarity: The external Trigger Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ExtTRGPolarity_Inverted: active low or falling edge active.
  *     @arg TIM_ExtTRGPolarity_NonInverted: active high or rising edge active.
  * @param  ExtTRGFilter: External Trigger Filter.
  *   This parameter must be a value between 0x00 and 0x0F
  * @retval None
  */
void TIM_ETRConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                   uint16_t ExtTRGFilter)
{
  uint16_t tmpsmcr = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_EXT_PRESCALER(TIM_ExtTRGPrescaler));
  assert_param(IS_TIM_EXT_POLARITY(TIM_ExtTRGPolarity));
  assert_param(IS_TIM_EXT_FILTER(ExtTRGFilter));
  tmpsmcr = TIMx->SMCR;
  /* Reset the ETR Bits */
  tmpsmcr &= SMCR_ETR_Mask;
  /* Set the Prescaler, the Filter value and the Polarity */
  tmpsmcr |= (uint16_t)(TIM_ExtTRGPrescaler | (uint16_t)(TIM_ExtTRGPolarity | (uint16_t)(ExtTRGFilter << (uint16_t)8)));
  /* Write to TIMx SMCR */
  TIMx->SMCR = tmpsmcr;
}

/**
  * @brief  Configures the TIMx Prescaler.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  Prescaler: specifies the Prescaler Register value
  * @param  TIM_PSCReloadMode: specifies the TIM Prescaler Reload mode
  *   This parameter can be one of the following values:
  *     @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
  *     @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediately.
  * @retval None
  */
void TIM_PrescalerConfig(STD_TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_PRESCALER_RELOAD(TIM_PSCReloadMode));
  /* Set the Prescaler value */
  TIMx->PSC = Prescaler;
  /* Set or reset the UG Bit */
  TIMx->EGR = TIM_PSCReloadMode;
}

/**
  * @brief  Specifies the TIMx Counter Mode to be used.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_CounterMode: specifies the Counter Mode to be used
  *   This parameter can be one of the following values:
  *     @arg TIM_CounterMode_Up: TIM Up Counting Mode
  *     @arg TIM_CounterMode_Down: TIM Down Counting Mode
  *     @arg TIM_CounterMode_CenterAligned1: TIM Center Aligned Mode1
  *     @arg TIM_CounterMode_CenterAligned2: TIM Center Aligned Mode2
  *     @arg TIM_CounterMode_CenterAligned3: TIM Center Aligned Mode3
  * @retval None
  */
void TIM_CounterModeConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_CounterMode)
{
  uint16_t tmpcr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_COUNTER_MODE(TIM_CounterMode));
  tmpcr1 = TIMx->CR1;
  /* Reset the CMS and DIR Bits */
  tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));
  /* Set the Counter Mode */
  tmpcr1 |= TIM_CounterMode;
  /* Write to TIMx CR1 register */
  TIMx->CR1 = tmpcr1;
}

/**
  * @brief  Selects the Input Trigger source
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_InputTriggerSource: The Input Trigger source.
  *   This parameter can be one of the following values:
  *     @arg TIM_TS_ITR0: Internal Trigger 0
  *     @arg TIM_TS_ITR1: Internal Trigger 1
  *     @arg TIM_TS_ITR2: Internal Trigger 2
  *     @arg TIM_TS_ITR3: Internal Trigger 3
  *     @arg TIM_TS_TI1F_ED: TI1 Edge Detector
  *     @arg TIM_TS_TI1FP1: Filtered Timer Input 1
  *     @arg TIM_TS_TI2FP2: Filtered Timer Input 2
  *     @arg TIM_TS_ETRF: External Trigger input
  * @retval None
  */
void TIM_SelectInputTrigger(STD_TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource)
{
  uint16_t tmpsmcr = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_TRIGGER_SELECTION(TIM_InputTriggerSource));
  /* Get the TIMx SMCR register value */
  tmpsmcr = TIMx->SMCR;
  /* Reset the TS Bits */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_TS));
  /* Set the Input Trigger source */
  tmpsmcr |= TIM_InputTriggerSource;
  /* Write to TIMx SMCR */
  TIMx->SMCR = tmpsmcr;
}

/**
  * @brief  Configures the TIMx Encoder Interface.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_EncoderMode: specifies the TIMx Encoder Mode.
  *   This parameter can be one of the following values:
  *     @arg TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
  *     @arg TIM_EncoderMode_TI2: Counter counts on TI2FP2 edge depending on TI1FP1 level.
  *     @arg TIM_EncoderMode_TI12: Counter counts on both TI1FP1 and TI2FP2 edges depending
  *                                on the level of the other input.
  * @param  TIM_IC1Polarity: specifies the IC1 Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Falling: IC Falling edge.
  *     @arg TIM_ICPolarity_Rising: IC Rising edge.
  * @param  TIM_IC2Polarity: specifies the IC2 Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Falling: IC Falling edge.
  *     @arg TIM_ICPolarity_Rising: IC Rising edge.
  * @retval None
  */
void TIM_EncoderInterfaceConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity)
{
  uint16_t tmpsmcr = 0;
  uint16_t tmpccmr1 = 0;
  uint16_t tmpccer = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST5_PERIPH(TIMx));
  assert_param(IS_TIM_ENCODER_MODE(TIM_EncoderMode));
  assert_param(IS_TIM_IC_POLARITY(TIM_IC1Polarity));
  assert_param(IS_TIM_IC_POLARITY(TIM_IC2Polarity));

  /* Get the TIMx SMCR register value */
  tmpsmcr = TIMx->SMCR;

  /* Get the TIMx CCMR1 register value */
  tmpccmr1 = TIMx->CCMR1;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;

  /* Set the encoder Mode */
  tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMCR_SMS));
  tmpsmcr |= TIM_EncoderMode;

  /* Select the Capture Compare 1 and the Capture Compare 2 as input */
  tmpccmr1 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR1_CC1S)) & (uint16_t)(~((uint16_t)TIM_CCMR1_CC2S)));
  tmpccmr1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

  /* Set the TI1 and the TI2 Polarities */
  tmpccer &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCER_CC1P)) & ((uint16_t)~((uint16_t)TIM_CCER_CC2P)));
  tmpccer |= (uint16_t)(TIM_IC1Polarity | (uint16_t)(TIM_IC2Polarity << (uint16_t)4));

  /* Write to TIMx SMCR */
  TIMx->SMCR = tmpsmcr;
  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmr1;
  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Forces the TIMx output 1 waveform to active or inactive level.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_ForcedAction: specifies the forced Action to be set to the output waveform.
  *   This parameter can be one of the following values:
  *     @arg TIM_ForcedAction_Active: Force active level on OC1REF
  *     @arg TIM_ForcedAction_InActive: Force inactive level on OC1REF.
  * @retval None
  */
void TIM_ForcedOC1Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC1M Bits */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1M);
  /* Configure The Forced output Mode */
  tmpccmr1 |= TIM_ForcedAction;
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Forces the TIMx output 2 waveform to active or inactive level.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_ForcedAction: specifies the forced Action to be set to the output waveform.
  *   This parameter can be one of the following values:
  *     @arg TIM_ForcedAction_Active: Force active level on OC2REF
  *     @arg TIM_ForcedAction_InActive: Force inactive level on OC2REF.
  * @retval None
  */
void TIM_ForcedOC2Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC2M Bits */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2M);
  /* Configure The Forced output Mode */
  tmpccmr1 |= (uint16_t)(TIM_ForcedAction << 8);
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Forces the TIMx output 3 waveform to active or inactive level.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ForcedAction: specifies the forced Action to be set to the output waveform.
  *   This parameter can be one of the following values:
  *     @arg TIM_ForcedAction_Active: Force active level on OC3REF
  *     @arg TIM_ForcedAction_InActive: Force inactive level on OC3REF.
  * @retval None
  */
void TIM_ForcedOC3Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC1M Bits */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3M);
  /* Configure The Forced output Mode */
  tmpccmr2 |= TIM_ForcedAction;
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Forces the TIMx output 4 waveform to active or inactive level.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ForcedAction: specifies the forced Action to be set to the output waveform.
  *   This parameter can be one of the following values:
  *     @arg TIM_ForcedAction_Active: Force active level on OC4REF
  *     @arg TIM_ForcedAction_InActive: Force inactive level on OC4REF.
  * @retval None
  */
void TIM_ForcedOC4Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_FORCED_ACTION(TIM_ForcedAction));
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC2M Bits */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4M);
  /* Configure The Forced output Mode */
  tmpccmr2 |= (uint16_t)(TIM_ForcedAction << 8);
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables TIMx peripheral Preload register on ARR.
  * @param  TIMx: where x can be  1 to 17 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx peripheral Preload register
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_ARRPreloadConfig(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the ARR Preload Bit */
    TIMx->CR1 |= TIM_CR1_ARPE;
  }
  else
  {
    /* Reset the ARR Preload Bit */
    TIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_ARPE);
  }
}

/**
  * @brief  Selects the TIM peripheral Commutation event.
  * @param  TIMx: where x can be  1, 8, 15, 16 or 17 to select the TIMx peripheral
  * @param  NewState: new state of the Commutation event.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_SelectCOM(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the COM Bit */
    TIMx->CR2 |= TIM_CR2_CCUS;
  }
  else
  {
    /* Reset the COM Bit */
    TIMx->CR2 &= (uint16_t)~((uint16_t)TIM_CR2_CCUS);
  }
}

/**
  * @brief  Selects the TIMx peripheral Capture Compare DMA source.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 15, 16 or 17 to select
  *         the TIM peripheral.
  * @param  NewState: new state of the Capture Compare DMA source
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_SelectCCDMA(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST4_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the CCDS Bit */
    TIMx->CR2 |= TIM_CR2_CCDS;
  }
  else
  {
    /* Reset the CCDS Bit */
    TIMx->CR2 &= (uint16_t)~((uint16_t)TIM_CR2_CCDS);
  }
}

/**
  * @brief  Sets or Resets the TIM peripheral Capture Compare Preload Control bit.
  * @param  TIMx: where x can be   1, 2, 3, 4, 5, 8 or 15
  *         to select the TIMx peripheral
  * @param  NewState: new state of the Capture Compare Preload Control bit
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_CCPreloadControl(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST5_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the CCPC Bit */
    TIMx->CR2 |= TIM_CR2_CCPC;
  }
  else
  {
    /* Reset the CCPC Bit */
    TIMx->CR2 &= (uint16_t)~((uint16_t)TIM_CR2_CCPC);
  }
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR1.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC1PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC1PE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr1 |= TIM_OCPreload;
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR2.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select
  *         the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC2PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC2PE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr1 |= (uint16_t)(TIM_OCPreload << 8);
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR3.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC3PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC3PE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr2 |= TIM_OCPreload;
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR4.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC4PreloadConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC4PE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr2 |= (uint16_t)(TIM_OCPreload << 8);
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Configures the TIMx Output Compare 1 Fast feature.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_OCFast: new state of the Output Compare Fast Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCFast_Enable: TIM output compare fast enable
  *     @arg TIM_OCFast_Disable: TIM output compare fast disable
  * @retval None
  */
void TIM_OC1FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));
  /* Get the TIMx CCMR1 register value */
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC1FE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1FE);
  /* Enable or Disable the Output Compare Fast Bit */
  tmpccmr1 |= TIM_OCFast;
  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Configures the TIMx Output Compare 2 Fast feature.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select
  *         the TIM peripheral.
  * @param  TIM_OCFast: new state of the Output Compare Fast Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCFast_Enable: TIM output compare fast enable
  *     @arg TIM_OCFast_Disable: TIM output compare fast disable
  * @retval None
  */
void TIM_OC2FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));
  /* Get the TIMx CCMR1 register value */
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC2FE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2FE);
  /* Enable or Disable the Output Compare Fast Bit */
  tmpccmr1 |= (uint16_t)(TIM_OCFast << 8);
  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Configures the TIMx Output Compare 3 Fast feature.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCFast: new state of the Output Compare Fast Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCFast_Enable: TIM output compare fast enable
  *     @arg TIM_OCFast_Disable: TIM output compare fast disable
  * @retval None
  */
void TIM_OC3FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));
  /* Get the TIMx CCMR2 register value */
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC3FE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3FE);
  /* Enable or Disable the Output Compare Fast Bit */
  tmpccmr2 |= TIM_OCFast;
  /* Write to TIMx CCMR2 */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Configures the TIMx Output Compare 4 Fast feature.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCFast: new state of the Output Compare Fast Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCFast_Enable: TIM output compare fast enable
  *     @arg TIM_OCFast_Disable: TIM output compare fast disable
  * @retval None
  */
void TIM_OC4FastConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCFast)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCFAST_STATE(TIM_OCFast));
  /* Get the TIMx CCMR2 register value */
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC4FE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4FE);
  /* Enable or Disable the Output Compare Fast Bit */
  tmpccmr2 |= (uint16_t)(TIM_OCFast << 8);
  /* Write to TIMx CCMR2 */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Clears or safeguards the OCREF1 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCClear: new state of the Output Compare Clear Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCClear_Enable: TIM Output clear enable
  *     @arg TIM_OCClear_Disable: TIM Output clear disable
  * @retval None
  */
void TIM_ClearOC1Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));

  tmpccmr1 = TIMx->CCMR1;

  /* Reset the OC1CE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1CE);
  /* Enable or Disable the Output Compare Clear Bit */
  tmpccmr1 |= TIM_OCClear;
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Clears or safeguards the OCREF2 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCClear: new state of the Output Compare Clear Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCClear_Enable: TIM Output clear enable
  *     @arg TIM_OCClear_Disable: TIM Output clear disable
  * @retval None
  */
void TIM_ClearOC2Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr1 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));
  tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC2CE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2CE);
  /* Enable or Disable the Output Compare Clear Bit */
  tmpccmr1 |= (uint16_t)(TIM_OCClear << 8);
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Clears or safeguards the OCREF3 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCClear: new state of the Output Compare Clear Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCClear_Enable: TIM Output clear enable
  *     @arg TIM_OCClear_Disable: TIM Output clear disable
  * @retval None
  */
void TIM_ClearOC3Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC3CE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3CE);
  /* Enable or Disable the Output Compare Clear Bit */
  tmpccmr2 |= TIM_OCClear;
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Clears or safeguards the OCREF4 signal on an external event
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCClear: new state of the Output Compare Clear Enable Bit.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCClear_Enable: TIM Output clear enable
  *     @arg TIM_OCClear_Disable: TIM Output clear disable
  * @retval None
  */
void TIM_ClearOC4Ref(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCClear)
{
  uint16_t tmpccmr2 = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OCCLEAR_STATE(TIM_OCClear));
  tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC4CE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4CE);
  /* Enable or Disable the Output Compare Clear Bit */
  tmpccmr2 |= (uint16_t)(TIM_OCClear << 8);
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Configures the TIMx channel 1 polarity.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_OCPolarity: specifies the OC1 Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPolarity_High: Output Compare active high
  *     @arg TIM_OCPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC1PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity));
  tmpccer = TIMx->CCER;
  /* Set or Reset the CC1P Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC1P);
  tmpccer |= TIM_OCPolarity;
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configures the TIMx Channel 1N polarity.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the TIM peripheral.
  * @param  TIM_OCNPolarity: specifies the OC1N Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCNPolarity_High: Output Compare active high
  *     @arg TIM_OCNPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC1NPolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity)
{
  uint16_t tmpccer = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_TIM_OCN_POLARITY(TIM_OCNPolarity));

  tmpccer = TIMx->CCER;
  /* Set or Reset the CC1NP Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC1NP);
  tmpccer |= TIM_OCNPolarity;
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configures the TIMx channel 2 polarity.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_OCPolarity: specifies the OC2 Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPolarity_High: Output Compare active high
  *     @arg TIM_OCPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC2PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity));
  tmpccer = TIMx->CCER;
  /* Set or Reset the CC2P Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC2P);
  tmpccer |= (uint16_t)(TIM_OCPolarity << 4);
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configures the TIMx Channel 2N polarity.
  * @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
  * @param  TIM_OCNPolarity: specifies the OC2N Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCNPolarity_High: Output Compare active high
  *     @arg TIM_OCNPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC2NPolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity)
{
  uint16_t tmpccer = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));
  assert_param(IS_TIM_OCN_POLARITY(TIM_OCNPolarity));

  tmpccer = TIMx->CCER;
  /* Set or Reset the CC2NP Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC2NP);
  tmpccer |= (uint16_t)(TIM_OCNPolarity << 4);
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configures the TIMx channel 3 polarity.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCPolarity: specifies the OC3 Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPolarity_High: Output Compare active high
  *     @arg TIM_OCPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC3PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity));
  tmpccer = TIMx->CCER;
  /* Set or Reset the CC3P Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC3P);
  tmpccer |= (uint16_t)(TIM_OCPolarity << 8);
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configures the TIMx Channel 3N polarity.
  * @param  TIMx: where x can be 1 or 8 to select the TIM peripheral.
  * @param  TIM_OCNPolarity: specifies the OC3N Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCNPolarity_High: Output Compare active high
  *     @arg TIM_OCNPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC3NPolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity)
{
  uint16_t tmpccer = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST1_PERIPH(TIMx));
  assert_param(IS_TIM_OCN_POLARITY(TIM_OCNPolarity));

  tmpccer = TIMx->CCER;
  /* Set or Reset the CC3NP Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC3NP);
  tmpccer |= (uint16_t)(TIM_OCNPolarity << 8);
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configures the TIMx channel 4 polarity.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCPolarity: specifies the OC4 Polarity
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPolarity_High: Output Compare active high
  *     @arg TIM_OCPolarity_Low: Output Compare active low
  * @retval None
  */
void TIM_OC4PolarityConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity)
{
  uint16_t tmpccer = 0;
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_OC_POLARITY(TIM_OCPolarity));
  tmpccer = TIMx->CCER;
  /* Set or Reset the CC4P Bit */
  tmpccer &= (uint16_t)~((uint16_t)TIM_CCER_CC4P);
  tmpccer |= (uint16_t)(TIM_OCPolarity << 12);
  /* Write to TIMx CCER register */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Enables or disables the TIM Capture Compare Channel x.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_Channel: specifies the TIM Channel
  *   This parameter can be one of the following values:
  *     @arg TIM_Channel_1: TIM Channel 1
  *     @arg TIM_Channel_2: TIM Channel 2
  *     @arg TIM_Channel_3: TIM Channel 3
  *     @arg TIM_Channel_4: TIM Channel 4
  * @param  TIM_CCx: specifies the TIM Channel CCxE bit new state.
  *   This parameter can be: TIM_CCx_Enable or TIM_CCx_Disable.
  * @retval None
  */
void TIM_CCxCmd(STD_TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx)
{
  uint16_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_CHANNEL(TIM_Channel));
  assert_param(IS_TIM_CCX(TIM_CCx));

  tmp = CCER_CCE_Set << TIM_Channel;

  /* Reset the CCxE Bit */
  TIMx->CCER &= (uint16_t)~ tmp;

  /* Set or reset the CCxE Bit */
  TIMx->CCER |=  (uint16_t)(TIM_CCx << TIM_Channel);
}

/**
  * @brief  Enables or disables the TIM Capture Compare Channel xN.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the TIM peripheral.
  * @param  TIM_Channel: specifies the TIM Channel
  *   This parameter can be one of the following values:
  *     @arg TIM_Channel_1: TIM Channel 1
  *     @arg TIM_Channel_2: TIM Channel 2
  *     @arg TIM_Channel_3: TIM Channel 3
  * @param  TIM_CCxN: specifies the TIM Channel CCxNE bit new state.
  *   This parameter can be: TIM_CCxN_Enable or TIM_CCxN_Disable.
  * @retval None
  */
void TIM_CCxNCmd(STD_TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN)
{
  uint16_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST2_PERIPH(TIMx));
  assert_param(IS_TIM_COMPLEMENTARY_CHANNEL(TIM_Channel));
  assert_param(IS_TIM_CCXN(TIM_CCxN));

  tmp = CCER_CCNE_Set << TIM_Channel;

  /* Reset the CCxNE Bit */
  TIMx->CCER &= (uint16_t) ~tmp;

  /* Set or reset the CCxNE Bit */
  TIMx->CCER |=  (uint16_t)(TIM_CCxN << TIM_Channel);
}

/**
  * @brief  Selects the TIM Output Compare Mode.
  * @note   This function disables the selected channel before changing the Output
  *         Compare Mode.
  *         User has to enable this channel using TIM_CCxCmd and TIM_CCxNCmd functions.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_Channel: specifies the TIM Channel
  *   This parameter can be one of the following values:
  *     @arg TIM_Channel_1: TIM Channel 1
  *     @arg TIM_Channel_2: TIM Channel 2
  *     @arg TIM_Channel_3: TIM Channel 3
  *     @arg TIM_Channel_4: TIM Channel 4
  * @param  TIM_OCMode: specifies the TIM Output Compare Mode.
  *   This parameter can be one of the following values:
  *     @arg TIM_OCMode_Timing
  *     @arg TIM_OCMode_Active
  *     @arg TIM_OCMode_Toggle
  *     @arg TIM_OCMode_PWM1
  *     @arg TIM_OCMode_PWM2
  *     @arg TIM_ForcedAction_Active
  *     @arg TIM_ForcedAction_InActive
  * @retval None
  */
void TIM_SelectOCxM(STD_TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode)
{
  uint32_t tmp = 0;
  uint16_t tmp1 = 0;

  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_CHANNEL(TIM_Channel));
  assert_param(IS_TIM_OCM(TIM_OCMode));

  tmp = (uint32_t) TIMx;
  tmp += CCMR_Offset;

  tmp1 = CCER_CCE_Set << (uint16_t)TIM_Channel;

  /* Disable the Channel: Reset the CCxE Bit */
  TIMx->CCER &= (uint16_t) ~tmp1;

  if((TIM_Channel == TIM_Channel_1) ||(TIM_Channel == TIM_Channel_3))
  {
    tmp += (TIM_Channel>>1);

    /* Reset the OCxM bits in the CCMRx register */
    *(__IO uint32_t *) tmp &= (uint32_t)~((uint32_t)TIM_CCMR1_OC1M);

    /* Configure the OCxM bits in the CCMRx register */
    *(__IO uint32_t *) tmp |= TIM_OCMode;
  }
  else
  {
    tmp += (uint16_t)(TIM_Channel - (uint16_t)4)>> (uint16_t)1;

    /* Reset the OCxM bits in the CCMRx register */
    *(__IO uint32_t *) tmp &= (uint32_t)~((uint32_t)TIM_CCMR1_OC2M);

    /* Configure the OCxM bits in the CCMRx register */
    *(__IO uint32_t *) tmp |= (uint16_t)(TIM_OCMode << 8);
  }
}

/**
  * @brief  Enables or Disables the TIMx Update event.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx UDIS bit
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_UpdateDisableConfig(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the Update Disable Bit */
    TIMx->CR1 |= TIM_CR1_UDIS;
  }
  else
  {
    /* Reset the Update Disable Bit */
    TIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_UDIS);
  }
}

/**
  * @brief  Configures the TIMx Update Request Interrupt source.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_UpdateSource: specifies the Update source.
  *   This parameter can be one of the following values:
  *     @arg TIM_UpdateSource_Regular: Source of update is the counter overflow/underflow
                                       or the setting of UG bit, or an update generation
                                       through the slave mode controller.
  *     @arg TIM_UpdateSource_Global: Source of update is counter overflow/underflow.
  * @retval None
  */
void TIM_UpdateRequestConfig(STD_TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_UPDATE_SOURCE(TIM_UpdateSource));
  if (TIM_UpdateSource != TIM_UpdateSource_Global)
  {
    /* Set the URS Bit */
    TIMx->CR1 |= TIM_CR1_URS;
  }
  else
  {
    /* Reset the URS Bit */
    TIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_URS);
  }
}

/**
  * @brief  Enables or disables the TIMx's Hall sensor interface.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx Hall sensor interface.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_SelectHallSensor(STD_TIM_TypeDef* TIMx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Set the TI1S Bit */
    TIMx->CR2 |= TIM_CR2_TI1S;
  }
  else
  {
    /* Reset the TI1S Bit */
    TIMx->CR2 &= (uint16_t)~((uint16_t)TIM_CR2_TI1S);
  }
}

/**
  * @brief  Selects the TIMx's One Pulse Mode.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_OPMode: specifies the OPM Mode to be used.
  *   This parameter can be one of the following values:
  *     @arg TIM_OPMode_Single
  *     @arg TIM_OPMode_Repetitive
  * @retval None
  */
void TIM_SelectOnePulseMode(STD_TIM_TypeDef* TIMx, uint16_t TIM_OPMode)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_OPM_MODE(TIM_OPMode));
  /* Reset the OPM Bit */
  TIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_OPM);
  /* Configure the OPM Mode */
  TIMx->CR1 |= TIM_OPMode;
}

/**
  * @brief  Selects the TIMx Trigger Output Mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 6, 7, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_TRGOSource: specifies the Trigger Output source.
  *   This paramter can be one of the following values:
  *
  *  - For all TIMx
  *     @arg TIM_TRGOSource_Reset:  The UG bit in the TIM_EGR register is used as the trigger output (TRGO).
  *     @arg TIM_TRGOSource_Enable: The Counter Enable CEN is used as the trigger output (TRGO).
  *     @arg TIM_TRGOSource_Update: The update event is selected as the trigger output (TRGO).
  *
  *  - For all TIMx except TIM6 and TIM7
  *     @arg TIM_TRGOSource_OC1: The trigger output sends a positive pulse when the CC1IF flag
  *                              is to be set, as soon as a capture or compare match occurs (TRGO).
  *     @arg TIM_TRGOSource_OC1Ref: OC1REF signal is used as the trigger output (TRGO).
  *     @arg TIM_TRGOSource_OC2Ref: OC2REF signal is used as the trigger output (TRGO).
  *     @arg TIM_TRGOSource_OC3Ref: OC3REF signal is used as the trigger output (TRGO).
  *     @arg TIM_TRGOSource_OC4Ref: OC4REF signal is used as the trigger output (TRGO).
  *
  * @retval None
  */
void TIM_SelectOutputTrigger(STD_TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST7_PERIPH(TIMx));
  assert_param(IS_TIM_TRGO_SOURCE(TIM_TRGOSource));
  /* Reset the MMS Bits */
  TIMx->CR2 &= (uint16_t)~((uint16_t)TIM_CR2_MMS);
  /* Select the TRGO source */
  TIMx->CR2 |=  TIM_TRGOSource;
}

/**
  * @brief  Selects the TIMx Slave Mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_SlaveMode: specifies the Timer Slave Mode.
  *   This parameter can be one of the following values:
  *     @arg TIM_SlaveMode_Reset: Rising edge of the selected trigger signal (TRGI) re-initializes
  *                               the counter and triggers an update of the registers.
  *     @arg TIM_SlaveMode_Gated:     The counter clock is enabled when the trigger signal (TRGI) is high.
  *     @arg TIM_SlaveMode_Trigger:   The counter starts at a rising edge of the trigger TRGI.
  *     @arg TIM_SlaveMode_External1: Rising edges of the selected trigger (TRGI) clock the counter.
  * @retval None
  */
void TIM_SelectSlaveMode(STD_TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_SLAVE_MODE(TIM_SlaveMode));
 /* Reset the SMS Bits */
  TIMx->SMCR &= (uint16_t)~((uint16_t)TIM_SMCR_SMS);
  /* Select the Slave Mode */
  TIMx->SMCR |= TIM_SlaveMode;
}

/**
  * @brief  Sets or Resets the TIMx Master/Slave Mode.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_MasterSlaveMode: specifies the Timer Master Slave Mode.
  *   This parameter can be one of the following values:
  *     @arg TIM_MasterSlaveMode_Enable: synchronization between the current timer
  *                                      and its slaves (through TRGO).
  *     @arg TIM_MasterSlaveMode_Disable: No action
  * @retval None
  */
void TIM_SelectMasterSlaveMode(STD_TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_MSM_STATE(TIM_MasterSlaveMode));
  /* Reset the MSM Bit */
  TIMx->SMCR &= (uint16_t)~((uint16_t)TIM_SMCR_MSM);

  /* Set or Reset the MSM Bit */
  TIMx->SMCR |= TIM_MasterSlaveMode;
}

/**
  * @brief  Sets the TIMx Counter Register value
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  Counter: specifies the Counter register new value.
  * @retval None
  */
void TIM_SetCounter(STD_TIM_TypeDef* TIMx, uint16_t Counter)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* Set the Counter Register value */
  TIMx->CNT = Counter;
}

/**
  * @brief  Sets the TIMx Autoreload Register value
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  * @retval None
  */
void TIM_SetAutoreload(STD_TIM_TypeDef* TIMx, uint16_t Autoreload)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* Set the Autoreload Register value */
  TIMx->ARR = Autoreload;
}

/**
  * @brief  Sets the TIMx Capture Compare1 Register value
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  Compare1: specifies the Capture Compare1 register new value.
  * @retval None
  */
void TIM_SetCompare1(STD_TIM_TypeDef* TIMx, uint16_t Compare1)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  /* Set the Capture Compare1 Register value */
  TIMx->CCR1 = Compare1;
}

/**
  * @brief  Sets the TIMx Capture Compare2 Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  Compare2: specifies the Capture Compare2 register new value.
  * @retval None
  */
void TIM_SetCompare2(STD_TIM_TypeDef* TIMx, uint16_t Compare2)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* Set the Capture Compare2 Register value */
  TIMx->CCR2 = Compare2;
}

/**
  * @brief  Sets the TIMx Capture Compare3 Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  Compare3: specifies the Capture Compare3 register new value.
  * @retval None
  */
void TIM_SetCompare3(STD_TIM_TypeDef* TIMx, uint16_t Compare3)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  /* Set the Capture Compare3 Register value */
  TIMx->CCR3 = Compare3;
}

/**
  * @brief  Sets the TIMx Capture Compare4 Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  Compare4: specifies the Capture Compare4 register new value.
  * @retval None
  */
void TIM_SetCompare4(STD_TIM_TypeDef* TIMx, uint16_t Compare4)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  /* Set the Capture Compare4 Register value */
  TIMx->CCR4 = Compare4;
}

/**
  * @brief  Sets the TIMx Input Capture 1 prescaler.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_ICPSC: specifies the Input Capture1 prescaler new value.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
  *     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
  *     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
  * @retval None
  */
void TIM_SetIC1Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));
  /* Reset the IC1PSC Bits */
  TIMx->CCMR1 &= (uint16_t)~((uint16_t)TIM_CCMR1_IC1PSC);
  /* Set the IC1PSC value */
  TIMx->CCMR1 |= TIM_ICPSC;
}

/**
  * @brief  Sets the TIMx Input Capture 2 prescaler.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_ICPSC: specifies the Input Capture2 prescaler new value.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
  *     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
  *     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
  * @retval None
  */
void TIM_SetIC2Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));
  /* Reset the IC2PSC Bits */
  TIMx->CCMR1 &= (uint16_t)~((uint16_t)TIM_CCMR1_IC2PSC);
  /* Set the IC2PSC value */
  TIMx->CCMR1 |= (uint16_t)(TIM_ICPSC << 8);
}

/**
  * @brief  Sets the TIMx Input Capture 3 prescaler.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ICPSC: specifies the Input Capture3 prescaler new value.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
  *     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
  *     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
  * @retval None
  */
void TIM_SetIC3Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));
  /* Reset the IC3PSC Bits */
  TIMx->CCMR2 &= (uint16_t)~((uint16_t)TIM_CCMR2_IC3PSC);
  /* Set the IC3PSC value */
  TIMx->CCMR2 |= TIM_ICPSC;
}

/**
  * @brief  Sets the TIMx Input Capture 4 prescaler.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ICPSC: specifies the Input Capture4 prescaler new value.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPSC_DIV1: no prescaler
  *     @arg TIM_ICPSC_DIV2: capture is done once every 2 events
  *     @arg TIM_ICPSC_DIV4: capture is done once every 4 events
  *     @arg TIM_ICPSC_DIV8: capture is done once every 8 events
  * @retval None
  */
void TIM_SetIC4Prescaler(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPSC)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  assert_param(IS_TIM_IC_PRESCALER(TIM_ICPSC));
  /* Reset the IC4PSC Bits */
  TIMx->CCMR2 &= (uint16_t)~((uint16_t)TIM_CCMR2_IC4PSC);
  /* Set the IC4PSC value */
  TIMx->CCMR2 |= (uint16_t)(TIM_ICPSC << 8);
}

/**
  * @brief  Sets the TIMx Clock Division value.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select
  *   the TIM peripheral.
  * @param  TIM_CKD: specifies the clock division value.
  *   This parameter can be one of the following value:
  *     @arg TIM_CKD_DIV1: TDTS = Tck_tim
  *     @arg TIM_CKD_DIV2: TDTS = 2*Tck_tim
  *     @arg TIM_CKD_DIV4: TDTS = 4*Tck_tim
  * @retval None
  */
void TIM_SetClockDivision(STD_TIM_TypeDef* TIMx, uint16_t TIM_CKD)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  assert_param(IS_TIM_CKD_DIV(TIM_CKD));
  /* Reset the CKD Bits */
  TIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_CKD);
  /* Set the CKD value */
  TIMx->CR1 |= TIM_CKD;
}

/**
  * @brief  Gets the TIMx Input Capture 1 value.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @retval Capture Compare 1 Register value.
  */
uint16_t TIM_GetCapture1(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  /* Get the Capture 1 Register value */
  return TIMx->CCR1;
}

/**
  * @brief  Gets the TIMx Input Capture 2 value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @retval Capture Compare 2 Register value.
  */
uint16_t TIM_GetCapture2(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* Get the Capture 2 Register value */
  return TIMx->CCR2;
}

/**
  * @brief  Gets the TIMx Input Capture 3 value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @retval Capture Compare 3 Register value.
  */
uint16_t TIM_GetCapture3(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  /* Get the Capture 3 Register value */
  return TIMx->CCR3;
}

/**
  * @brief  Gets the TIMx Input Capture 4 value.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @retval Capture Compare 4 Register value.
  */
uint16_t TIM_GetCapture4(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_LIST3_PERIPH(TIMx));
  /* Get the Capture 4 Register value */
  return TIMx->CCR4;
}

/**
  * @brief  Gets the TIMx Counter value.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @retval Counter Register value.
  */
uint16_t TIM_GetCounter(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* Get the Counter Register value */
  return TIMx->CNT;
}

/**
  * @brief  Gets the TIMx Prescaler value.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @retval Prescaler Register value.
  */
uint16_t TIM_GetPrescaler(STD_TIM_TypeDef* TIMx)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  /* Get the Prescaler Register value */
  return TIMx->PSC;
}

/**
  * @brief  Checks whether the specified TIM flag is set or not.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg TIM_FLAG_Update: TIM update Flag
  *     @arg TIM_FLAG_CC1: TIM Capture Compare 1 Flag
  *     @arg TIM_FLAG_CC2: TIM Capture Compare 2 Flag
  *     @arg TIM_FLAG_CC3: TIM Capture Compare 3 Flag
  *     @arg TIM_FLAG_CC4: TIM Capture Compare 4 Flag
  *     @arg TIM_FLAG_COM: TIM Commutation Flag
  *     @arg TIM_FLAG_Trigger: TIM Trigger Flag
  *     @arg TIM_FLAG_Break: TIM Break Flag
  *     @arg TIM_FLAG_CC1OF: TIM Capture Compare 1 overcapture Flag
  *     @arg TIM_FLAG_CC2OF: TIM Capture Compare 2 overcapture Flag
  *     @arg TIM_FLAG_CC3OF: TIM Capture Compare 3 overcapture Flag
  *     @arg TIM_FLAG_CC4OF: TIM Capture Compare 4 overcapture Flag
  * @note
  *   - TIM6 and TIM7 can have only one update flag.
  *   - TIM9, TIM12 and TIM15 can have only TIM_FLAG_Update, TIM_FLAG_CC1,
  *      TIM_FLAG_CC2 or TIM_FLAG_Trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have TIM_FLAG_Update or TIM_FLAG_CC1.
  *   - TIM_FLAG_Break is used only with TIM1, TIM8 and TIM15.
  *   - TIM_FLAG_COM is used only with TIM1, TIM8, TIM15, TIM16 and TIM17.
  * @retval The new state of TIM_FLAG (SET or RESET).
  */
FlagStatus TIM_GetFlagStatus(STD_TIM_TypeDef* TIMx, uint16_t TIM_FLAG)
{
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_GET_FLAG(TIM_FLAG));

  if ((TIMx->SR & TIM_FLAG) != (uint16_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears the TIMx's pending flags.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_FLAG: specifies the flag bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg TIM_FLAG_Update: TIM update Flag
  *     @arg TIM_FLAG_CC1: TIM Capture Compare 1 Flag
  *     @arg TIM_FLAG_CC2: TIM Capture Compare 2 Flag
  *     @arg TIM_FLAG_CC3: TIM Capture Compare 3 Flag
  *     @arg TIM_FLAG_CC4: TIM Capture Compare 4 Flag
  *     @arg TIM_FLAG_COM: TIM Commutation Flag
  *     @arg TIM_FLAG_Trigger: TIM Trigger Flag
  *     @arg TIM_FLAG_Break: TIM Break Flag
  *     @arg TIM_FLAG_CC1OF: TIM Capture Compare 1 overcapture Flag
  *     @arg TIM_FLAG_CC2OF: TIM Capture Compare 2 overcapture Flag
  *     @arg TIM_FLAG_CC3OF: TIM Capture Compare 3 overcapture Flag
  *     @arg TIM_FLAG_CC4OF: TIM Capture Compare 4 overcapture Flag
  * @note
  *   - TIM6 and TIM7 can have only one update flag.
  *   - TIM9, TIM12 and TIM15 can have only TIM_FLAG_Update, TIM_FLAG_CC1,
  *      TIM_FLAG_CC2 or TIM_FLAG_Trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have TIM_FLAG_Update or TIM_FLAG_CC1.
  *   - TIM_FLAG_Break is used only with TIM1, TIM8 and TIM15.
  *   - TIM_FLAG_COM is used only with TIM1, TIM8, TIM15, TIM16 and TIM17.
  * @retval None
  */
void TIM_ClearFlag(STD_TIM_TypeDef* TIMx, uint16_t TIM_FLAG)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_CLEAR_FLAG(TIM_FLAG));

  /* Clear the flags */
  TIMx->SR = (uint16_t)~TIM_FLAG;
}

/**
  * @brief  Checks whether the TIM interrupt has occurred or not.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_IT: specifies the TIM interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg TIM_IT_Update: TIM update Interrupt source
  *     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     @arg TIM_IT_COM: TIM Commutation Interrupt source
  *     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *     @arg TIM_IT_Break: TIM Break Interrupt source
  * @note
  *   - TIM6 and TIM7 can generate only an update interrupt.
  *   - TIM9, TIM12 and TIM15 can have only TIM_IT_Update, TIM_IT_CC1,
  *      TIM_IT_CC2 or TIM_IT_Trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.
  *   - TIM_IT_Break is used only with TIM1, TIM8 and TIM15.
  *   - TIM_IT_COM is used only with TIM1, TIM8, TIM15, TIM16 and TIM17.
  * @retval The new state of the TIM_IT(SET or RESET).
  */
ITStatus TIM_GetITStatus(STD_TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  ITStatus bitstatus = RESET;
  uint16_t itstatus = 0x0, itenable = 0x0;
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_GET_IT(TIM_IT));

  itstatus = TIMx->SR & TIM_IT;

  itenable = TIMx->DIER & TIM_IT;
  if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears the TIMx's interrupt pending bits.
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral.
  * @param  TIM_IT: specifies the pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg TIM_IT_Update: TIM1 update Interrupt source
  *     @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *     @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *     @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *     @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *     @arg TIM_IT_COM: TIM Commutation Interrupt source
  *     @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *     @arg TIM_IT_Break: TIM Break Interrupt source
  * @note
  *   - TIM6 and TIM7 can generate only an update interrupt.
  *   - TIM9, TIM12 and TIM15 can have only TIM_IT_Update, TIM_IT_CC1,
  *      TIM_IT_CC2 or TIM_IT_Trigger.
  *   - TIM10, TIM11, TIM13, TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.
  *   - TIM_IT_Break is used only with TIM1, TIM8 and TIM15.
  *   - TIM_IT_COM is used only with TIM1, TIM8, TIM15, TIM16 and TIM17.
  * @retval None
  */
void TIM_ClearITPendingBit(STD_TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  /* Check the parameters */
  assert_param(IS_TIM_ALL_PERIPH(TIMx));
  assert_param(IS_TIM_IT(TIM_IT));
  /* Clear the IT pending Bit */
  TIMx->SR = (uint16_t)~TIM_IT;
}

/**
  * @brief  Configure the TI1 as Input.
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Rising
  *     @arg TIM_ICPolarity_Falling
  * @param  TIM_ICSelection: specifies the input to be used.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICSelection_DirectTI: TIM Input 1 is selected to be connected to IC1.
  *     @arg TIM_ICSelection_IndirectTI: TIM Input 1 is selected to be connected to IC2.
  *     @arg TIM_ICSelection_TRC: TIM Input 1 is selected to be connected to TRC.
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *   This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI1_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0;
  /* Disable the Channel 1: Reset the CC1E Bit */
  TIMx->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC1E);
  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;
  /* Select the Input and set the filter */
  tmpccmr1 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR1_CC1S)) & ((uint16_t)~((uint16_t)TIM_CCMR1_IC1F)));
  tmpccmr1 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8) || (TIMx == STD_TIM2) || (TIMx == STD_TIM3) ||
     (TIMx == STD_TIM4) ||(TIMx == STD_TIM5))
  {
    /* Select the Polarity and set the CC1E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC1P));
    tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CCER_CC1E);
  }
  else
  {
    /* Select the Polarity and set the CC1E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC1P | TIM_CCER_CC1NP));
    tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CCER_CC1E);
  }

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1;
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configure the TI2 as Input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Rising
  *     @arg TIM_ICPolarity_Falling
  * @param  TIM_ICSelection: specifies the input to be used.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICSelection_DirectTI: TIM Input 2 is selected to be connected to IC2.
  *     @arg TIM_ICSelection_IndirectTI: TIM Input 2 is selected to be connected to IC1.
  *     @arg TIM_ICSelection_TRC: TIM Input 2 is selected to be connected to TRC.
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *   This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI2_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;
  /* Disable the Channel 2: Reset the CC2E Bit */
  TIMx->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);
  tmpccmr1 = TIMx->CCMR1;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 4);
  /* Select the Input and set the filter */
  tmpccmr1 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR1_CC2S)) & ((uint16_t)~((uint16_t)TIM_CCMR1_IC2F)));
  tmpccmr1 |= (uint16_t)(TIM_ICFilter << 12);
  tmpccmr1 |= (uint16_t)(TIM_ICSelection << 8);

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8) || (TIMx == STD_TIM2) || (TIMx == STD_TIM3) ||
     (TIMx == STD_TIM4) ||(TIMx == STD_TIM5))
  {
    /* Select the Polarity and set the CC2E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC2P));
    tmpccer |=  (uint16_t)(tmp | (uint16_t)TIM_CCER_CC2E);
  }
  else
  {
    /* Select the Polarity and set the CC2E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC2P | TIM_CCER_CC2NP));
    tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CCER_CC2E);
  }

  /* Write to TIMx CCMR1 and CCER registers */
  TIMx->CCMR1 = tmpccmr1 ;
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configure the TI3 as Input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Rising
  *     @arg TIM_ICPolarity_Falling
  * @param  TIM_ICSelection: specifies the input to be used.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICSelection_DirectTI: TIM Input 3 is selected to be connected to IC3.
  *     @arg TIM_ICSelection_IndirectTI: TIM Input 3 is selected to be connected to IC4.
  *     @arg TIM_ICSelection_TRC: TIM Input 3 is selected to be connected to TRC.
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *   This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI3_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;
  /* Disable the Channel 3: Reset the CC3E Bit */
  TIMx->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC3E);
  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 8);
  /* Select the Input and set the filter */
  tmpccmr2 &= (uint16_t)(((uint16_t)~((uint16_t)TIM_CCMR2_CC3S)) & ((uint16_t)~((uint16_t)TIM_CCMR2_IC3F)));
  tmpccmr2 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8) || (TIMx == STD_TIM2) || (TIMx == STD_TIM3) ||
     (TIMx == STD_TIM4) ||(TIMx == STD_TIM5))
  {
    /* Select the Polarity and set the CC3E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC3P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CCER_CC3E);
  }
  else
  {
    /* Select the Polarity and set the CC3E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC3P | TIM_CCER_CC3NP));
    tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CCER_CC3E);
  }

  /* Write to TIMx CCMR2 and CCER registers */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Configure the TI4 as Input.
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_ICPolarity : The Input Polarity.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICPolarity_Rising
  *     @arg TIM_ICPolarity_Falling
  * @param  TIM_ICSelection: specifies the input to be used.
  *   This parameter can be one of the following values:
  *     @arg TIM_ICSelection_DirectTI: TIM Input 4 is selected to be connected to IC4.
  *     @arg TIM_ICSelection_IndirectTI: TIM Input 4 is selected to be connected to IC3.
  *     @arg TIM_ICSelection_TRC: TIM Input 4 is selected to be connected to TRC.
  * @param  TIM_ICFilter: Specifies the Input Capture Filter.
  *   This parameter must be a value between 0x00 and 0x0F.
  * @retval None
  */
static void TI4_Config(STD_TIM_TypeDef* TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
  uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

   /* Disable the Channel 4: Reset the CC4E Bit */
  TIMx->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC4E);
  tmpccmr2 = TIMx->CCMR2;
  tmpccer = TIMx->CCER;
  tmp = (uint16_t)(TIM_ICPolarity << 12);
  /* Select the Input and set the filter */
  tmpccmr2 &= (uint16_t)((uint16_t)(~(uint16_t)TIM_CCMR2_CC4S) & ((uint16_t)~((uint16_t)TIM_CCMR2_IC4F)));
  tmpccmr2 |= (uint16_t)(TIM_ICSelection << 8);
  tmpccmr2 |= (uint16_t)(TIM_ICFilter << 12);

  if((TIMx == STD_TIM1) || (TIMx == STD_TIM8) || (TIMx == STD_TIM2) || (TIMx == STD_TIM3) ||
     (TIMx == STD_TIM4) ||(TIMx == STD_TIM5))
  {
    /* Select the Polarity and set the CC4E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC4P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CCER_CC4E);
  }
  else
  {
    /* Select the Polarity and set the CC4E Bit */
    tmpccer &= (uint16_t)~((uint16_t)(TIM_CCER_CC3P | TIM_CCER_CC4NP));
    tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CCER_CC4E);
  }
  /* Write to TIMx CCMR2 and CCER registers */
  TIMx->CCMR2 = tmpccmr2;
  TIMx->CCER = tmpccer;
}

//STD DAC

/**
  * @brief  DAC Init structure definition
  */

typedef struct
{
  uint32_t DAC_Trigger;                      /*!< Specifies the external trigger for the selected DAC channel.
                                                  This parameter can be a value of @ref DAC_trigger_selection */

  uint32_t DAC_WaveGeneration;               /*!< Specifies whether DAC channel noise waves or triangle waves
                                                  are generated, or whether no wave is generated.
                                                  This parameter can be a value of @ref DAC_wave_generation */

  uint32_t DAC_LFSRUnmask_TriangleAmplitude; /*!< Specifies the LFSR mask for noise wave generation or
                                                  the maximum amplitude triangle generation for the DAC channel.
                                                  This parameter can be a value of @ref DAC_lfsrunmask_triangleamplitude */

  uint32_t DAC_OutputBuffer;                 /*!< Specifies whether the DAC channel output buffer is enabled or disabled.
                                                  This parameter can be a value of @ref DAC_output_buffer */
}STD_DAC_InitTypeDef;

/**
  * @}
  */

/** @defgroup DAC_Exported_Constants
  * @{
  */

/** @defgroup DAC_trigger_selection
  * @{
  */

#define DAC_Trigger_None                   ((uint32_t)0x00000000) /*!< Conversion is automatic once the DAC1_DHRxxxx register
                                                                       has been loaded, and not by external trigger */
#define DAC_Trigger_T6_TRGO                ((uint32_t)0x00000004) /*!< TIM6 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T8_TRGO                ((uint32_t)0x0000000C) /*!< TIM8 TRGO selected as external conversion trigger for DAC channel
                                                                       only in High-density devices*/
#define DAC_Trigger_T3_TRGO                ((uint32_t)0x0000000C) /*!< TIM8 TRGO selected as external conversion trigger for DAC channel
                                                                       only in Connectivity line, Medium-density and Low-density Value Line devices */
#define DAC_Trigger_T7_TRGO                ((uint32_t)0x00000014) /*!< TIM7 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T5_TRGO                ((uint32_t)0x0000001C) /*!< TIM5 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T15_TRGO               ((uint32_t)0x0000001C) /*!< TIM15 TRGO selected as external conversion trigger for DAC channel
                                                                       only in Medium-density and Low-density Value Line devices*/
#define DAC_Trigger_T2_TRGO                ((uint32_t)0x00000024) /*!< TIM2 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T4_TRGO                ((uint32_t)0x0000002C) /*!< TIM4 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_Ext_IT9                ((uint32_t)0x00000034) /*!< EXTI Line9 event selected as external conversion trigger for DAC channel */
#define DAC_Trigger_Software               ((uint32_t)0x0000003C) /*!< Conversion started by software trigger for DAC channel */

#define IS_DAC_TRIGGER(TRIGGER) (((TRIGGER) == DAC_Trigger_None) || \
                                 ((TRIGGER) == DAC_Trigger_T6_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T8_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T7_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T5_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T2_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_T4_TRGO) || \
                                 ((TRIGGER) == DAC_Trigger_Ext_IT9) || \
                                 ((TRIGGER) == DAC_Trigger_Software))

/**
  * @}
  */

/** @defgroup DAC_wave_generation
  * @{
  */

#define DAC_WaveGeneration_None            ((uint32_t)0x00000000)
#define DAC_WaveGeneration_Noise           ((uint32_t)0x00000040)
#define DAC_WaveGeneration_Triangle        ((uint32_t)0x00000080)
#define IS_DAC_GENERATE_WAVE(WAVE) (((WAVE) == DAC_WaveGeneration_None) || \
                                    ((WAVE) == DAC_WaveGeneration_Noise) || \
                                    ((WAVE) == DAC_WaveGeneration_Triangle))
/**
  * @}
  */

/** @defgroup DAC_lfsrunmask_triangleamplitude
  * @{
  */

#define DAC_LFSRUnmask_Bit0                ((uint32_t)0x00000000) /*!< Unmask DAC channel LFSR bit0 for noise wave generation */
#define DAC_LFSRUnmask_Bits1_0             ((uint32_t)0x00000100) /*!< Unmask DAC channel LFSR bit[1:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits2_0             ((uint32_t)0x00000200) /*!< Unmask DAC channel LFSR bit[2:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits3_0             ((uint32_t)0x00000300) /*!< Unmask DAC channel LFSR bit[3:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits4_0             ((uint32_t)0x00000400) /*!< Unmask DAC channel LFSR bit[4:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits5_0             ((uint32_t)0x00000500) /*!< Unmask DAC channel LFSR bit[5:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits6_0             ((uint32_t)0x00000600) /*!< Unmask DAC channel LFSR bit[6:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits7_0             ((uint32_t)0x00000700) /*!< Unmask DAC channel LFSR bit[7:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits8_0             ((uint32_t)0x00000800) /*!< Unmask DAC channel LFSR bit[8:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits9_0             ((uint32_t)0x00000900) /*!< Unmask DAC channel LFSR bit[9:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits10_0            ((uint32_t)0x00000A00) /*!< Unmask DAC channel LFSR bit[10:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits11_0            ((uint32_t)0x00000B00) /*!< Unmask DAC channel LFSR bit[11:0] for noise wave generation */
#define DAC_TriangleAmplitude_1            ((uint32_t)0x00000000) /*!< Select max triangle amplitude of 1 */
#define DAC_TriangleAmplitude_3            ((uint32_t)0x00000100) /*!< Select max triangle amplitude of 3 */
#define DAC_TriangleAmplitude_7            ((uint32_t)0x00000200) /*!< Select max triangle amplitude of 7 */
#define DAC_TriangleAmplitude_15           ((uint32_t)0x00000300) /*!< Select max triangle amplitude of 15 */
#define DAC_TriangleAmplitude_31           ((uint32_t)0x00000400) /*!< Select max triangle amplitude of 31 */
#define DAC_TriangleAmplitude_63           ((uint32_t)0x00000500) /*!< Select max triangle amplitude of 63 */
#define DAC_TriangleAmplitude_127          ((uint32_t)0x00000600) /*!< Select max triangle amplitude of 127 */
#define DAC_TriangleAmplitude_255          ((uint32_t)0x00000700) /*!< Select max triangle amplitude of 255 */
#define DAC_TriangleAmplitude_511          ((uint32_t)0x00000800) /*!< Select max triangle amplitude of 511 */
#define DAC_TriangleAmplitude_1023         ((uint32_t)0x00000900) /*!< Select max triangle amplitude of 1023 */
#define DAC_TriangleAmplitude_2047         ((uint32_t)0x00000A00) /*!< Select max triangle amplitude of 2047 */
#define DAC_TriangleAmplitude_4095         ((uint32_t)0x00000B00) /*!< Select max triangle amplitude of 4095 */

#define IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(VALUE) (((VALUE) == DAC_LFSRUnmask_Bit0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits1_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits2_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits3_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits4_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits5_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits6_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits7_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits8_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits9_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits10_0) || \
                                                      ((VALUE) == DAC_LFSRUnmask_Bits11_0) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_1) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_3) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_7) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_15) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_31) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_63) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_127) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_255) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_511) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_1023) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_2047) || \
                                                      ((VALUE) == DAC_TriangleAmplitude_4095))
/**
  * @}
  */

/** @defgroup DAC_output_buffer
  * @{
  */

#define DAC_OutputBuffer_Enable            ((uint32_t)0x00000000)
#define DAC_OutputBuffer_Disable           ((uint32_t)0x00000002)
#define IS_DAC_OUTPUT_BUFFER_STATE(STATE) (((STATE) == DAC_OutputBuffer_Enable) || \
                                           ((STATE) == DAC_OutputBuffer_Disable))
/**
  * @}
  */

/** @defgroup DAC_Channel_selection
  * @{
  */

#define DAC_Channel_1                      ((uint32_t)0x00000000)
#define DAC_Channel_2                      ((uint32_t)0x00000010)
#define IS_DAC_CHANNEL(CHANNEL) (((CHANNEL) == DAC_Channel_1) || \
                                 ((CHANNEL) == DAC_Channel_2))
/**
  * @}
  */

/** @defgroup DAC_data_alignment
  * @{
  */

#define DAC_Align_12b_R                    ((uint32_t)0x00000000)
#define DAC_Align_12b_L                    ((uint32_t)0x00000004)
#define DAC_Align_8b_R                     ((uint32_t)0x00000008)
#define IS_DAC_ALIGN(ALIGN) (((ALIGN) == DAC_Align_12b_R) || \
                             ((ALIGN) == DAC_Align_12b_L) || \
                             ((ALIGN) == DAC_Align_8b_R))
/**
  * @}
  */

/** @defgroup DAC_wave_generation
  * @{
  */

#define DAC_Wave_Noise                     ((uint32_t)0x00000040)
#define DAC_Wave_Triangle                  ((uint32_t)0x00000080)
#define IS_DAC_WAVE(WAVE) (((WAVE) == DAC_Wave_Noise) || \
                           ((WAVE) == DAC_Wave_Triangle))
/**
  * @}
  */

/** @defgroup DAC_data
  * @{
  */

#define IS_DAC_DATA(DATA) ((DATA) <= 0xFFF0)


/* CR register Mask */
#define CR_CLEAR_MASK              ((uint32_t)0x00000FFE)

/* DAC Dual Channels SWTRIG masks */
#define DUAL_SWTRIG_SET            ((uint32_t)0x00000003)
#define DUAL_SWTRIG_RESET          ((uint32_t)0xFFFFFFFC)

/* DHR registers offsets */
#define DHR12R1_OFFSET             ((uint32_t)0x00000008)
#define DHR12R2_OFFSET             ((uint32_t)0x00000014)
#define DHR12RD_OFFSET             ((uint32_t)0x00000020)

/* DOR register offset */
#define DOR_OFFSET                 ((uint32_t)0x0000002C)

/**
  * @brief  Deinitializes the DAC peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void DAC_DeInit(void)
{
	  periph_clk_en(APB1,RCC_APB1Periph_DAC);
	  periph_clk_dis(APB1,RCC_APB1Periph_DAC);
//  /* Enable DAC reset state */
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_DAC, ENABLE);
//  /* Release DAC from reset state */
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_DAC, DISABLE);
}



/**
  * @brief  Initializes the DAC peripheral according to the specified
  *         parameters in the DAC_InitStruct.
  * @param  DAC_Channel: the selected DAC channel.
  *   This parameter can be one of the following values:
  *     @arg DAC_Channel_1: DAC Channel1 selected
  *     @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_InitStruct: pointer to a DAC_InitTypeDef structure that
  *        contains the configuration information for the specified DAC channel.
  * @retval None
  */
void DAC_Init(uint32_t DAC_Channel, STD_DAC_InitTypeDef* DAC_InitStruct)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;
  /* Check the DAC parameters */
  assert_param(IS_DAC_TRIGGER(DAC_InitStruct->DAC_Trigger));
  assert_param(IS_DAC_GENERATE_WAVE(DAC_InitStruct->DAC_WaveGeneration));
  assert_param(IS_DAC_LFSR_UNMASK_TRIANGLE_AMPLITUDE(DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude));
  assert_param(IS_DAC_OUTPUT_BUFFER_STATE(DAC_InitStruct->DAC_OutputBuffer));
/*---------------------------- DAC CR Configuration --------------------------*/
  /* Get the DAC CR value */
  tmpreg1 = DAC->CR;
  /* Clear BOFFx, TENx, TSELx, WAVEx and MAMPx bits */
  tmpreg1 &= ~(CR_CLEAR_MASK << DAC_Channel);
  /* Configure for the selected DAC channel: buffer output, trigger, wave generation,
     mask/amplitude for wave generation */
  /* Set TSELx and TENx bits according to DAC_Trigger value */
  /* Set WAVEx bits according to DAC_WaveGeneration value */
  /* Set MAMPx bits according to DAC_LFSRUnmask_TriangleAmplitude value */
  /* Set BOFFx bit according to DAC_OutputBuffer value */
  tmpreg2 = (DAC_InitStruct->DAC_Trigger | DAC_InitStruct->DAC_WaveGeneration |
             DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude | DAC_InitStruct->DAC_OutputBuffer);
  /* Calculate CR register value depending on DAC_Channel */
  tmpreg1 |= tmpreg2 << DAC_Channel;
  /* Write to DAC CR */
  DAC->CR = tmpreg1;
}

/**
  * @brief  Fills each DAC_InitStruct member with its default value.
  * @param  DAC_InitStruct : pointer to a DAC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void DAC_StructInit(STD_DAC_InitTypeDef* DAC_InitStruct)
{
/*--------------- Reset DAC init structure parameters values -----------------*/
  /* Initialize the DAC_Trigger member */
  DAC_InitStruct->DAC_Trigger = DAC_Trigger_None;
  /* Initialize the DAC_WaveGeneration member */
  DAC_InitStruct->DAC_WaveGeneration = DAC_WaveGeneration_None;
  /* Initialize the DAC_LFSRUnmask_TriangleAmplitude member */
  DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
  /* Initialize the DAC_OutputBuffer member */
  DAC_InitStruct->DAC_OutputBuffer = DAC_OutputBuffer_Enable;
}

/**
  * @brief  Enables or disables the specified DAC channel.
  * @param  DAC_Channel: the selected DAC channel.
  *   This parameter can be one of the following values:
  *     @arg DAC_Channel_1: DAC Channel1 selected
  *     @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the DAC channel.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected DAC channel */
    DAC->CR |= (DAC_CR_EN1 << DAC_Channel);
  }
  else
  {
    /* Disable the selected DAC channel */
    DAC->CR &= ~(DAC_CR_EN1 << DAC_Channel);
  }
}

/**
  * @brief  Enables or disables the specified DAC channel DMA request.
  * @param  DAC_Channel: the selected DAC channel.
  *   This parameter can be one of the following values:
  *     @arg DAC_Channel_1: DAC Channel1 selected
  *     @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the selected DAC channel DMA request.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected DAC channel DMA request */
    DAC->CR |= (DAC_CR_DMAEN1 << DAC_Channel);
  }
  else
  {
    /* Disable the selected DAC channel DMA request */
    DAC->CR &= ~(DAC_CR_DMAEN1 << DAC_Channel);
  }
}

/**
  * @brief  Enables or disables the selected DAC channel software trigger.
  * @param  DAC_Channel: the selected DAC channel.
  *   This parameter can be one of the following values:
  *     @arg DAC_Channel_1: DAC Channel1 selected
  *     @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the selected DAC channel software trigger.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable software trigger for the selected DAC channel */
    DAC->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4);
  }
  else
  {
    /* Disable software trigger for the selected DAC channel */
    DAC->SWTRIGR &= ~((uint32_t)DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4));
  }
}

/**
  * @brief  Enables or disables simultaneously the two DAC channels software
  *   triggers.
  * @param  NewState: new state of the DAC channels software triggers.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable software trigger for both DAC channels */
    DAC->SWTRIGR |= DUAL_SWTRIG_SET ;
  }
  else
  {
    /* Disable software trigger for both DAC channels */
    DAC->SWTRIGR &= DUAL_SWTRIG_RESET;
  }
}

/**
  * @brief  Enables or disables the selected DAC channel wave generation.
  * @param  DAC_Channel: the selected DAC channel.
  *   This parameter can be one of the following values:
  *     @arg DAC_Channel_1: DAC Channel1 selected
  *     @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_Wave: Specifies the wave type to enable or disable.
  *   This parameter can be one of the following values:
  *     @arg DAC_Wave_Noise: noise wave generation
  *     @arg DAC_Wave_Triangle: triangle wave generation
  * @param  NewState: new state of the selected DAC channel wave generation.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));
  assert_param(IS_DAC_WAVE(DAC_Wave));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected wave generation for the selected DAC channel */
    DAC->CR |= DAC_Wave << DAC_Channel;
  }
  else
  {
    /* Disable the selected wave generation for the selected DAC channel */
    DAC->CR &= ~(DAC_Wave << DAC_Channel);
  }
}

/**
  * @brief  Set the specified data holding register value for DAC channel1.
  * @param  DAC_Align: Specifies the data alignment for DAC channel1.
  *   This parameter can be one of the following values:
  *     @arg DAC_Align_8b_R: 8bit right data alignment selected
  *     @arg DAC_Align_12b_L: 12bit left data alignment selected
  *     @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data : Data to be loaded in the selected data holding register.
  * @retval None
  */
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(DAC_Align));
  assert_param(IS_DAC_DATA(Data));

  tmp = (uint32_t)DAC_BASE;
  tmp += DHR12R1_OFFSET + DAC_Align;

  /* Set the DAC channel1 selected data holding register */
  *(__IO uint32_t *) tmp = Data;
}


/**
  * @brief  Set the specified data holding register value for DAC channel2.
  * @param  DAC_Align: Specifies the data alignment for DAC channel2.
  *   This parameter can be one of the following values:
  *     @arg DAC_Align_8b_R: 8bit right data alignment selected
  *     @arg DAC_Align_12b_L: 12bit left data alignment selected
  *     @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data : Data to be loaded in the selected data holding register.
  * @retval None
  */
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(DAC_Align));
  assert_param(IS_DAC_DATA(Data));

  tmp = (uint32_t)DAC_BASE;
  tmp += DHR12R2_OFFSET + DAC_Align;

  /* Set the DAC channel2 selected data holding register */
  *(__IO uint32_t *)tmp = Data;
}

/**
  * @brief  Set the specified data holding register value for dual channel
  *   DAC.
  * @param  DAC_Align: Specifies the data alignment for dual channel DAC.
  *   This parameter can be one of the following values:
  *     @arg DAC_Align_8b_R: 8bit right data alignment selected
  *     @arg DAC_Align_12b_L: 12bit left data alignment selected
  *     @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data2: Data for DAC Channel2 to be loaded in the selected data
  *   holding register.
  * @param  Data1: Data for DAC Channel1 to be loaded in the selected data
  *   holding register.
  * @retval None
  */
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1)
{
  uint32_t data = 0, tmp = 0;

  /* Check the parameters */
  assert_param(IS_DAC_ALIGN(DAC_Align));
  assert_param(IS_DAC_DATA(Data1));
  assert_param(IS_DAC_DATA(Data2));

  /* Calculate and set dual DAC data holding register value */
  if (DAC_Align == DAC_Align_8b_R)
  {
    data = ((uint32_t)Data2 << 8) | Data1;
  }
  else
  {
    data = ((uint32_t)Data2 << 16) | Data1;
  }

  tmp = (uint32_t)DAC_BASE;
  tmp += DHR12RD_OFFSET + DAC_Align;

  /* Set the dual DAC selected data holding register */
  *(__IO uint32_t *)tmp = data;
}

/**
  * @brief  Returns the last data output value of the selected DAC channel.
  * @param  DAC_Channel: the selected DAC channel.
  *   This parameter can be one of the following values:
  *     @arg DAC_Channel_1: DAC Channel1 selected
  *     @arg DAC_Channel_2: DAC Channel2 selected
  * @retval The selected DAC channel data output value.
  */
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel)
{
  __IO uint32_t tmp = 0;

  /* Check the parameters */
  assert_param(IS_DAC_CHANNEL(DAC_Channel));

  tmp = (uint32_t) DAC_BASE ;
  tmp += DOR_OFFSET + ((uint32_t)DAC_Channel >> 2);

  /* Returns the DAC channel data output register value */
  return (uint16_t) (*(__IO uint32_t*) tmp);
}

//STD IWDG

/** @defgroup IWDG_WriteAccess
  * @{
  */

#define IWDG_WriteAccess_Enable     ((uint16_t)0x5555)
#define IWDG_WriteAccess_Disable    ((uint16_t)0x0000)
#define IS_IWDG_WRITE_ACCESS(ACCESS) (((ACCESS) == IWDG_WriteAccess_Enable) || \
                                      ((ACCESS) == IWDG_WriteAccess_Disable))
/**
  * @}
  */

/** @defgroup IWDG_prescaler
  * @{
  */

#define IWDG_Prescaler_4            ((uint8_t)0x00)
#define IWDG_Prescaler_8            ((uint8_t)0x01)
#define IWDG_Prescaler_16           ((uint8_t)0x02)
#define IWDG_Prescaler_32           ((uint8_t)0x03)
#define IWDG_Prescaler_64           ((uint8_t)0x04)
#define IWDG_Prescaler_128          ((uint8_t)0x05)
#define IWDG_Prescaler_256          ((uint8_t)0x06)
#define IS_IWDG_PRESCALER(PRESCALER) (((PRESCALER) == IWDG_Prescaler_4)  || \
                                      ((PRESCALER) == IWDG_Prescaler_8)  || \
                                      ((PRESCALER) == IWDG_Prescaler_16) || \
                                      ((PRESCALER) == IWDG_Prescaler_32) || \
                                      ((PRESCALER) == IWDG_Prescaler_64) || \
                                      ((PRESCALER) == IWDG_Prescaler_128)|| \
                                      ((PRESCALER) == IWDG_Prescaler_256))
/**
  * @}
  */

/** @defgroup IWDG_Flag
  * @{
  */

#define IWDG_FLAG_PVU               ((uint16_t)0x0001)
#define IWDG_FLAG_RVU               ((uint16_t)0x0002)
#define IS_IWDG_FLAG(FLAG) (((FLAG) == IWDG_FLAG_PVU) || ((FLAG) == IWDG_FLAG_RVU))
#define IS_IWDG_RELOAD(RELOAD) ((RELOAD) <= 0xFFF)

/* ---------------------- IWDG registers bit mask ----------------------------*/

/* KR register bit mask */
#define KR_KEY_Reload    ((uint16_t)0xAAAA)
#define KR_KEY_Enable    ((uint16_t)0xCCCC)

/**
  * @brief  Enables or disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  IWDG_WriteAccess: new state of write access to IWDG_PR and IWDG_RLR registers.
  *   This parameter can be one of the following values:
  *     @arg IWDG_WriteAccess_Enable: Enable write access to IWDG_PR and IWDG_RLR registers
  *     @arg IWDG_WriteAccess_Disable: Disable write access to IWDG_PR and IWDG_RLR registers
  * @retval None
  */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WRITE_ACCESS(IWDG_WriteAccess));
  IWDG->KR = IWDG_WriteAccess;
}

/**
  * @brief  Sets IWDG Prescaler value.
  * @param  IWDG_Prescaler: specifies the IWDG Prescaler value.
  *   This parameter can be one of the following values:
  *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
  *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
  *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
  *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
  *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
  *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
  *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
  * @retval None
  */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
  /* Check the parameters */
  assert_param(IS_IWDG_PRESCALER(IWDG_Prescaler));
  IWDG->PR = IWDG_Prescaler;
}

/**
  * @brief  Sets IWDG Reload value.
  * @param  Reload: specifies the IWDG Reload value.
  *   This parameter must be a number between 0 and 0x0FFF.
  * @retval None
  */
void IWDG_SetReload(uint16_t Reload)
{
  /* Check the parameters */
  assert_param(IS_IWDG_RELOAD(Reload));
  IWDG->RLR = Reload;
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  *   (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @param  None
  * @retval None
  */
void IWDG_ReloadCounter(void)
{
  IWDG->KR = KR_KEY_Reload;
}

/**
  * @brief  Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @param  None
  * @retval None
  */
void IWDG_Enable(void)
{
  IWDG->KR = KR_KEY_Enable;
}

/**
  * @brief  Checks whether the specified IWDG flag is set or not.
  * @param  IWDG_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg IWDG_FLAG_PVU: Prescaler Value Update on going
  *     @arg IWDG_FLAG_RVU: Reload Value Update on going
  * @retval The new state of IWDG_FLAG (SET or RESET).
  */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_IWDG_FLAG(IWDG_FLAG));
  if ((IWDG->SR & IWDG_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}


//STD WWDG
/** @defgroup WWDG_Prescaler
  * @{
  */

#define WWDG_Prescaler_1    ((uint32_t)0x00000000)
#define WWDG_Prescaler_2    ((uint32_t)0x00000080)
#define WWDG_Prescaler_4    ((uint32_t)0x00000100)
#define WWDG_Prescaler_8    ((uint32_t)0x00000180)
#define IS_WWDG_PRESCALER(PRESCALER) (((PRESCALER) == WWDG_Prescaler_1) || \
                                      ((PRESCALER) == WWDG_Prescaler_2) || \
                                      ((PRESCALER) == WWDG_Prescaler_4) || \
                                      ((PRESCALER) == WWDG_Prescaler_8))
#define IS_WWDG_WINDOW_VALUE(VALUE) ((VALUE) <= 0x7F)
#define IS_WWDG_COUNTER(COUNTER) (((COUNTER) >= 0x40) && ((COUNTER) <= 0x7F))


/* ----------- WWDG registers bit address in the alias region ----------- */
#define WWDG_OFFSET       (WWDG_BASE - PERIPH_BASE)

/* Alias word address of EWI bit */
#define CFR_OFFSET        (WWDG_OFFSET + 0x04)
#define EWI_BitNumber     0x09
#define CFR_EWI_BB        (PERIPH_BB_BASE + (CFR_OFFSET * 32) + (EWI_BitNumber * 4))

/* --------------------- WWDG registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_WDGA_Set       ((uint32_t)0x00000080)

/* CFR register bit mask */
#define CFR_WDGTB_Mask    ((uint32_t)0xFFFFFE7F)
#define CFR_W_Mask        ((uint32_t)0xFFFFFF80)
#define BIT_Mask          ((uint8_t)0x7F)



/**
  * @brief  Deinitializes the WWDG peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void WWDG_DeInit(void)
{

	  periph_clk_en(APB1,RCC_APB1Periph_WWDG);
	  periph_clk_dis(APB1,RCC_APB1Periph_WWDG);

//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, ENABLE);
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, DISABLE);
}

/**
  * @brief  Sets the WWDG Prescaler.
  * @param  WWDG_Prescaler: specifies the WWDG Prescaler.
  *   This parameter can be one of the following values:
  *     @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
  *     @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
  *     @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
  *     @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
  * @retval None
  */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_WWDG_PRESCALER(WWDG_Prescaler));
  /* Clear WDGTB[1:0] bits */
  tmpreg = WWDG->CFR & CFR_WDGTB_Mask;
  /* Set WDGTB[1:0] bits according to WWDG_Prescaler value */
  tmpreg |= WWDG_Prescaler;
  /* Store the new value */
  WWDG->CFR = tmpreg;
}

/**
  * @brief  Sets the WWDG window value.
  * @param  WindowValue: specifies the window value to be compared to the downcounter.
  *   This parameter value must be lower than 0x80.
  * @retval None
  */
void WWDG_SetWindowValue(uint8_t WindowValue)
{
  __IO uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_WWDG_WINDOW_VALUE(WindowValue));
  /* Clear W[6:0] bits */

  tmpreg = WWDG->CFR & CFR_W_Mask;

  /* Set W[6:0] bits according to WindowValue value */
  tmpreg |= WindowValue & (uint32_t) BIT_Mask;

  /* Store the new value */
  WWDG->CFR = tmpreg;
}

/**
  * @brief  Enables the WWDG Early Wakeup interrupt(EWI).
  * @param  None
  * @retval None
  */
void WWDG_EnableIT(void)
{
  *(__IO uint32_t *) CFR_EWI_BB = (uint32_t)ENABLE;
}

/**
  * @brief  Sets the WWDG counter value.
  * @param  Counter: specifies the watchdog counter value.
  *   This parameter must be a number between 0x40 and 0x7F.
  * @retval None
  */
void WWDG_SetCounter(uint8_t Counter)
{
  /* Check the parameters */
  assert_param(IS_WWDG_COUNTER(Counter));
  /* Write to T[6:0] bits to configure the counter value, no need to do
     a read-modify-write; writing a 0 to WDGA bit does nothing */
  WWDG->CR = Counter & BIT_Mask;
}

/**
  * @brief  Enables WWDG and load the counter value.
  * @param  Counter: specifies the watchdog counter value.
  *   This parameter must be a number between 0x40 and 0x7F.
  * @retval None
  */
void WWDG_Enable(uint8_t Counter)
{
  /* Check the parameters */
  assert_param(IS_WWDG_COUNTER(Counter));
  WWDG->CR = CR_WDGA_Set | Counter;
}

/**
  * @brief  Checks whether the Early Wakeup interrupt flag is set or not.
  * @param  None
  * @retval The new state of the Early Wakeup interrupt flag (SET or RESET)
  */
FlagStatus WWDG_GetFlagStatus(void)
{
  return (FlagStatus)(WWDG->SR);
}

/**
  * @brief  Clears Early Wakeup interrupt flag.
  * @param  None
  * @retval None
  */
void WWDG_ClearFlag(void)
{
  WWDG->SR = (uint32_t)RESET;
}

//STD GPIO

#define EVCR_PORTPINCONFIG_MASK     ((uint16_t)0xFF80)
#define LSB_MASK                    ((uint16_t)0xFFFF)
#define DBGAFR_POSITION_MASK        ((uint32_t)0x000F0000)
#define DBGAFR_SWJCFG_MASK          ((uint32_t)0xF0FFFFFF)
#define DBGAFR_LOCATION_MASK        ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK         ((uint32_t)0x00100000)

#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)

void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState)
{
  uint32_t tmp = 0x00, tmp1 = 0x00, tmpreg = 0x00, tmpmask = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_REMAP(GPIO_Remap));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if((GPIO_Remap & 0x80000000) == 0x80000000)
  {
    tmpreg = AFIO->MAPR2;
  }
  else
  {
    tmpreg = AFIO->MAPR;
  }

  tmpmask = (GPIO_Remap & DBGAFR_POSITION_MASK) >> 0x10;
  tmp = GPIO_Remap & LSB_MASK;

  if ((GPIO_Remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK))
  {
    tmpreg &= DBGAFR_SWJCFG_MASK;
    AFIO->MAPR &= DBGAFR_SWJCFG_MASK;
  }
  else if ((GPIO_Remap & DBGAFR_NUMBITS_MASK) == DBGAFR_NUMBITS_MASK)
  {
    tmp1 = ((uint32_t)0x03) << tmpmask;
    tmpreg &= ~tmp1;
    tmpreg |= ~DBGAFR_SWJCFG_MASK;
  }
  else
  {
    tmpreg &= ~(tmp << ((GPIO_Remap >> 0x15)*0x10));
    tmpreg |= ~DBGAFR_SWJCFG_MASK;
  }

  if (NewState != DISABLE)
  {
    tmpreg |= (tmp << ((GPIO_Remap >> 0x15)*0x10));
  }

  if((GPIO_Remap & 0x80000000) == 0x80000000)
  {
    AFIO->MAPR2 = tmpreg;
  }
  else
  {
    AFIO->MAPR = tmpreg;
  }
}

//STD FSMC
/**
  * @brief Flexible Static Memory Controller
  */

/**
  * @brief  Timing parameters For NOR/SRAM Banks
  */

typedef struct
{
  uint32_t FSMC_AddressSetupTime;       /*!< Defines the number of HCLK cycles to configure
                                             the duration of the address setup time.
                                             This parameter can be a value between 0 and 0xF.
                                             @note: It is not used with synchronous NOR Flash memories. */

  uint32_t FSMC_AddressHoldTime;        /*!< Defines the number of HCLK cycles to configure
                                             the duration of the address hold time.
                                             This parameter can be a value between 0 and 0xF.
                                             @note: It is not used with synchronous NOR Flash memories.*/

  uint32_t FSMC_DataSetupTime;          /*!< Defines the number of HCLK cycles to configure
                                             the duration of the data setup time.
                                             This parameter can be a value between 0 and 0xFF.
                                             @note: It is used for SRAMs, ROMs and asynchronous multiplexed NOR Flash memories. */

  uint32_t FSMC_BusTurnAroundDuration;  /*!< Defines the number of HCLK cycles to configure
                                             the duration of the bus turnaround.
                                             This parameter can be a value between 0 and 0xF.
                                             @note: It is only used for multiplexed NOR Flash memories. */

  uint32_t FSMC_CLKDivision;            /*!< Defines the period of CLK clock output signal, expressed in number of HCLK cycles.
                                             This parameter can be a value between 1 and 0xF.
                                             @note: This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses. */

  uint32_t FSMC_DataLatency;            /*!< Defines the number of memory clock cycles to issue
                                             to the memory before getting the first data.
                                             The value of this parameter depends on the memory type as shown below:
                                              - It must be set to 0 in case of a CRAM
                                              - It is don't care in asynchronous NOR, SRAM or ROM accesses
                                              - It may assume a value between 0 and 0xF in NOR Flash memories
                                                with synchronous burst mode enable */

  uint32_t FSMC_AccessMode;             /*!< Specifies the asynchronous access mode.
                                             This parameter can be a value of @ref FSMC_Access_Mode */
}FSMC_NORSRAMTimingInitTypeDef;

/**
  * @brief  FSMC NOR/SRAM Init structure definition
  */

typedef struct
{
  uint32_t FSMC_Bank;                /*!< Specifies the NOR/SRAM memory bank that will be used.
                                          This parameter can be a value of @ref FSMC_NORSRAM_Bank */

  uint32_t FSMC_DataAddressMux;      /*!< Specifies whether the address and data values are
                                          multiplexed on the databus or not.
                                          This parameter can be a value of @ref FSMC_Data_Address_Bus_Multiplexing */

  uint32_t FSMC_MemoryType;          /*!< Specifies the type of external memory attached to
                                          the corresponding memory bank.
                                          This parameter can be a value of @ref FSMC_Memory_Type */

  uint32_t FSMC_MemoryDataWidth;     /*!< Specifies the external memory device width.
                                          This parameter can be a value of @ref FSMC_Data_Width */

  uint32_t FSMC_BurstAccessMode;     /*!< Enables or disables the burst access mode for Flash memory,
                                          valid only with synchronous burst Flash memories.
                                          This parameter can be a value of @ref FSMC_Burst_Access_Mode */

  uint32_t FSMC_AsynchronousWait;     /*!< Enables or disables wait signal during asynchronous transfers,
                                          valid only with asynchronous Flash memories.
                                          This parameter can be a value of @ref FSMC_AsynchronousWait */

  uint32_t FSMC_WaitSignalPolarity;  /*!< Specifies the wait signal polarity, valid only when accessing
                                          the Flash memory in burst mode.
                                          This parameter can be a value of @ref FSMC_Wait_Signal_Polarity */

  uint32_t FSMC_WrapMode;            /*!< Enables or disables the Wrapped burst access mode for Flash
                                          memory, valid only when accessing Flash memories in burst mode.
                                          This parameter can be a value of @ref FSMC_Wrap_Mode */

  uint32_t FSMC_WaitSignalActive;    /*!< Specifies if the wait signal is asserted by the memory one
                                          clock cycle before the wait state or during the wait state,
                                          valid only when accessing memories in burst mode.
                                          This parameter can be a value of @ref FSMC_Wait_Timing */

  uint32_t FSMC_WriteOperation;      /*!< Enables or disables the write operation in the selected bank by the FSMC.
                                          This parameter can be a value of @ref FSMC_Write_Operation */

  uint32_t FSMC_WaitSignal;          /*!< Enables or disables the wait-state insertion via wait
                                          signal, valid for Flash memory access in burst mode.
                                          This parameter can be a value of @ref FSMC_Wait_Signal */

  uint32_t FSMC_ExtendedMode;        /*!< Enables or disables the extended mode.
                                          This parameter can be a value of @ref FSMC_Extended_Mode */

  uint32_t FSMC_WriteBurst;          /*!< Enables or disables the write burst operation.
                                          This parameter can be a value of @ref FSMC_Write_Burst */

  FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct; /*!< Timing Parameters for write and read access if the  ExtendedMode is not used*/

  FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;     /*!< Timing Parameters for write access if the  ExtendedMode is used*/
}FSMC_NORSRAMInitTypeDef;

/**
  * @brief  Timing parameters For FSMC NAND and PCCARD Banks
  */

typedef struct
{
  uint32_t FSMC_SetupTime;      /*!< Defines the number of HCLK cycles to setup address before
                                     the command assertion for NAND-Flash read or write access
                                     to common/Attribute or I/O memory space (depending on
                                     the memory space timing to be configured).
                                     This parameter can be a value between 0 and 0xFF.*/

  uint32_t FSMC_WaitSetupTime;  /*!< Defines the minimum number of HCLK cycles to assert the
                                     command for NAND-Flash read or write access to
                                     common/Attribute or I/O memory space (depending on the
                                     memory space timing to be configured).
                                     This parameter can be a number between 0x00 and 0xFF */

  uint32_t FSMC_HoldSetupTime;  /*!< Defines the number of HCLK clock cycles to hold address
                                     (and data for write access) after the command deassertion
                                     for NAND-Flash read or write access to common/Attribute
                                     or I/O memory space (depending on the memory space timing
                                     to be configured).
                                     This parameter can be a number between 0x00 and 0xFF */

  uint32_t FSMC_HiZSetupTime;   /*!< Defines the number of HCLK clock cycles during which the
                                     databus is kept in HiZ after the start of a NAND-Flash
                                     write access to common/Attribute or I/O memory space (depending
                                     on the memory space timing to be configured).
                                     This parameter can be a number between 0x00 and 0xFF */
}FSMC_NAND_PCCARDTimingInitTypeDef;

/**
  * @brief  FSMC NAND Init structure definition
  */

typedef struct
{
  uint32_t FSMC_Bank;              /*!< Specifies the NAND memory bank that will be used.
                                      This parameter can be a value of @ref FSMC_NAND_Bank */

  uint32_t FSMC_Waitfeature;      /*!< Enables or disables the Wait feature for the NAND Memory Bank.
                                       This parameter can be any value of @ref FSMC_Wait_feature */

  uint32_t FSMC_MemoryDataWidth;  /*!< Specifies the external memory device width.
                                       This parameter can be any value of @ref FSMC_Data_Width */

  uint32_t FSMC_ECC;              /*!< Enables or disables the ECC computation.
                                       This parameter can be any value of @ref FSMC_ECC */

  uint32_t FSMC_ECCPageSize;      /*!< Defines the page size for the extended ECC.
                                       This parameter can be any value of @ref FSMC_ECC_Page_Size */

  uint32_t FSMC_TCLRSetupTime;    /*!< Defines the number of HCLK cycles to configure the
                                       delay between CLE low and RE low.
                                       This parameter can be a value between 0 and 0xFF. */

  uint32_t FSMC_TARSetupTime;     /*!< Defines the number of HCLK cycles to configure the
                                       delay between ALE low and RE low.
                                       This parameter can be a number between 0x0 and 0xFF */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_CommonSpaceTimingStruct;   /*!< FSMC Common Space Timing */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_AttributeSpaceTimingStruct; /*!< FSMC Attribute Space Timing */
}FSMC_NANDInitTypeDef;

/**
  * @brief  FSMC PCCARD Init structure definition
  */

typedef struct
{
  uint32_t FSMC_Waitfeature;    /*!< Enables or disables the Wait feature for the Memory Bank.
                                    This parameter can be any value of @ref FSMC_Wait_feature */

  uint32_t FSMC_TCLRSetupTime;  /*!< Defines the number of HCLK cycles to configure the
                                     delay between CLE low and RE low.
                                     This parameter can be a value between 0 and 0xFF. */

  uint32_t FSMC_TARSetupTime;   /*!< Defines the number of HCLK cycles to configure the
                                     delay between ALE low and RE low.
                                     This parameter can be a number between 0x0 and 0xFF */


  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_CommonSpaceTimingStruct; /*!< FSMC Common Space Timing */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_AttributeSpaceTimingStruct;  /*!< FSMC Attribute Space Timing */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_IOSpaceTimingStruct; /*!< FSMC IO Space Timing */
}FSMC_PCCARDInitTypeDef;

/**
  * @}
  */

/** @defgroup FSMC_Exported_Constants
  * @{
  */

/** @defgroup FSMC_NORSRAM_Bank
  * @{
  */
#define FSMC_Bank1_NORSRAM1                             ((uint32_t)0x00000000)
#define FSMC_Bank1_NORSRAM2                             ((uint32_t)0x00000002)
#define FSMC_Bank1_NORSRAM3                             ((uint32_t)0x00000004)
#define FSMC_Bank1_NORSRAM4                             ((uint32_t)0x00000006)
/**
  * @}
  */

/** @defgroup FSMC_NAND_Bank
  * @{
  */
#define FSMC_Bank2_NAND                                 ((uint32_t)0x00000010)
#define FSMC_Bank3_NAND                                 ((uint32_t)0x00000100)
/**
  * @}
  */

/** @defgroup FSMC_PCCARD_Bank
  * @{
  */
#define FSMC_Bank4_PCCARD                               ((uint32_t)0x00001000)
/**
  * @}
  */

#define IS_FSMC_NORSRAM_BANK(BANK) (((BANK) == FSMC_Bank1_NORSRAM1) || \
                                    ((BANK) == FSMC_Bank1_NORSRAM2) || \
                                    ((BANK) == FSMC_Bank1_NORSRAM3) || \
                                    ((BANK) == FSMC_Bank1_NORSRAM4))

#define IS_FSMC_NAND_BANK(BANK) (((BANK) == FSMC_Bank2_NAND) || \
                                 ((BANK) == FSMC_Bank3_NAND))

#define IS_FSMC_GETFLAG_BANK(BANK) (((BANK) == FSMC_Bank2_NAND) || \
                                    ((BANK) == FSMC_Bank3_NAND) || \
                                    ((BANK) == FSMC_Bank4_PCCARD))

#define IS_FSMC_IT_BANK(BANK) (((BANK) == FSMC_Bank2_NAND) || \
                               ((BANK) == FSMC_Bank3_NAND) || \
                               ((BANK) == FSMC_Bank4_PCCARD))

/** @defgroup NOR_SRAM_Controller
  * @{
  */

/** @defgroup FSMC_Data_Address_Bus_Multiplexing
  * @{
  */

#define FSMC_DataAddressMux_Disable                       ((uint32_t)0x00000000)
#define FSMC_DataAddressMux_Enable                        ((uint32_t)0x00000002)
#define IS_FSMC_MUX(MUX) (((MUX) == FSMC_DataAddressMux_Disable) || \
                          ((MUX) == FSMC_DataAddressMux_Enable))

/**
  * @}
  */

/** @defgroup FSMC_Memory_Type
  * @{
  */

#define FSMC_MemoryType_SRAM                            ((uint32_t)0x00000000)
#define FSMC_MemoryType_PSRAM                           ((uint32_t)0x00000004)
#define FSMC_MemoryType_NOR                             ((uint32_t)0x00000008)
#define IS_FSMC_MEMORY(MEMORY) (((MEMORY) == FSMC_MemoryType_SRAM) || \
                                ((MEMORY) == FSMC_MemoryType_PSRAM)|| \
                                ((MEMORY) == FSMC_MemoryType_NOR))

/**
  * @}
  */

/** @defgroup FSMC_Data_Width
  * @{
  */

#define FSMC_MemoryDataWidth_8b                         ((uint32_t)0x00000000)
#define FSMC_MemoryDataWidth_16b                        ((uint32_t)0x00000010)
#define IS_FSMC_MEMORY_WIDTH(WIDTH) (((WIDTH) == FSMC_MemoryDataWidth_8b) || \
                                     ((WIDTH) == FSMC_MemoryDataWidth_16b))

/**
  * @}
  */

/** @defgroup FSMC_Burst_Access_Mode
  * @{
  */

#define FSMC_BurstAccessMode_Disable                    ((uint32_t)0x00000000)
#define FSMC_BurstAccessMode_Enable                     ((uint32_t)0x00000100)
#define IS_FSMC_BURSTMODE(STATE) (((STATE) == FSMC_BurstAccessMode_Disable) || \
                                  ((STATE) == FSMC_BurstAccessMode_Enable))
/**
  * @}
  */

/** @defgroup FSMC_AsynchronousWait
  * @{
  */
#define FSMC_AsynchronousWait_Disable                   ((uint32_t)0x00000000)
#define FSMC_AsynchronousWait_Enable                    ((uint32_t)0x00008000)
#define IS_FSMC_ASYNWAIT(STATE) (((STATE) == FSMC_AsynchronousWait_Disable) || \
                                 ((STATE) == FSMC_AsynchronousWait_Enable))

/**
  * @}
  */

/** @defgroup FSMC_Wait_Signal_Polarity
  * @{
  */

#define FSMC_WaitSignalPolarity_Low                     ((uint32_t)0x00000000)
#define FSMC_WaitSignalPolarity_High                    ((uint32_t)0x00000200)
#define IS_FSMC_WAIT_POLARITY(POLARITY) (((POLARITY) == FSMC_WaitSignalPolarity_Low) || \
                                         ((POLARITY) == FSMC_WaitSignalPolarity_High))

/**
  * @}
  */

/** @defgroup FSMC_Wrap_Mode
  * @{
  */

#define FSMC_WrapMode_Disable                           ((uint32_t)0x00000000)
#define FSMC_WrapMode_Enable                            ((uint32_t)0x00000400)
#define IS_FSMC_WRAP_MODE(MODE) (((MODE) == FSMC_WrapMode_Disable) || \
                                 ((MODE) == FSMC_WrapMode_Enable))

/**
  * @}
  */

/** @defgroup FSMC_Wait_Timing
  * @{
  */

#define FSMC_WaitSignalActive_BeforeWaitState           ((uint32_t)0x00000000)
#define FSMC_WaitSignalActive_DuringWaitState           ((uint32_t)0x00000800)
#define IS_FSMC_WAIT_SIGNAL_ACTIVE(ACTIVE) (((ACTIVE) == FSMC_WaitSignalActive_BeforeWaitState) || \
                                            ((ACTIVE) == FSMC_WaitSignalActive_DuringWaitState))

/**
  * @}
  */

/** @defgroup FSMC_Write_Operation
  * @{
  */

#define FSMC_WriteOperation_Disable                     ((uint32_t)0x00000000)
#define FSMC_WriteOperation_Enable                      ((uint32_t)0x00001000)
#define IS_FSMC_WRITE_OPERATION(OPERATION) (((OPERATION) == FSMC_WriteOperation_Disable) || \
                                            ((OPERATION) == FSMC_WriteOperation_Enable))

/**
  * @}
  */

/** @defgroup FSMC_Wait_Signal
  * @{
  */

#define FSMC_WaitSignal_Disable                         ((uint32_t)0x00000000)
#define FSMC_WaitSignal_Enable                          ((uint32_t)0x00002000)
#define IS_FSMC_WAITE_SIGNAL(SIGNAL) (((SIGNAL) == FSMC_WaitSignal_Disable) || \
                                      ((SIGNAL) == FSMC_WaitSignal_Enable))
/**
  * @}
  */

/** @defgroup FSMC_Extended_Mode
  * @{
  */

#define FSMC_ExtendedMode_Disable                       ((uint32_t)0x00000000)
#define FSMC_ExtendedMode_Enable                        ((uint32_t)0x00004000)

#define IS_FSMC_EXTENDED_MODE(MODE) (((MODE) == FSMC_ExtendedMode_Disable) || \
                                     ((MODE) == FSMC_ExtendedMode_Enable))

/**
  * @}
  */

/** @defgroup FSMC_Write_Burst
  * @{
  */

#define FSMC_WriteBurst_Disable                         ((uint32_t)0x00000000)
#define FSMC_WriteBurst_Enable                          ((uint32_t)0x00080000)
#define IS_FSMC_WRITE_BURST(BURST) (((BURST) == FSMC_WriteBurst_Disable) || \
                                    ((BURST) == FSMC_WriteBurst_Enable))
/**
  * @}
  */

/** @defgroup FSMC_Address_Setup_Time
  * @{
  */

#define IS_FSMC_ADDRESS_SETUP_TIME(TIME) ((TIME) <= 0xF)

/**
  * @}
  */

/** @defgroup FSMC_Address_Hold_Time
  * @{
  */

#define IS_FSMC_ADDRESS_HOLD_TIME(TIME) ((TIME) <= 0xF)

/**
  * @}
  */

/** @defgroup FSMC_Data_Setup_Time
  * @{
  */

#define IS_FSMC_DATASETUP_TIME(TIME) (((TIME) > 0) && ((TIME) <= 0xFF))

/**
  * @}
  */

/** @defgroup FSMC_Bus_Turn_around_Duration
  * @{
  */

#define IS_FSMC_TURNAROUND_TIME(TIME) ((TIME) <= 0xF)

/**
  * @}
  */

/** @defgroup FSMC_CLK_Division
  * @{
  */

#define IS_FSMC_CLK_DIV(DIV) ((DIV) <= 0xF)

/**
  * @}
  */

/** @defgroup FSMC_Data_Latency
  * @{
  */

#define IS_FSMC_DATA_LATENCY(LATENCY) ((LATENCY) <= 0xF)

/**
  * @}
  */

/** @defgroup FSMC_Access_Mode
  * @{
  */

#define FSMC_AccessMode_A                               ((uint32_t)0x00000000)
#define FSMC_AccessMode_B                               ((uint32_t)0x10000000)
#define FSMC_AccessMode_C                               ((uint32_t)0x20000000)
#define FSMC_AccessMode_D                               ((uint32_t)0x30000000)
#define IS_FSMC_ACCESS_MODE(MODE) (((MODE) == FSMC_AccessMode_A) || \
                                   ((MODE) == FSMC_AccessMode_B) || \
                                   ((MODE) == FSMC_AccessMode_C) || \
                                   ((MODE) == FSMC_AccessMode_D))

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup NAND_PCCARD_Controller
  * @{
  */

/** @defgroup FSMC_Wait_feature
  * @{
  */

#define FSMC_Waitfeature_Disable                        ((uint32_t)0x00000000)
#define FSMC_Waitfeature_Enable                         ((uint32_t)0x00000002)
#define IS_FSMC_WAIT_FEATURE(FEATURE) (((FEATURE) == FSMC_Waitfeature_Disable) || \
                                       ((FEATURE) == FSMC_Waitfeature_Enable))

/**
  * @}
  */


/** @defgroup FSMC_ECC
  * @{
  */

#define FSMC_ECC_Disable                                ((uint32_t)0x00000000)
#define FSMC_ECC_Enable                                 ((uint32_t)0x00000040)
#define IS_FSMC_ECC_STATE(STATE) (((STATE) == FSMC_ECC_Disable) || \
                                  ((STATE) == FSMC_ECC_Enable))

/**
  * @}
  */

/** @defgroup FSMC_ECC_Page_Size
  * @{
  */

#define FSMC_ECCPageSize_256Bytes                       ((uint32_t)0x00000000)
#define FSMC_ECCPageSize_512Bytes                       ((uint32_t)0x00020000)
#define FSMC_ECCPageSize_1024Bytes                      ((uint32_t)0x00040000)
#define FSMC_ECCPageSize_2048Bytes                      ((uint32_t)0x00060000)
#define FSMC_ECCPageSize_4096Bytes                      ((uint32_t)0x00080000)
#define FSMC_ECCPageSize_8192Bytes                      ((uint32_t)0x000A0000)
#define IS_FSMC_ECCPAGE_SIZE(SIZE) (((SIZE) == FSMC_ECCPageSize_256Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_512Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_1024Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_2048Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_4096Bytes) || \
                                    ((SIZE) == FSMC_ECCPageSize_8192Bytes))

/**
  * @}
  */

/** @defgroup FSMC_TCLR_Setup_Time
  * @{
  */

#define IS_FSMC_TCLR_TIME(TIME) ((TIME) <= 0xFF)

/**
  * @}
  */

/** @defgroup FSMC_TAR_Setup_Time
  * @{
  */

#define IS_FSMC_TAR_TIME(TIME) ((TIME) <= 0xFF)

/**
  * @}
  */

/** @defgroup FSMC_Setup_Time
  * @{
  */

#define IS_FSMC_SETUP_TIME(TIME) ((TIME) <= 0xFF)

/**
  * @}
  */

/** @defgroup FSMC_Wait_Setup_Time
  * @{
  */

#define IS_FSMC_WAIT_TIME(TIME) ((TIME) <= 0xFF)

/**
  * @}
  */

/** @defgroup FSMC_Hold_Setup_Time
  * @{
  */

#define IS_FSMC_HOLD_TIME(TIME) ((TIME) <= 0xFF)

/**
  * @}
  */

/** @defgroup FSMC_HiZ_Setup_Time
  * @{
  */

#define IS_FSMC_HIZ_TIME(TIME) ((TIME) <= 0xFF)

/**
  * @}
  */

/** @defgroup FSMC_Interrupt_sources
  * @{
  */

#define FSMC_IT_RisingEdge                              ((uint32_t)0x00000008)
#define FSMC_IT_Level                                   ((uint32_t)0x00000010)
#define FSMC_IT_FallingEdge                             ((uint32_t)0x00000020)
#define IS_FSMC_IT(IT) ((((IT) & (uint32_t)0xFFFFFFC7) == 0x00000000) && ((IT) != 0x00000000))
#define IS_FSMC_GET_IT(IT) (((IT) == FSMC_IT_RisingEdge) || \
                            ((IT) == FSMC_IT_Level) || \
                            ((IT) == FSMC_IT_FallingEdge))
/**
  * @}
  */

/** @defgroup FSMC_Flags
  * @{
  */

#define FSMC_FLAG_RisingEdge                            ((uint32_t)0x00000001)
#define FSMC_FLAG_Level                                 ((uint32_t)0x00000002)
#define FSMC_FLAG_FallingEdge                           ((uint32_t)0x00000004)
#define FSMC_FLAG_FEMPT                                 ((uint32_t)0x00000040)
#define IS_FSMC_GET_FLAG(FLAG) (((FLAG) == FSMC_FLAG_RisingEdge) || \
                                ((FLAG) == FSMC_FLAG_Level) || \
                                ((FLAG) == FSMC_FLAG_FallingEdge) || \
                                ((FLAG) == FSMC_FLAG_FEMPT))

#define IS_FSMC_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFF8) == 0x00000000) && ((FLAG) != 0x00000000))


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


typedef struct
{
  __IO uint32_t PCR2;
  __IO uint32_t SR2;
  __IO uint32_t PMEM2;
  __IO uint32_t PATT2;
  uint32_t  RESERVED0;
  __IO uint32_t ECCR2;
} FSMC_Bank2_TypeDef;

/**
  * @brief Flexible Static Memory Controller Bank3
  */

typedef struct
{
  __IO uint32_t PCR3;
  __IO uint32_t SR3;
  __IO uint32_t PMEM3;
  __IO uint32_t PATT3;
  uint32_t  RESERVED0;
  __IO uint32_t ECCR3;
} FSMC_Bank3_TypeDef;



#define FSMC_Bank2_R_BASE     (FSMC_R_BASE + 0x0060) /*!< FSMC Bank2 registers base address */
#define FSMC_Bank3_R_BASE     (FSMC_R_BASE + 0x0080) /*!< FSMC Bank3 registers base address */

#define FSMC_Bank2			((FSMC_Bank2_TypeDef *) FSMC_Bank2_R_BASE)
#define FSMC_Bank3			((FSMC_Bank3_TypeDef *) FSMC_Bank3_R_BASE)

/**
  * @brief  Deinitializes the FSMC NOR/SRAM Banks registers to their default
  *         reset values.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank1_NORSRAM1: FSMC Bank1 NOR/SRAM1
  *     @arg FSMC_Bank1_NORSRAM2: FSMC Bank1 NOR/SRAM2
  *     @arg FSMC_Bank1_NORSRAM3: FSMC Bank1 NOR/SRAM3
  *     @arg FSMC_Bank1_NORSRAM4: FSMC Bank1 NOR/SRAM4
  * @retval None
  */
void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank)
{
  /* Check the parameter */
  assert_param(IS_FSMC_NORSRAM_BANK(FSMC_Bank));

  /* FSMC_Bank1_NORSRAM1 */
  if(FSMC_Bank == FSMC_Bank1_NORSRAM1)
  {
    FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030DB;
  }
  /* FSMC_Bank1_NORSRAM2,  FSMC_Bank1_NORSRAM3 or FSMC_Bank1_NORSRAM4 */
  else
  {
    FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030D2;
  }
  FSMC_Bank1->BTCR[FSMC_Bank + 1] = 0x0FFFFFFF;
  FSMC_Bank1E->BWTR[FSMC_Bank] = 0x0FFFFFFF;
}

/**
  * @brief  Deinitializes the FSMC NAND Banks registers to their default reset values.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  * @retval None
  */
void FSMC_NANDDeInit(uint32_t FSMC_Bank)
{
  /* Check the parameter */
  assert_param(IS_FSMC_NAND_BANK(FSMC_Bank));

  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    /* Set the FSMC_Bank2 registers to their reset values */
    FSMC_Bank2->PCR2 = 0x00000018;
    FSMC_Bank2->SR2 = 0x00000040;
    FSMC_Bank2->PMEM2 = 0xFCFCFCFC;
    FSMC_Bank2->PATT2 = 0xFCFCFCFC;
  }
  /* FSMC_Bank3_NAND */
  else
  {
    /* Set the FSMC_Bank3 registers to their reset values */
    FSMC_Bank3->PCR3 = 0x00000018;
    FSMC_Bank3->SR3 = 0x00000040;
    FSMC_Bank3->PMEM3 = 0xFCFCFCFC;
    FSMC_Bank3->PATT3 = 0xFCFCFCFC;
  }
}

/**
  * @brief  Deinitializes the FSMC PCCARD Bank registers to their default reset values.
  * @param  None
  * @retval None
  */
void FSMC_PCCARDDeInit(void)
{
  /* Set the FSMC_Bank4 registers to their reset values */
  FSMC_Bank4->PCR4 = 0x00000018;
  FSMC_Bank4->SR4 = 0x00000000;
  FSMC_Bank4->PMEM4 = 0xFCFCFCFC;
  FSMC_Bank4->PATT4 = 0xFCFCFCFC;
  FSMC_Bank4->PIO4 = 0xFCFCFCFC;
}

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
  /* Check the parameters */
  assert_param(IS_FSMC_NORSRAM_BANK(FSMC_NORSRAMInitStruct->FSMC_Bank));
  assert_param(IS_FSMC_MUX(FSMC_NORSRAMInitStruct->FSMC_DataAddressMux));
  assert_param(IS_FSMC_MEMORY(FSMC_NORSRAMInitStruct->FSMC_MemoryType));
  assert_param(IS_FSMC_MEMORY_WIDTH(FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth));
  assert_param(IS_FSMC_BURSTMODE(FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode));
  assert_param(IS_FSMC_ASYNWAIT(FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait));
  assert_param(IS_FSMC_WAIT_POLARITY(FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity));
  assert_param(IS_FSMC_WRAP_MODE(FSMC_NORSRAMInitStruct->FSMC_WrapMode));
  assert_param(IS_FSMC_WAIT_SIGNAL_ACTIVE(FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive));
  assert_param(IS_FSMC_WRITE_OPERATION(FSMC_NORSRAMInitStruct->FSMC_WriteOperation));
  assert_param(IS_FSMC_WAITE_SIGNAL(FSMC_NORSRAMInitStruct->FSMC_WaitSignal));
  assert_param(IS_FSMC_EXTENDED_MODE(FSMC_NORSRAMInitStruct->FSMC_ExtendedMode));
  assert_param(IS_FSMC_WRITE_BURST(FSMC_NORSRAMInitStruct->FSMC_WriteBurst));
  assert_param(IS_FSMC_ADDRESS_SETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime));
  assert_param(IS_FSMC_ADDRESS_HOLD_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime));
  assert_param(IS_FSMC_DATASETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime));
  assert_param(IS_FSMC_TURNAROUND_TIME(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration));
  assert_param(IS_FSMC_CLK_DIV(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision));
  assert_param(IS_FSMC_DATA_LATENCY(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency));
  assert_param(IS_FSMC_ACCESS_MODE(FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode));

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
    assert_param(IS_FSMC_ADDRESS_SETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime));
    assert_param(IS_FSMC_ADDRESS_HOLD_TIME(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime));
    assert_param(IS_FSMC_DATASETUP_TIME(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime));
    assert_param(IS_FSMC_CLK_DIV(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision));
    assert_param(IS_FSMC_DATA_LATENCY(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency));
    assert_param(IS_FSMC_ACCESS_MODE(FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode));
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
  * @brief  Initializes the FSMC NAND Banks according to the specified
  *         parameters in the FSMC_NANDInitStruct.
  * @param  FSMC_NANDInitStruct : pointer to a FSMC_NANDInitTypeDef
  *         structure that contains the configuration information for the FSMC
  *         NAND specified Banks.
  * @retval None
  */
void FSMC_NANDInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct)
{
  uint32_t tmppcr = 0x00000000, tmppmem = 0x00000000, tmppatt = 0x00000000;

  /* Check the parameters */
  assert_param( IS_FSMC_NAND_BANK(FSMC_NANDInitStruct->FSMC_Bank));
  assert_param( IS_FSMC_WAIT_FEATURE(FSMC_NANDInitStruct->FSMC_Waitfeature));
  assert_param( IS_FSMC_MEMORY_WIDTH(FSMC_NANDInitStruct->FSMC_MemoryDataWidth));
  assert_param( IS_FSMC_ECC_STATE(FSMC_NANDInitStruct->FSMC_ECC));
  assert_param( IS_FSMC_ECCPAGE_SIZE(FSMC_NANDInitStruct->FSMC_ECCPageSize));
  assert_param( IS_FSMC_TCLR_TIME(FSMC_NANDInitStruct->FSMC_TCLRSetupTime));
  assert_param( IS_FSMC_TAR_TIME(FSMC_NANDInitStruct->FSMC_TARSetupTime));
  assert_param(IS_FSMC_SETUP_TIME(FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime));
  assert_param(IS_FSMC_SETUP_TIME(FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime));

  /* Set the tmppcr value according to FSMC_NANDInitStruct parameters */
  tmppcr = (uint32_t)FSMC_NANDInitStruct->FSMC_Waitfeature |
            PCR_MemoryType_NAND |
            FSMC_NANDInitStruct->FSMC_MemoryDataWidth |
            FSMC_NANDInitStruct->FSMC_ECC |
            FSMC_NANDInitStruct->FSMC_ECCPageSize |
            (FSMC_NANDInitStruct->FSMC_TCLRSetupTime << 9 )|
            (FSMC_NANDInitStruct->FSMC_TARSetupTime << 13);

  /* Set tmppmem value according to FSMC_CommonSpaceTimingStructure parameters */
  tmppmem = (uint32_t)FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime |
            (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
            (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime << 16)|
            (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  /* Set tmppatt value according to FSMC_AttributeSpaceTimingStructure parameters */
  tmppatt = (uint32_t)FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime |
            (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
            (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime << 16)|
            (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  if(FSMC_NANDInitStruct->FSMC_Bank == FSMC_Bank2_NAND)
  {
    /* FSMC_Bank2_NAND registers configuration */
    FSMC_Bank2->PCR2 = tmppcr;
    FSMC_Bank2->PMEM2 = tmppmem;
    FSMC_Bank2->PATT2 = tmppatt;
  }
  else
  {
    /* FSMC_Bank3_NAND registers configuration */
    FSMC_Bank3->PCR3 = tmppcr;
    FSMC_Bank3->PMEM3 = tmppmem;
    FSMC_Bank3->PATT3 = tmppatt;
  }
}

/**
  * @brief  Initializes the FSMC PCCARD Bank according to the specified
  *         parameters in the FSMC_PCCARDInitStruct.
  * @param  FSMC_PCCARDInitStruct : pointer to a FSMC_PCCARDInitTypeDef
  *         structure that contains the configuration information for the FSMC
  *         PCCARD Bank.
  * @retval None
  */
void FSMC_PCCARDInit(FSMC_PCCARDInitTypeDef* FSMC_PCCARDInitStruct)
{
  /* Check the parameters */
  assert_param(IS_FSMC_WAIT_FEATURE(FSMC_PCCARDInitStruct->FSMC_Waitfeature));
  assert_param(IS_FSMC_TCLR_TIME(FSMC_PCCARDInitStruct->FSMC_TCLRSetupTime));
  assert_param(IS_FSMC_TAR_TIME(FSMC_PCCARDInitStruct->FSMC_TARSetupTime));

  assert_param(IS_FSMC_SETUP_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime));

  assert_param(IS_FSMC_SETUP_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime));
  assert_param(IS_FSMC_SETUP_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_SetupTime));
  assert_param(IS_FSMC_WAIT_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_WaitSetupTime));
  assert_param(IS_FSMC_HOLD_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HoldSetupTime));
  assert_param(IS_FSMC_HIZ_TIME(FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HiZSetupTime));

  /* Set the PCR4 register value according to FSMC_PCCARDInitStruct parameters */
  FSMC_Bank4->PCR4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_Waitfeature |
                     FSMC_MemoryDataWidth_16b |
                     (FSMC_PCCARDInitStruct->FSMC_TCLRSetupTime << 9) |
                     (FSMC_PCCARDInitStruct->FSMC_TARSetupTime << 13);

  /* Set PMEM4 register value according to FSMC_CommonSpaceTimingStructure parameters */
  FSMC_Bank4->PMEM4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime |
                      (FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
                      (FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime << 16)|
                      (FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  /* Set PATT4 register value according to FSMC_AttributeSpaceTimingStructure parameters */
  FSMC_Bank4->PATT4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime |
                      (FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
                      (FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime << 16)|
                      (FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime << 24);

  /* Set PIO4 register value according to FSMC_IOSpaceTimingStructure parameters */
  FSMC_Bank4->PIO4 = (uint32_t)FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_SetupTime |
                     (FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
                     (FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HoldSetupTime << 16)|
                     (FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HiZSetupTime << 24);
}

/**
  * @brief  Fills each FSMC_NORSRAMInitStruct member with its default value.
  * @param  FSMC_NORSRAMInitStruct: pointer to a FSMC_NORSRAMInitTypeDef
  *         structure which will be initialized.
  * @retval None
  */
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct)
{
  /* Reset NOR/SRAM Init structure parameters values */
  FSMC_NORSRAMInitStruct->FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStruct->FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
  FSMC_NORSRAMInitStruct->FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStruct->FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStruct->FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStruct->FSMC_WaitSignal = FSMC_WaitSignal_Enable;
  FSMC_NORSRAMInitStruct->FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStruct->FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime = 0xFF;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode = FSMC_AccessMode_A;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime = 0xFF;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_BusTurnAroundDuration = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency = 0xF;
  FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode = FSMC_AccessMode_A;
}

/**
  * @brief  Fills each FSMC_NANDInitStruct member with its default value.
  * @param  FSMC_NANDInitStruct: pointer to a FSMC_NANDInitTypeDef
  *         structure which will be initialized.
  * @retval None
  */
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct)
{
  /* Reset NAND Init structure parameters values */
  FSMC_NANDInitStruct->FSMC_Bank = FSMC_Bank2_NAND;
  FSMC_NANDInitStruct->FSMC_Waitfeature = FSMC_Waitfeature_Disable;
  FSMC_NANDInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NANDInitStruct->FSMC_ECC = FSMC_ECC_Disable;
  FSMC_NANDInitStruct->FSMC_ECCPageSize = FSMC_ECCPageSize_256Bytes;
  FSMC_NANDInitStruct->FSMC_TCLRSetupTime = 0x0;
  FSMC_NANDInitStruct->FSMC_TARSetupTime = 0x0;
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
  FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
}

/**
  * @brief  Fills each FSMC_PCCARDInitStruct member with its default value.
  * @param  FSMC_PCCARDInitStruct: pointer to a FSMC_PCCARDInitTypeDef
  *         structure which will be initialized.
  * @retval None
  */
void FSMC_PCCARDStructInit(FSMC_PCCARDInitTypeDef* FSMC_PCCARDInitStruct)
{
  /* Reset PCCARD Init structure parameters values */
  FSMC_PCCARDInitStruct->FSMC_Waitfeature = FSMC_Waitfeature_Disable;
  FSMC_PCCARDInitStruct->FSMC_TCLRSetupTime = 0x0;
  FSMC_PCCARDInitStruct->FSMC_TARSetupTime = 0x0;
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_SetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
  FSMC_PCCARDInitStruct->FSMC_IOSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
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
  assert_param(IS_FSMC_NORSRAM_BANK(FSMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

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

/**
  * @brief  Enables or disables the specified NAND Memory Bank.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  * @param  NewState: new state of the FSMC_Bank. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
  assert_param(IS_FSMC_NAND_BANK(FSMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected NAND Bank by setting the PBKEN bit in the PCRx register */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 |= PCR_PBKEN_Set;
    }
    else
    {
      FSMC_Bank3->PCR3 |= PCR_PBKEN_Set;
    }
  }
  else
  {
    /* Disable the selected NAND Bank by clearing the PBKEN bit in the PCRx register */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 &= PCR_PBKEN_Reset;
    }
    else
    {
      FSMC_Bank3->PCR3 &= PCR_PBKEN_Reset;
    }
  }
}

/**
  * @brief  Enables or disables the PCCARD Memory Bank.
  * @param  NewState: new state of the PCCARD Memory Bank.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FSMC_PCCARDCmd(FunctionalState NewState)
{
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the PCCARD Bank by setting the PBKEN bit in the PCR4 register */
    FSMC_Bank4->PCR4 |= PCR_PBKEN_Set;
  }
  else
  {
    /* Disable the PCCARD Bank by clearing the PBKEN bit in the PCR4 register */
    FSMC_Bank4->PCR4 &= PCR_PBKEN_Reset;
  }
}

/**
  * @brief  Enables or disables the FSMC NAND ECC feature.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  * @param  NewState: new state of the FSMC NAND ECC feature.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
  assert_param(IS_FSMC_NAND_BANK(FSMC_Bank));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected NAND Bank ECC function by setting the ECCEN bit in the PCRx register */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 |= PCR_ECCEN_Set;
    }
    else
    {
      FSMC_Bank3->PCR3 |= PCR_ECCEN_Set;
    }
  }
  else
  {
    /* Disable the selected NAND Bank ECC function by clearing the ECCEN bit in the PCRx register */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->PCR2 &= PCR_ECCEN_Reset;
    }
    else
    {
      FSMC_Bank3->PCR3 &= PCR_ECCEN_Reset;
    }
  }
}

/**
  * @brief  Returns the error correction code register value.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  * @retval The Error Correction Code (ECC) value.
  */
uint32_t FSMC_GetECC(uint32_t FSMC_Bank)
{
  uint32_t eccval = 0x00000000;

  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    /* Get the ECCR2 register value */
    eccval = FSMC_Bank2->ECCR2;
  }
  else
  {
    /* Get the ECCR3 register value */
    eccval = FSMC_Bank3->ECCR3;
  }
  /* Return the error correction code value */
  return(eccval);
}

/**
  * @brief  Enables or disables the specified FSMC interrupts.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  *     @arg FSMC_Bank4_PCCARD: FSMC Bank4 PCCARD
  * @param  FSMC_IT: specifies the FSMC interrupt sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg FSMC_IT_RisingEdge: Rising edge detection interrupt.
  *     @arg FSMC_IT_Level: Level edge detection interrupt.
  *     @arg FSMC_IT_FallingEdge: Falling edge detection interrupt.
  * @param  NewState: new state of the specified FSMC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FSMC_ITConfig(uint32_t FSMC_Bank, uint32_t FSMC_IT, FunctionalState NewState)
{
  assert_param(IS_FSMC_IT_BANK(FSMC_Bank));
  assert_param(IS_FSMC_IT(FSMC_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected FSMC_Bank2 interrupts */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
      FSMC_Bank2->SR2 |= FSMC_IT;
    }
    /* Enable the selected FSMC_Bank3 interrupts */
    else if (FSMC_Bank == FSMC_Bank3_NAND)
    {
      FSMC_Bank3->SR3 |= FSMC_IT;
    }
    /* Enable the selected FSMC_Bank4 interrupts */
    else
    {
      FSMC_Bank4->SR4 |= FSMC_IT;
    }
  }
  else
  {
    /* Disable the selected FSMC_Bank2 interrupts */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {

      FSMC_Bank2->SR2 &= (uint32_t)~FSMC_IT;
    }
    /* Disable the selected FSMC_Bank3 interrupts */
    else if (FSMC_Bank == FSMC_Bank3_NAND)
    {
      FSMC_Bank3->SR3 &= (uint32_t)~FSMC_IT;
    }
    /* Disable the selected FSMC_Bank4 interrupts */
    else
    {
      FSMC_Bank4->SR4 &= (uint32_t)~FSMC_IT;
    }
  }
}

/**
  * @brief  Checks whether the specified FSMC flag is set or not.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  *     @arg FSMC_Bank4_PCCARD: FSMC Bank4 PCCARD
  * @param  FSMC_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg FSMC_FLAG_RisingEdge: Rising egde detection Flag.
  *     @arg FSMC_FLAG_Level: Level detection Flag.
  *     @arg FSMC_FLAG_FallingEdge: Falling egde detection Flag.
  *     @arg FSMC_FLAG_FEMPT: Fifo empty Flag.
  * @retval The new state of FSMC_FLAG (SET or RESET).
  */
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpsr = 0x00000000;

  /* Check the parameters */
  assert_param(IS_FSMC_GETFLAG_BANK(FSMC_Bank));
  assert_param(IS_FSMC_GET_FLAG(FSMC_FLAG));

  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    tmpsr = FSMC_Bank2->SR2;
  }
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    tmpsr = FSMC_Bank3->SR3;
  }
  /* FSMC_Bank4_PCCARD*/
  else
  {
    tmpsr = FSMC_Bank4->SR4;
  }

  /* Get the flag status */
  if ((tmpsr & FSMC_FLAG) != (uint16_t)RESET )
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Clears the FSMC's pending flags.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  *     @arg FSMC_Bank4_PCCARD: FSMC Bank4 PCCARD
  * @param  FSMC_FLAG: specifies the flag to clear.
  *   This parameter can be any combination of the following values:
  *     @arg FSMC_FLAG_RisingEdge: Rising egde detection Flag.
  *     @arg FSMC_FLAG_Level: Level detection Flag.
  *     @arg FSMC_FLAG_FallingEdge: Falling egde detection Flag.
  * @retval None
  */
void FSMC_ClearFlag(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
 /* Check the parameters */
  assert_param(IS_FSMC_GETFLAG_BANK(FSMC_Bank));
  assert_param(IS_FSMC_CLEAR_FLAG(FSMC_FLAG)) ;

  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    FSMC_Bank2->SR2 &= ~FSMC_FLAG;
  }
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    FSMC_Bank3->SR3 &= ~FSMC_FLAG;
  }
  /* FSMC_Bank4_PCCARD*/
  else
  {
    FSMC_Bank4->SR4 &= ~FSMC_FLAG;
  }
}

/**
  * @brief  Checks whether the specified FSMC interrupt has occurred or not.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  *     @arg FSMC_Bank4_PCCARD: FSMC Bank4 PCCARD
  * @param  FSMC_IT: specifies the FSMC interrupt source to check.
  *   This parameter can be one of the following values:
  *     @arg FSMC_IT_RisingEdge: Rising edge detection interrupt.
  *     @arg FSMC_IT_Level: Level edge detection interrupt.
  *     @arg FSMC_IT_FallingEdge: Falling edge detection interrupt.
  * @retval The new state of FSMC_IT (SET or RESET).
  */
ITStatus FSMC_GetITStatus(uint32_t FSMC_Bank, uint32_t FSMC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t tmpsr = 0x0, itstatus = 0x0, itenable = 0x0;

  /* Check the parameters */
  assert_param(IS_FSMC_IT_BANK(FSMC_Bank));
  assert_param(IS_FSMC_GET_IT(FSMC_IT));

  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    tmpsr = FSMC_Bank2->SR2;
  }
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    tmpsr = FSMC_Bank3->SR3;
  }
  /* FSMC_Bank4_PCCARD*/
  else
  {
    tmpsr = FSMC_Bank4->SR4;
  }

  itstatus = tmpsr & FSMC_IT;

  itenable = tmpsr & (FSMC_IT >> 3);
  if ((itstatus != (uint32_t)RESET)  && (itenable != (uint32_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears the FSMC's interrupt pending bits.
  * @param  FSMC_Bank: specifies the FSMC Bank to be used
  *   This parameter can be one of the following values:
  *     @arg FSMC_Bank2_NAND: FSMC Bank2 NAND
  *     @arg FSMC_Bank3_NAND: FSMC Bank3 NAND
  *     @arg FSMC_Bank4_PCCARD: FSMC Bank4 PCCARD
  * @param  FSMC_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg FSMC_IT_RisingEdge: Rising edge detection interrupt.
  *     @arg FSMC_IT_Level: Level edge detection interrupt.
  *     @arg FSMC_IT_FallingEdge: Falling edge detection interrupt.
  * @retval None
  */
void FSMC_ClearITPendingBit(uint32_t FSMC_Bank, uint32_t FSMC_IT)
{
  /* Check the parameters */
  assert_param(IS_FSMC_IT_BANK(FSMC_Bank));
  assert_param(IS_FSMC_IT(FSMC_IT));

  if(FSMC_Bank == FSMC_Bank2_NAND)
  {
    FSMC_Bank2->SR2 &= ~(FSMC_IT >> 3);
  }
  else if(FSMC_Bank == FSMC_Bank3_NAND)
  {
    FSMC_Bank3->SR3 &= ~(FSMC_IT >> 3);
  }
  /* FSMC_Bank4_PCCARD*/
  else
  {
    FSMC_Bank4->SR4 &= ~(FSMC_IT >> 3);
  }
}

//STD PWR

/** @defgroup PVD_detection_level
  * @{
  */

#define PWR_PVDLevel_2V2          ((uint32_t)0x00000000)
#define PWR_PVDLevel_2V3          ((uint32_t)0x00000020)
#define PWR_PVDLevel_2V4          ((uint32_t)0x00000040)
#define PWR_PVDLevel_2V5          ((uint32_t)0x00000060)
#define PWR_PVDLevel_2V6          ((uint32_t)0x00000080)
#define PWR_PVDLevel_2V7          ((uint32_t)0x000000A0)
#define PWR_PVDLevel_2V8          ((uint32_t)0x000000C0)
#define PWR_PVDLevel_2V9          ((uint32_t)0x000000E0)
#define IS_PWR_PVD_LEVEL(LEVEL) (((LEVEL) == PWR_PVDLevel_2V2) || ((LEVEL) == PWR_PVDLevel_2V3)|| \
                                 ((LEVEL) == PWR_PVDLevel_2V4) || ((LEVEL) == PWR_PVDLevel_2V5)|| \
                                 ((LEVEL) == PWR_PVDLevel_2V6) || ((LEVEL) == PWR_PVDLevel_2V7)|| \
                                 ((LEVEL) == PWR_PVDLevel_2V8) || ((LEVEL) == PWR_PVDLevel_2V9))
/**
  * @}
  */

/** @defgroup Regulator_state_is_STOP_mode
  * @{
  */

#define PWR_Regulator_ON          ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower    ((uint32_t)0x00000001)
#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_Regulator_ON) || \
                                     ((REGULATOR) == PWR_Regulator_LowPower))
/**
  * @}
  */

/** @defgroup STOP_mode_entry
  * @{
  */

#define PWR_STOPEntry_WFI         ((uint8_t)0x01)
#define PWR_STOPEntry_WFE         ((uint8_t)0x02)
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPEntry_WFI) || ((ENTRY) == PWR_STOPEntry_WFE))

/**
  * @}
  */

/** @defgroup PWR_Flag
  * @{
  */

#define PWR_FLAG_WU               ((uint32_t)0x00000001)
#define PWR_FLAG_SB               ((uint32_t)0x00000002)
#define PWR_FLAG_PVDO             ((uint32_t)0x00000004)
#define IS_PWR_GET_FLAG(FLAG) (((FLAG) == PWR_FLAG_WU) || ((FLAG) == PWR_FLAG_SB) || \
                               ((FLAG) == PWR_FLAG_PVDO))

#define IS_PWR_CLEAR_FLAG(FLAG) (((FLAG) == PWR_FLAG_WU) || ((FLAG) == PWR_FLAG_SB))


/* --------- PWR registers bit address in the alias region ---------- */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of DBP bit */
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

/* Alias word address of PVDE bit */
#define PVDE_BitNumber           0x04
#define CR_PVDE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PVDE_BitNumber * 4))

/* --- CSR Register ---*/

/* Alias word address of EWUP bit */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP_BitNumber           0x08
#define CSR_EWUP_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))

/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)


/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
void PWR_DeInit(void)
{
	  periph_clk_en(APB1,RCC_APB1Periph_PWR);
	  periph_clk_dis(APB1,RCC_APB1Periph_PWR);

//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
}

/**
  * @brief  Enables or disables access to the RTC and backup registers.
  * @param  NewState: new state of the access to the RTC and backup registers.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Power Voltage Detector(PVD).
  * @param  NewState: new state of the PVD.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_PVDCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param  PWR_PVDLevel: specifies the PVD detection level
  *   This parameter can be one of the following values:
  *     @arg PWR_PVDLevel_2V2: PVD detection level set to 2.2V
  *     @arg PWR_PVDLevel_2V3: PVD detection level set to 2.3V
  *     @arg PWR_PVDLevel_2V4: PVD detection level set to 2.4V
  *     @arg PWR_PVDLevel_2V5: PVD detection level set to 2.5V
  *     @arg PWR_PVDLevel_2V6: PVD detection level set to 2.6V
  *     @arg PWR_PVDLevel_2V7: PVD detection level set to 2.7V
  *     @arg PWR_PVDLevel_2V8: PVD detection level set to 2.8V
  *     @arg PWR_PVDLevel_2V9: PVD detection level set to 2.9V
  * @retval None
  */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_PVD_LEVEL(PWR_PVDLevel));
  tmpreg = PWR->CR;
  /* Clear PLS[7:5] bits */
  tmpreg &= CR_PLS_MASK;
  /* Set PLS[7:5] bits according to PWR_PVDLevel value */
  tmpreg |= PWR_PVDLevel;
  /* Store the new value */
  PWR->CR = tmpreg;
}

/**
  * @brief  Enables or disables the WakeUp Pin functionality.
  * @param  NewState: new state of the WakeUp Pin functionality.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_WakeUpPinCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  *(__IO uint32_t *) CSR_EWUP_BB = (uint32_t)NewState;
}

/**
  * @brief  Enters STOP mode.
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *   This parameter can be one of the following values:
  *     @arg PWR_Regulator_ON: STOP mode with regulator ON
  *     @arg PWR_Regulator_LowPower: STOP mode with regulator in low power mode
  * @param  PWR_STOPEntry: specifies if STOP mode in entered with WFI or WFE instruction.
  *   This parameter can be one of the following values:
  *     @arg PWR_STOPEntry_WFI: enter STOP mode with WFI instruction
  *     @arg PWR_STOPEntry_WFE: enter STOP mode with WFE instruction
  * @retval None
  */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(PWR_Regulator));
  assert_param(IS_PWR_STOP_ENTRY(PWR_STOPEntry));

  /* Select the regulator state in STOP mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  /* Set LPDS bit according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  /* Store the new value */
  PWR->CR = tmpreg;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;

  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __WFE();
  }

  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);
}

/**
  * @brief  Enters STANDBY mode.
  * @param  None
  * @retval None
  */
void PWR_EnterSTANDBYMode(void)
{
  /* Clear Wake-up flag */
  PWR->CR |= PWR_CR_CWUF;
  /* Select STANDBY mode */
  PWR->CR |= PWR_CR_PDDS;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;
/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
  __force_stores();
#endif
  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  *     @arg PWR_FLAG_PVDO: PVD Output
  * @retval The new state of PWR_FLAG (SET or RESET).
  */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_PWR_GET_FLAG(PWR_FLAG));

  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Clears the PWR's pending flags.
  * @param  PWR_FLAG: specifies the flag to clear.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  * @retval None
  */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
  /* Check the parameters */
  assert_param(IS_PWR_CLEAR_FLAG(PWR_FLAG));

  PWR->CR |=  PWR_FLAG << 2;
}


//USER FUNC
#define ADC1_C0 GPIO_PIN(PORT_A,0)
#define ADC1_C1 GPIO_PIN(PORT_A,1)
#define ADC1_C2 GPIO_PIN(PORT_A,2)
#define ADC1_C3 GPIO_PIN(PORT_A,3)
#define ADC1_C4 GPIO_PIN(PORT_A,4)
#define ADC1_C5 GPIO_PIN(PORT_A,5)
#define ADC1_C6 GPIO_PIN(PORT_A,6)
#define ADC1_C7 GPIO_PIN(PORT_A,7)
#define ADC1_C8 GPIO_PIN(PORT_B,0)
#define ADC1_C9 GPIO_PIN(PORT_B,1)
#define ADC1_C10 GPIO_PIN(PORT_C,0)
#define ADC1_C11 GPIO_PIN(PORT_C,1)
#define ADC1_C12 GPIO_PIN(PORT_C,2)
#define ADC1_C13 GPIO_PIN(PORT_C,3)
#define ADC1_C14 GPIO_PIN(PORT_C,4)
#define ADC1_C15 GPIO_PIN(PORT_C,5)

int adc1_dma1_tim4_sample_adc1_pin_init(uint16_t channel_num)
{
	switch(channel_num)
	{
		case 0: gpio_init(ADC1_C0, GPIO_IN);break;
		case 1: gpio_init(ADC1_C1, GPIO_IN);break;
		case 2: gpio_init(ADC1_C2, GPIO_IN);break;
		case 3: gpio_init(ADC1_C3, GPIO_IN);break;
		case 4: gpio_init(ADC1_C4, GPIO_IN);break;
		case 5: gpio_init(ADC1_C5, GPIO_IN);break;
		case 6: gpio_init(ADC1_C6, GPIO_IN);break;
		case 7: gpio_init(ADC1_C7, GPIO_IN);break;
		case 8: gpio_init(ADC1_C8, GPIO_IN);break;
		case 9: gpio_init(ADC1_C9, GPIO_IN);break;
		case 10: gpio_init(ADC1_C10, GPIO_IN);break;
		case 11: gpio_init(ADC1_C11, GPIO_IN);break;
		case 12: gpio_init(ADC1_C12, GPIO_IN);break;
		case 13: gpio_init(ADC1_C13, GPIO_IN);break;
		case 14: gpio_init(ADC1_C14, GPIO_IN);break;
		case 15: gpio_init(ADC1_C15, GPIO_IN);break;
		default: return -1;
	}
	return 0;
}


int adc1_dma1_tim4_sample_adc1_pins_init(uint16_t *channel_list,uint16_t channel_count)
{
	int i = 0;
	for (i=0;i<channel_count;i++)
	{
		if(adc1_dma1_tim4_sample_adc1_pin_init(channel_list[i]) < 0)
		{
			return -1;
		}
	}
	return 0;
}

void adc1_dma1_tim4_sample_adc1_init(uint16_t *channel_list,uint16_t channel_count)
{
	   STD_ADC_InitTypeDef ADC_InitStructure;
	   ADC_DeInit(ADC1);
	   periph_clk_en(APB2, RCC_APB2Periph_ADC1);
	   RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟（ADCCLK） 72MHZ/6 = 12MHZ

	   ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
	   ADC_InitStructure.ADC_ScanConvMode = ENABLE; //DISABLE单次模式(单通道) ENABLE多次模式
	   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //关闭连续模式
	   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4; //转换由TIM4触发
	   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	   ADC_InitStructure.ADC_NbrOfChannel = channel_count; //规则转换的ADC的通道数目,取值范围为1-16
	   ADC_Init(ADC1, &ADC_InitStructure); //初始化配置

	   int i = 0;
	   for(i=0;i<channel_count;i++)
	   {
		   ADC_RegularChannelConfig(ADC1, channel_list[i], i+1, ADC_SampleTime_239Cycles5);
	   }

	   ADC_DMACmd(ADC1, ENABLE);

	   ADC_Cmd(ADC1, ENABLE);

	   ADC_ResetCalibration(ADC1);
	   while (ADC_GetResetCalibrationStatus(ADC1) == SET)
	      ; //获取ADC重置校准寄存器的状态 等待复位结束
	   ADC_StartCalibration(ADC1); //开始指定ADC的校准状态
	   while (ADC_GetCalibrationStatus(ADC1) == SET)
	      ;
	   ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}


void adc1_dma1_tim4_sample_adc1_dma1_init(uint16_t channel_count,uint16_t sample_count,uint16_t *sample_buf)
{
   STD_DMA_InitTypeDef DMA_InitStructure;
   periph_clk_en(AHB, RCC_AHBPeriph_DMA1);
   //ADC1 DMA channel 1
   DMA_DeInit(DMA1_Channel1);
   DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t)&ADC1->DR;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) sample_buf;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = channel_count * sample_count;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //接收一次数据后，设备地址禁止后移
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //  内存自增
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //16位
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //16位
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //DMA normal
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;  //DMA优先级高
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);

   NVIC_EnableIRQ(DMA1_Channel1_IRQn);

   DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);  //使能或者失能指定的通道x中断(DMA1 通道1)
   /* Enable DMA1 channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);
}

void adc1_dma1_tim4_sample_tim4_init(uint16_t sample_count,uint16_t sample_duration_ms)
{
	uint16_t sample_period_us = sample_duration_ms*1000/sample_count;

	STD_TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	STD_TIM_OCInitTypeDef TIM_OCInitStructure;
	periph_clk_en(APB1, RCC_APB1Periph_TIM4);

	TIM_TimeBaseStructure.TIM_Period = sample_period_us; /* 自动重装载寄存器周期的值(计数值) */
	/* 累计 TIM_Period个频率后产生一个更新或者中断 */
	TIM_TimeBaseStructure.TIM_Prescaler = (72-1);  //系统主频72M，这里分频72，相当于1000K的定时器2时钟
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; /* 采样分频 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /* 向上计数模式 */
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(STD_TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;        //使能

	TIM_OCInitStructure.TIM_Pulse = 20;        //脉冲宽度,由这个设置占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //LOW电平有效

	TIM_OC4Init(STD_TIM4, &TIM_OCInitStructure);        //初始化

	TIM_Cmd(STD_TIM4, ENABLE);

	TIM_InternalClockConfig(STD_TIM4);
	TIM_OC2PreloadConfig(STD_TIM4, TIM_OCPreload_Enable);
	TIM_UpdateDisableConfig(STD_TIM4, DISABLE);
}

int adc1_dma1_tim4_sample_condition_check(uint16_t channel_count,uint16_t sample_count,uint16_t sample_duration_ms)
{
	uint16_t sample_period_us = sample_duration_ms*1000/sample_count;
	uint16_t sample_period_time_us_needed = (252*channel_count) / (72/6); //252 = 239.5+12.5
	if(sample_period_us < sample_period_time_us_needed)
	{
		return -1;
	}

	return 0;
}

int adc1_dma1_tim4_sample_init(uint16_t *channel_list,uint16_t channel_count,uint16_t sample_count,uint16_t sample_duration_ms,uint16_t *sample_buf)
{
	if(adc1_dma1_tim4_sample_condition_check(channel_count,sample_count,sample_duration_ms) < 0)
	{
//		printf("[STM32F10X STD PERIPH]: adc1 dma1 tim4 sample condition check failed\r\n");
		return -1;
	}

	if(adc1_dma1_tim4_sample_adc1_pins_init(channel_list,channel_count) < 0)
	{
//		printf("[STM32F10X STD PERIPH]: adc1 dma1 tim4 sample adc1 pins init failed\r\n");
		return -1;
	}
	adc1_dma1_tim4_sample_adc1_init(channel_list,channel_count);
	adc1_dma1_tim4_sample_adc1_dma1_init(channel_count,sample_count,sample_buf);
	adc1_dma1_tim4_sample_tim4_init(sample_count,sample_duration_ms);
	return 0;
}

adc1_dma1_tim4_sample_done_cb_t isr_cb = NULL;
void set_adc1_dma1_tim4_sample_done_cb(adc1_dma1_tim4_sample_done_cb_t cb)
{
	isr_cb = cb;
}

void isr_dma1_channel1(void) {
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) {
        TIM_Cmd(STD_TIM4, DISABLE);
        if(isr_cb != NULL)
        {
            isr_cb();
        }
//        printf("[STM32F10X STD PERIPH]: isr_dma1_channel1\r\n");
    }
    DMA_ClearITPendingBit(DMA1_IT_TC1);
}


void adc1_dma1_tim4_sample_enable(void)
{
	TIM_Cmd(STD_TIM4, ENABLE);
}

void adc1_dma1_tim4_sample_disable(void)
{
	TIM_Cmd(STD_TIM4, DISABLE);
}


int adc1_temp_sensor_init(void)
{
	STD_ADC_InitTypeDef ADC_InitStructure;
	ADC_DeInit(ADC1);
	periph_clk_en(APB2, RCC_APB2Periph_ADC1);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟（ADCCLK） 72MHZ/6 = 12MHZ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //DISABLE单次模式(单通道) ENABLE多次模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //关闭连续模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; //规则转换的ADC的通道数目,取值范围为1-16

	ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC1配置

	ADC_RegularChannelConfig(ADC1,ADC_Channel_16,1,ADC_SampleTime_239Cycles5);
	ADC_TempSensorVrefintCmd(ENABLE);

	ADC_Cmd(ADC1, ENABLE); // enable adc

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET)
		; //获取ADC重置校准寄存器的状态 等待复位结束
	ADC_StartCalibration(ADC1); //开始指定ADC的校准状态
	while (ADC_GetCalibrationStatus(ADC1) == SET)
		;

	return 0;
}

float adc1_temp_sensor_sample(void)
{
    uint16_t temp = 0;
    float temperate = 0.0;

    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
        ;
    temp = ADC_GetConversionValue(ADC1);

    temperate=(float)temp*(3.3/4096);
    temperate=(1.42-temperate)*1000/4.35+25;
    return temperate;
}



#define ADC2_C0 GPIO_PIN(PORT_A,0)
#define ADC2_C1 GPIO_PIN(PORT_A,1)
#define ADC2_C2 GPIO_PIN(PORT_A,2)
#define ADC2_C3 GPIO_PIN(PORT_A,3)
#define ADC2_C4 GPIO_PIN(PORT_A,4)
#define ADC2_C5 GPIO_PIN(PORT_A,5)
#define ADC2_C6 GPIO_PIN(PORT_A,6)
#define ADC2_C7 GPIO_PIN(PORT_A,7)
#define ADC2_C8 GPIO_PIN(PORT_B,0)
#define ADC2_C9 GPIO_PIN(PORT_B,1)
#define ADC2_C10 GPIO_PIN(PORT_C,0)
#define ADC2_C11 GPIO_PIN(PORT_C,1)
#define ADC2_C12 GPIO_PIN(PORT_C,2)
#define ADC2_C13 GPIO_PIN(PORT_C,3)
#define ADC2_C14 GPIO_PIN(PORT_C,4)
#define ADC2_C15 GPIO_PIN(PORT_C,5)

int adc2_sample_pin_init(uint16_t channel_num)
{
	switch(channel_num)
	{
		case 0: gpio_init(ADC2_C0, GPIO_IN);break;
		case 1: gpio_init(ADC2_C1, GPIO_IN);break;
		case 2: gpio_init(ADC2_C2, GPIO_IN);break;
		case 3: gpio_init(ADC2_C3, GPIO_IN);break;
		case 4: gpio_init(ADC2_C4, GPIO_IN);break;
		case 5: gpio_init(ADC2_C5, GPIO_IN);break;
		case 6: gpio_init(ADC2_C6, GPIO_IN);break;
		case 7: gpio_init(ADC2_C7, GPIO_IN);break;
		case 8: gpio_init(ADC2_C8, GPIO_IN);break;
		case 9: gpio_init(ADC2_C9, GPIO_IN);break;
		case 10: gpio_init(ADC2_C10, GPIO_IN);break;
		case 11: gpio_init(ADC2_C11, GPIO_IN);break;
		case 12: gpio_init(ADC2_C12, GPIO_IN);break;
		case 13: gpio_init(ADC2_C13, GPIO_IN);break;
		case 14: gpio_init(ADC2_C14, GPIO_IN);break;
		case 15: gpio_init(ADC2_C15, GPIO_IN);break;
		default: return -1;
	}
	return 0;
}


int adc2_sample_pins_init(uint16_t *channel_list,uint16_t channel_count)
{
	int i = 0;
	for (i=0;i<channel_count;i++)
	{
		if(adc2_sample_pin_init(channel_list[i]) < 0)
		{
			return -1;
		}
	}
	return 0;
}



int adc2_sample_init(uint16_t *channel_list,uint16_t channel_count)
{
	if(adc2_sample_pins_init(channel_list,channel_count) < 0)
	{
		return -1;
	}

    STD_ADC_InitTypeDef ADC_InitStructure;
    ADC_DeInit(ADC2);
    periph_clk_en(APB2, RCC_APB2Periph_ADC2);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟（ADCCLK） 72MHZ/6 = 12MHZ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //DISABLE单次模式(单通道) ENABLE多次模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //关闭连续模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; //规则转换的ADC的通道数目,取值范围为1-16

    ADC_Init(ADC2, &ADC_InitStructure); //初始化ADC2配置

    ADC_Cmd(ADC2, ENABLE); // enable adc

    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2) == SET)
        ; //获取ADC重置校准寄存器的状态 等待复位结束
    ADC_StartCalibration(ADC2); //开始指定ADC的校准状态
    while (ADC_GetCalibrationStatus(ADC2) == SET)
        ;

	return 0;
}

uint16_t adc2_sample_by_channel(uint16_t channel)
{
    uint16_t temp = 0;

    ADC_ClearFlag(ADC2, ADC_FLAG_EOC);
    ADC_RegularChannelConfig(ADC2, channel, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);

    while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET)
        ;
    temp = ADC_GetConversionValue(ADC2);
    return temp;
}


#define ADC3_C0 GPIO_PIN(PORT_A,0)
#define ADC3_C1 GPIO_PIN(PORT_A,1)
#define ADC3_C2 GPIO_PIN(PORT_A,2)
#define ADC3_C3 GPIO_PIN(PORT_A,3)
#define ADC3_C4 GPIO_PIN(PORT_F,6)
#define ADC3_C5 GPIO_PIN(PORT_F,7)
#define ADC3_C6 GPIO_PIN(PORT_F,8)
#define ADC3_C7 GPIO_PIN(PORT_F,9)
#define ADC3_C8 GPIO_PIN(PORT_F,10)

#define ADC3_C10 GPIO_PIN(PORT_C,0)
#define ADC3_C11 GPIO_PIN(PORT_C,1)
#define ADC3_C12 GPIO_PIN(PORT_C,2)
#define ADC3_C13 GPIO_PIN(PORT_C,3)

int adc3_sample_pin_init(uint16_t channel_num)
{
	switch(channel_num)
	{
		case 0: gpio_init(ADC3_C0, GPIO_IN);break;
		case 1: gpio_init(ADC3_C1, GPIO_IN);break;
		case 2: gpio_init(ADC3_C2, GPIO_IN);break;
		case 3: gpio_init(ADC3_C3, GPIO_IN);break;
		case 4: gpio_init(ADC3_C4, GPIO_IN);break;
		case 5: gpio_init(ADC3_C5, GPIO_IN);break;
		case 6: gpio_init(ADC3_C6, GPIO_IN);break;
		case 7: gpio_init(ADC3_C7, GPIO_IN);break;
		case 8: gpio_init(ADC3_C8, GPIO_IN);break;
		case 10: gpio_init(ADC3_C10, GPIO_IN);break;
		case 11: gpio_init(ADC3_C11, GPIO_IN);break;
		case 12: gpio_init(ADC3_C12, GPIO_IN);break;
		case 13: gpio_init(ADC3_C13, GPIO_IN);break;
		default: return -1;
	}
	return 0;
}


int adc3_sample_pins_init(uint16_t *channel_list,uint16_t channel_count)
{
	int i = 0;
	for (i=0;i<channel_count;i++)
	{
		if(adc3_sample_pin_init(channel_list[i]) < 0)
		{
			return -1;
		}
	}
	return 0;
}



int adc3_sample_init(uint16_t *channel_list,uint16_t channel_count)
{
	if(adc3_sample_pins_init(channel_list,channel_count) < 0)
	{
		return -1;
	}

    STD_ADC_InitTypeDef ADC_InitStructure;
    ADC_DeInit(ADC3);
    periph_clk_en(APB2, RCC_APB2Periph_ADC3);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟（ADCCLK） 72MHZ/6 = 12MHZ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //DISABLE单次模式(单通道) ENABLE多次模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //关闭连续模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; //规则转换的ADC的通道数目,取值范围为1-16

    ADC_Init(ADC3, &ADC_InitStructure); //初始化ADC2配置

    ADC_Cmd(ADC3, ENABLE);

    ADC_ResetCalibration(ADC3);
    while (ADC_GetResetCalibrationStatus(ADC3) == SET)
        ; //获取ADC重置校准寄存器的状态 等待复位结束
    ADC_StartCalibration(ADC3); //开始指定ADC的校准状态
    while (ADC_GetCalibrationStatus(ADC3) == SET)
        ;

	return 0;
}

uint16_t adc3_sample_by_channel(uint16_t channel)
{
    uint16_t temp = 0;

    ADC_ClearFlag(ADC3, ADC_FLAG_EOC);
    ADC_RegularChannelConfig(ADC3, channel, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC3, ENABLE);

    while (ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET)
        ;
    temp = ADC_GetConversionValue(ADC3);
    return temp;
}


#define  DAC_PIN     GPIO_PIN(PORT_A,4)

int dac1_pa4_dma2_tim6_wave_outopt_init(uint32_t wave_fre)
{
	gpio_init(DAC_PIN, GPIO_OUT);

    STD_TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    periph_clk_en(APB1, RCC_APB1Periph_TIM6);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_Period = wave_fre;
    TIM_TimeBaseInit(STD_TIM6, &TIM_TimeBaseStructure);
    TIM_SelectOutputTrigger(STD_TIM6, TIM_TRGOSource_Update);

    STD_DAC_InitTypeDef            DAC_InitStructure;
    periph_clk_en(APB1, RCC_APB1Periph_DAC);

    DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
    return 0;
}
void dac1_pa4_dma2_tim6_wave_outopt(uint32_t wave_start_addr,uint16_t wave_len)
{
    STD_DMA_InitTypeDef            DMA_InitStructure;
    periph_clk_en(AHB, RCC_AHBPeriph_DMA2);

    DMA_StructInit( &DMA_InitStructure);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//peripherals  to memory
    DMA_InitStructure.DMA_BufferSize = wave_len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR8R1);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)wave_start_addr;

    DMA_Cmd(DMA2_Channel3, DISABLE);
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE);

    TIM_Cmd(STD_TIM6, ENABLE);
}


void iwdg_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	// 设置预分频器值
	IWDG_SetPrescaler( IWDG_Prescaler_256 );

	// 设置重装载寄存器值
	IWDG_SetReload( 0xFFF );

	// 把重装载寄存器的值放到计数器中
	IWDG_ReloadCounter();

	// 使能 IWDG
	IWDG_Enable();
}

void iwdg_feed(void)
{
	 IWDG_ReloadCounter();
}


void disable_jtag_pins(void)
{
   periph_clk_en(APB2, RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO);
   GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}














