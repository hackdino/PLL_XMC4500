/*
 * Copyright (C) 2014 Infineon Technologies AG. All rights reserved.
 *
 * Infineon Technologies AG (Infineon) is supplying this software for use with
 * Infineon's microcontrollers.
 * This file can be freely distributed within development tools that are
 * supporting such microcontrollers.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

/**
 * @file
 * @date 19 Nov, 2015
 * @version 1.0.2
 *
 * @brief XMC4500 Relax kit ERU external interrupt
 *
 * The user can switch on/off the LED2 (P1.0) by pressing BUTTON2 (P1.15)
 *
 * History <br>
 *
 * Version 1.0.0 Initial
 * Version 1.0.2 Use recommended .input_a instead of .input in button_event_generator_config
 *
 */

#include <xmc_gpio.h>
#include <xmc_dac.h>
#include <xmc_scu.h>
#include <xmc_eru.h>

/******************************************************************** DEFINES */
#define LED2 P1_0
#define BUTTON2 P1_15
#define TEST 1
/******************************************************************** GLOBALS */
const uint8_t pattern[] = XMC_DAC_PATTERN_SINE;

/******************************************************************** STRUCTURES */
// DAC config
XMC_DAC_CH_CONFIG_t const ch_config0=
{
  .output_offset  = 0U,
  .data_type    = XMC_DAC_CH_DATA_TYPE_SIGNED,
  .output_scale   = XMC_DAC_CH_OUTPUT_SCALE_MUL_64,
  .output_negation = XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
};

XMC_DAC_CH_CONFIG_t const ch_config1=
{
  .output_offset  = 0U,
  .data_type    = XMC_DAC_CH_DATA_TYPE_SIGNED,
  .output_scale   = XMC_DAC_CH_OUTPUT_SCALE_NONE,
  .output_negation = XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
};

// ERU config
XMC_ERU_ETL_CONFIG_t button_event_generator_config =
{
  .input_a = ERU1_ETL1_INPUTA_P1_15,
  .source = XMC_ERU_ETL_SOURCE_A,
  .edge_detection = XMC_ERU_ETL_EDGE_DETECTION_RISING,
  .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
  .enable_output_trigger = true,
  .output_trigger_channel = XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL0
};

XMC_ERU_OGU_CONFIG_t button_event_detection_config =
{
  .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER
};

/******************************************************************** PROTOTYPES */
void _initDAC0(void);
void _dac_set_value(const uint16_t dacVal);
uint8_t _dac_set_pattern(const uint32_t freq);

/******************************************************************** INTERRUPTS */
void ERU1_0_IRQHandler(void)
{
  XMC_GPIO_ToggleOutput(LED2);
}

/******************************************************************** MAIN */
int main(void)
{

  XMC_ERU_ETL_Init(ERU1_ETL1, &button_event_generator_config);
  XMC_ERU_OGU_Init(ERU1_OGU0, &button_event_detection_config);

  XMC_GPIO_SetMode(LED2, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
  XMC_GPIO_SetMode(BUTTON2, XMC_GPIO_MODE_INPUT_TRISTATE);

  NVIC_SetPriority(ERU1_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
  NVIC_EnableIRQ(ERU1_0_IRQn);

  _initDAC0();

  while(1)
  {
	/* Infinite loop */
  }
}


/******************************************************************** FUNCTIONS */
void _initDAC0(void)
{

  /* API to initial DAC Module*/
  XMC_DAC_CH_Init(XMC_DAC0, 0U, &ch_config0);
  XMC_DAC_CH_Init(XMC_DAC0, 1U, &ch_config1);

  /* API to initial DAC in Pattern mode. When using a predefined pattern a type cast avoid warnings */
  XMC_DAC_CH_StartPatternMode(XMC_DAC0, 0U, pattern, XMC_DAC_CH_PATTERN_SIGN_OUTPUT_DISABLED, XMC_DAC_CH_TRIGGER_INTERNAL, 500U);

  /* API to initial DAC in SingleValue mode */
  XMC_DAC_CH_StartSingleValueMode(XMC_DAC0, 1U);

}

void _dac_set_value(const uint16_t dacVal)
{
  /* API to write a value into DAC Output */
  XMC_DAC_CH_Write(XMC_DAC0, 1, dacVal);

  return;
}

uint8_t _dac_set_pattern(const uint32_t freq)
{
  /* update frequency */
  XMC_DAC_CH_STATUS_t ret = XMC_DAC_CH_SetPatternFrequency(XMC_DAC0, 0, freq);

  if (ret == XMC_DAC_CH_STATUS_OK)
    return 0;

  return ret;
}
