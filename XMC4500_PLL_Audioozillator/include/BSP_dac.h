/*
 * BSP_dac.h
 *
 *  Created on: 08. Juni 2017
 *      Author: thomas
 */

#ifndef INCLUDE_BSP_DAC_H_
#define INCLUDE_BSP_DAC_H_



void _initDAC0(void);
void _dac_start_pattern_mode(const uint8_t channel, const uint32_t defaultFreq);
void _dac_set_value(const uint8_t channel, const uint16_t dacVal);
uint8_t _dac_change_pattern_freq(const uint8_t channel, const uint32_t freq);



#endif /* INCLUDE_BSP_DAC_H_ */
