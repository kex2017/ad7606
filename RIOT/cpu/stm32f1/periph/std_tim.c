
#include "cpu.h"
#include "board.h"
#include "periph/std_tim.h"
#include "periph_cpu.h"
#include "periph_conf.h"

void TIM_OC2Init(Local_TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct) {

	uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

	/* Check the parameters */
	assert_param(IS_TIM_LIST6_PERIPH(TIMx));
	assert_param(IS_TIM_OC_MODE(TIM_OCInitStruct->TIM_OCMode));
	assert_param(IS_TIM_OUTPUT_STATE(TIM_OCInitStruct->TIM_OutputState));
	assert_param(IS_TIM_OC_POLARITY(TIM_OCInitStruct->TIM_OCPolarity));
	/* Disable the Channel 2: Reset the CC2E Bit */
	TIMx->CCER &= (uint16_t) (~((uint16_t) TIM_CCER_CC2E));

	/* Get the TIMx CCER register value */
	tmpccer = TIMx->CCER;
	/* Get the TIMx CR2 register value */
	tmpcr2 = TIMx->CR2;

	/* Get the TIMx CCMR1 register value */
	tmpccmrx = TIMx->CCMR1;

	/* Reset the Output Compare mode and Capture/Compare selection Bits */
	tmpccmrx &= (uint16_t) (~((uint16_t) TIM_CCMR1_OC2M));
	tmpccmrx &= (uint16_t) (~((uint16_t) TIM_CCMR1_CC2S));

	/* Select the Output Compare Mode */
	tmpccmrx |= (uint16_t) (TIM_OCInitStruct->TIM_OCMode << 8);

	/* Reset the Output Polarity level */
	tmpccer &= (uint16_t) (~((uint16_t) TIM_CCER_CC2P));
	/* Set the Output Compare Polarity */
	tmpccer |= (uint16_t) (TIM_OCInitStruct->TIM_OCPolarity << 4);

	/* Set the Output State */
	tmpccer |= (uint16_t) (TIM_OCInitStruct->TIM_OutputState << 4);

	if ((TIMx == LOCAL_TIM1) || (TIMx == LOCAL_TIM8)) {
		assert_param(IS_TIM_OUTPUTN_STATE(TIM_OCInitStruct->TIM_OutputNState));
		assert_param(IS_TIM_OCN_POLARITY(TIM_OCInitStruct->TIM_OCNPolarity));
		assert_param(IS_TIM_OCNIDLE_STATE(TIM_OCInitStruct->TIM_OCNIdleState));
		assert_param(IS_TIM_OCIDLE_STATE(TIM_OCInitStruct->TIM_OCIdleState));

		/* Reset the Output N Polarity level */
		tmpccer &= (uint16_t) (~((uint16_t) TIM_CCER_CC2NP));
		/* Set the Output N Polarity */
		tmpccer |= (uint16_t) (TIM_OCInitStruct->TIM_OCNPolarity << 4);

		/* Reset the Output N State */
		tmpccer &= (uint16_t) (~((uint16_t) TIM_CCER_CC2NE));
		/* Set the Output N State */
		tmpccer |= (uint16_t) (TIM_OCInitStruct->TIM_OutputNState << 4);

		/* Reset the Output Compare and Output Compare N IDLE State */
		tmpcr2 &= (uint16_t) (~((uint16_t) TIM_CR2_OIS2));
		tmpcr2 &= (uint16_t) (~((uint16_t) TIM_CR2_OIS2N));

		/* Set the Output Idle state */
		tmpcr2 |= (uint16_t) (TIM_OCInitStruct->TIM_OCIdleState << 2);
		/* Set the Output N Idle state */
		tmpcr2 |= (uint16_t) (TIM_OCInitStruct->TIM_OCNIdleState << 2);
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
void TIM_Cmd(Local_TIM_TypeDef* TIMx, FunctionalState NewState) {
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));

	if (NewState != DISABLE) {
		/* Enable the TIM Counter */
		TIMx->CR1 |= TIM_CR1_CEN;
	} else {
		/* Disable the TIM Counter */
		TIMx->CR1 &= (uint16_t) (~((uint16_t) TIM_CR1_CEN));
	}

}
void TIM_InternalClockConfig(Local_TIM_TypeDef* TIMx) {

	/* Check the parameters */
	assert_param(IS_TIM_LIST6_PERIPH(TIMx));
	/* Disable slave mode to clock the prescaler directly with the internal clock */
	TIMx->SMCR &= (uint16_t) (~((uint16_t) TIM_SMCR_SMS));

}
void TIM_OC2PreloadConfig(Local_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload) {

	uint16_t tmpccmr1 = 0;
	/* Check the parameters */
	assert_param(IS_TIM_LIST6_PERIPH(TIMx));
	assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCPreload));
	tmpccmr1 = TIMx->CCMR1;
	/* Reset the OC2PE Bit */
	tmpccmr1 &= (uint16_t) ~((uint16_t) TIM_CCMR1_OC2PE);
	/* Enable or Disable the Output Compare Preload feature */
	tmpccmr1 |= (uint16_t) (TIM_OCPreload << 8);
	/* Write to TIMx CCMR1 register */
	TIMx->CCMR1 = tmpccmr1;

}

void TIM_UpdateDisableConfig(Local_TIM_TypeDef* TIMx, FunctionalState NewState) {
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	if (NewState != DISABLE) {
		/* Set the Update Disable Bit */
		TIMx->CR1 |= TIM_CR1_UDIS;
	} else {
		/* Reset the Update Disable Bit */
		TIMx->CR1 &= (uint16_t) ~((uint16_t) TIM_CR1_UDIS);
	}
}

void TIM_TimeBaseInit(Local_TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{

	  uint16_t tmpcr1 = 0;

	  /* Check the parameters */
	  assert_param(IS_TIM_ALL_PERIPH(TIMx));
	  assert_param(IS_TIM_COUNTER_MODE(TIM_TimeBaseInitStruct->TIM_CounterMode));
	  assert_param(IS_TIM_CKD_DIV(TIM_TimeBaseInitStruct->TIM_ClockDivision));

	  tmpcr1 = TIMx->CR1;

	  if((TIMx == LOCAL_TIM1) || (TIMx == LOCAL_TIM8)|| (TIMx == LOCAL_TIM2) || (TIMx == LOCAL_TIM3)||
	     (TIMx == LOCAL_TIM4) || (TIMx == LOCAL_TIM5))
	  {
	    /* Select the Counter Mode */
	    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));
	    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode;
	  }

	  if((TIMx != LOCAL_TIM6) && (TIMx != LOCAL_TIM7))
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

	  if ((TIMx == LOCAL_TIM1) || (TIMx == LOCAL_TIM8)|| (TIMx == LOCAL_TIM15)|| (TIMx == LOCAL_TIM16) || (TIMx == LOCAL_TIM17))
	  {
	    /* Set the Repetition Counter value */
	    TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
	  }

	  /* Generate an update event to reload the Prescaler and the Repetition counter
	     values immediately */
	  TIMx->EGR = TIM_PSCReloadMode_Immediate;

}

void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  /* Set the default configuration */
  TIM_TimeBaseInitStruct->TIM_Period = 0xFFFF;
  TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000;
  TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000;
}

void TIM_OC1Init(Local_TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
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

  if((TIMx == LOCAL_TIM1) || (TIMx == LOCAL_TIM8)|| (TIMx == LOCAL_TIM15)||
     (TIMx == LOCAL_TIM16)|| (TIMx == LOCAL_TIM17))
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

void TIM_CtrlPWMOutputs(Local_TIM_TypeDef* TIMx, FunctionalState NewState)
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

void TIM_OC4Init(Local_TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct)
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

  if((TIMx == LOCAL_TIM1) || (TIMx == LOCAL_TIM8))
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

void TIM_OC4PreloadConfig(Local_TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
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

