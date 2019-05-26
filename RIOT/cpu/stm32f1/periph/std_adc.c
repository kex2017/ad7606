#include "cpu.h"
#include "board.h"
#include "periph/std_adc.h"
#include "periph_cpu.h"
#include "periph_conf.h"

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

void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct)
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

void ADC_DeInit(ADC_TypeDef* ADCx)
{

	  /* Check the parameters */
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
void ADC_ResetCalibration(ADC_TypeDef* ADCx)
{
	  /* Check the parameters */
	  assert_param(IS_ADC_ALL_PERIPH(ADCx));
	  /* Resets the selected ADC calibration registers */
	  ADCx->CR2 |= CR2_RSTCAL_Set;
}

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

void ADC_StartCalibration(ADC_TypeDef* ADCx)
{
	  /* Check the parameters */
	  assert_param(IS_ADC_ALL_PERIPH(ADCx));
	  /* Enable the selected ADC calibration process */
	  ADCx->CR2 |= CR2_CAL_Set;
}


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


