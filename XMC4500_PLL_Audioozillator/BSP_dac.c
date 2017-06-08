/*
 * BSP_dac.c
 *
 *  Created on: 08. Juni 2017
 *      Author: thomas
 */

#include <xmc_gpio.h>
#include <xmc_dac.h>
#include "BSP_dac.h"

const uint8_t pattern[] = XMC_DAC_PATTERN_SINE;

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



void _initDAC0(void)
{

  /* API to initial DAC Module*/
  XMC_DAC_CH_Init(XMC_DAC0, 0U, &ch_config0); // pattern config
  XMC_DAC_CH_Init(XMC_DAC0, 1U, &ch_config1); // single value config

  /* API to initial DAC in SingleValue mode */
  //XMC_DAC_CH_StartSingleValueMode(XMC_DAC0, 1U);
}

void _dac_start_pattern_mode(const uint8_t channel, const uint32_t defaultFreq)
{
  /* API to initial DAC in Pattern mode. When using a predefined pattern a type cast avoid warnings */
    XMC_DAC_CH_StartPatternMode(XMC_DAC0, channel, pattern,
        XMC_DAC_CH_PATTERN_SIGN_OUTPUT_DISABLED, XMC_DAC_CH_TRIGGER_INTERNAL, defaultFreq);
}

void _dac_set_value(const uint8_t channel, const uint16_t dacVal)
{
  /* API to write a value into DAC Output */
  XMC_DAC_CH_Write(XMC_DAC0, channel, dacVal);

  return;
}

uint8_t _dac_change_pattern_freq(const uint8_t channel, const uint32_t freq)
{
  /* update frequency */
  XMC_DAC_CH_STATUS_t ret = XMC_DAC_CH_SetPatternFrequency(XMC_DAC0, channel, freq);

  if (ret == XMC_DAC_CH_STATUS_OK)
    return 0;

  return ret;
}
