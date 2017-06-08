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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <xmc_gpio.h>
#include <xmc_dac.h>
#include <xmc_scu.h>
#include <xmc_eru.h>
#include <xmc_ccu4.h>

#include <BSP_uart.h>
#include "BSP_ccu4.h"
#include "BSP_dac.h"

/******************************************************************** DEFINES */
#define LED2 P1_0
// #define BUTTON2 P1_15


#define PLL_INPUT_PIN   P2_1
#define PLL_OUTPUT_PIN  P14_8

// UART
#define RX_BUFFER_SIZE 64
/******************************************************************** GLOBALS */
typedef enum { MULTIPLE = 1, FREQUENCY = 2 } t_pll_action;
typedef enum { QUARTER = 1, HALF = 2, EQUAL = 3, DOUBLE = 4 } t_pll_factor;

/* Timer Interrupt counters */
volatile uint32_t g_num_period_interrupts;
volatile bool period_match;

/* PLL program variables */
volatile bool IsPllRequest;
volatile bool IsPllRunning;
volatile bool IsTimerRunning;

t_pll_action CurrentPllAction;
t_pll_factor CurrentPllFactor;
uint32_t CurrentPllFrequency;

double g_timer_period_duration = 0.00001; // 10 us ??
double g_currentInputSignalDuration;
double g_currentInputSignalFrequency;
/******************************************************************** STRUCTURES */

// ERU config
XMC_ERU_ETL_CONFIG_t button_event_generator_config =
{
  .input_a = ERU1_ETL0_INPUTB_P2_1,
  .source = XMC_ERU_ETL_SOURCE_B,
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
void initPLL();
void error_message_led(void);

/******************************************************************** INTERRUPTS */
void ERU1_0_IRQHandler(void)
{
  XMC_GPIO_ToggleOutput(LED2);
  XMC_GPIO_ToggleOutput(PLL_OUTPUT_PIN);

  // TODO:
  /*
   * - receive PLL request from UART
   * - on rising edge start a ccu4 timer
   * - on next rising edge stop the timer and save the duration
   * -- start the DAC with the requested frequency
   */

  if (IsPllRequest)
  {
    if (!IsTimerRunning)
    {
      g_num_period_interrupts = 0;
      _ccu4_start_timer();
    }
    else
    {
      _ccu4_stop_timer();

      initPLL();

      IsPllRequest = false;
    }
  }
  else if (IsPllRunning)
  {
    // TODO: PLL Regler Code... adjust current pll output signal

    // Idee: output pll signal mit ADC messen (mind. 3 werte),
    // abweichung prüfen (mit 3 werten kann bestimmt werden ob steigende oder fallende,
    // somit auch ob zu früh oder zu spät)
    // dann einen 2. timer starten der entweder etwas länger oder kürzer ist also eingangssignal
    // im timer interrupt pll dac neu starten um die zeitverzögerung auszugleichen
    // diese schritte immer wieder wiederholen = REGLER
  }
}

/* Interrupt handlers */
void CCU40_0_IRQHandler(void)
{
  g_num_period_interrupts++;
  XMC_CCU4_SLICE_ClearEvent(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
  period_match = true;
}

/******************************************************************** MAIN */
int main(void)
{
  XMC_ERU_ETL_Init(ERU1_ETL0, &button_event_generator_config);
  XMC_ERU_OGU_Init(ERU1_OGU0, &button_event_detection_config);

  XMC_GPIO_SetMode(PLL_OUTPUT_PIN, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
  XMC_GPIO_SetMode(LED2, XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
  XMC_GPIO_SetMode(PLL_INPUT_PIN, XMC_GPIO_MODE_INPUT_TRISTATE);

  NVIC_SetPriority(ERU1_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
  NVIC_EnableIRQ(ERU1_0_IRQn);

  _initDAC0();
  _initUART1_CH1();
  _initCCU40_0();


  /* Available UART functions */
//  _uart_send_char('X');
//  _uart_send_string("Testing...\n");
//  _uart_printf("Magic Number = %d\n",42);
//
//  _uart_get_string (rx_buff);
//  _uart_printf("RX string = %s\n", rx_buff);

  uint8_t run = 1;
  char rx_buff[RX_BUFFER_SIZE] = {0};

  while(run)
  {
    /* Infinite loop */

    int ret = _uart_get_string (rx_buff); // blocking

    if (ret != 0) // error
    {
      continue;
    }

    if (rx_buff[0] == '#' && rx_buff[2] == ';')
    {
      if (rx_buff[1] == '1') // DAC command
      {
        uint16_t dacVal = strtol(&rx_buff[3], (char **)NULL, 10);

        // TODO: check error

        _dac_set_value(1U, dacVal);

        sprintf(rx_buff, "DAC: Set Value %d", dacVal);

        if (_uart_send_string(rx_buff) < 0) // error
          run = 0;
      }
      else if (rx_buff[1] == '2') // ADC command
      {
//        uint16_t numOfAdcSamples = strtol(&text[3], (char **)NULL, 10);
//
//        // TODO: check error
//
//        // TODO: return the number of ADC samples?
//        uint16_t adcSamples[numOfAdcSamples];
//        uint32_t i;
//        for (i = 0; i < numOfAdcSamples; i++)
//        {
//          ADC_Value_Available = 0;
//          _vadc_trigger_conversion();
//
//          while (!ADC_Value_Available)
//          {
//            // TODO: save the samples in array?
//            adcSamples[i] = ADC_Current_Value;
//          }
//        }
//
//        // TODO: transfer the adcSamples array over usb?
//        //sprintf(text, "ADC: Set Value %d", dacVal);
//        char adcText[50];
//        for (i = 0; i < numOfAdcSamples; i++)
//        {
//          sprintf(adcText, "ADC: Value %d", adcSamples[i]);
//          if (usb_transfer(adcText) < 0) // error
//          {
//            run = 0;
//            break;
//          }
//        }
      }
      else if (rx_buff[1] == '3') // PLL command
      {
        uint8_t pllAction = strtol(&rx_buff[3], (char **)NULL, 10);
        CurrentPllAction = pllAction;

        if (CurrentPllAction == MULTIPLE)
        {
          uint8_t pllFactor = strtol(&rx_buff[3], (char **)NULL, 10);
          CurrentPllFactor = pllFactor;
        }
        else if (CurrentPllAction == FREQUENCY)
        {
          uint32_t pllFrequency = strtol(&rx_buff[3], (char **)NULL, 10);

          // check min, max values?
          CurrentPllFrequency = pllFrequency;
        }

        IsPllRequest = true;
      }
    }

    memset(rx_buff, '\0', strlen(rx_buff));
  }

  //Switches on the LED P1.0 in case of an error.
  error_message_led();
}


/******************************************************************** FUNCTIONS */
void initPLL()
{
  // calculate duration [s]
  g_currentInputSignalDuration = g_num_period_interrupts * g_timer_period_duration;

  // Frequenz [Hz]
  g_currentInputSignalFrequency = 1 / g_currentInputSignalDuration;

  if (CurrentPllAction == MULTIPLE)
  {
    if (CurrentPllFactor == HALF)
    {
      // NOTE: resulting frequ = frequency * XMC_DAC_SAMPLES_PER_PERIOD
      // XMC_DAC_SAMPLES_PER_PERIOD = 32U
      uint32_t freq = g_currentInputSignalFrequency / 2;
      _dac_start_pattern_mode(0U, freq);
    }
    else if (CurrentPllFactor == DOUBLE)
    {
      uint32_t freq = g_currentInputSignalFrequency * 2;
      _dac_start_pattern_mode(0U, freq);
    }
  }
  else if (CurrentPllAction == FREQUENCY)
  {
    _dac_start_pattern_mode(0U, CurrentPllFrequency);
  }

  IsPllRunning = true;
}


void error_message_led(void)
{
    PORT1->IOCR0 = 0x80;    //Set the port P1.0 to output
    PORT1->OUT = 1;     //Set the output signal to port P1.0
}
