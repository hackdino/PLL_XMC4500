/*
 * BSP_ccu4.c
 *
 *  Created on: 07. Juni 2017
 *      Author: thomas
 */


#include <xmc_ccu4.h>
#include <xmc_scu.h>
#include "BSP_ccu4.h"


XMC_CCU4_SLICE_COMPARE_CONFIG_t g_timer_object =
{
  .timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
  .monoshot            = false,
  .shadow_xfer_clear   = 0U,
  .dither_timer_period = 0U,
  .dither_duty_cycle   = 0U,
  .prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
  .mcm_enable          = 0U,
  .prescaler_initval   = 0U,
  .float_limit         = 0U,
  .dither_limit        = 0U,
  .passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
  .timer_concatenation = 0U
};


void _initCCU40_0()
{
  /* Local variable which holds configuration of Event-1 */
  XMC_CCU4_SLICE_EVENT_CONFIG_t config;

  config.duration = XMC_CCU4_SLICE_EVENT_FILTER_5_CYCLES;
  config.edge     = XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE;
  config.level    = XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH; /* Not needed */
  config.mapped_input = XMC_CCU4_SLICE_INPUT_I;

  /* Ensure fCCU reaches CCU42 */
  XMC_CCU4_SetModuleClock(MODULE_PTR, XMC_CCU4_CLOCK_SCU);

  XMC_CCU4_Init(MODULE_PTR, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

  /* Get the slice out of idle mode */
  XMC_CCU4_EnableClock(MODULE_PTR, SLICE_NUMBER);

  /* Start the prescaler and restore clocks to slices */
  XMC_CCU4_StartPrescaler(MODULE_PTR);

  /* Initialize the Slice */
  XMC_CCU4_SLICE_CompareInit(SLICE_PTR, &g_timer_object);

  /* Enable compare match and period match events */
  XMC_CCU4_SLICE_EnableEvent(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);

  /* Connect period match event to SR0 */
  XMC_CCU4_SLICE_SetInterruptNode(SLICE_PTR, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);

  /* Configure NVIC */

  /* Set priority */
  NVIC_SetPriority(CCU40_0_IRQn, 10U);

  /* Enable IRQ */
  NVIC_EnableIRQ(CCU40_0_IRQn);

  /* Program a very large value into PR and CR */
  XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR, 1200U);

  /* Enable shadow transfer */
  XMC_CCU4_EnableShadowTransfer(MODULE_PTR, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);

  /* Configure Event-1 and map it to Input-I */
  XMC_CCU4_SLICE_ConfigureEvent(SLICE_PTR, XMC_CCU4_SLICE_EVENT_1, &config);

  /* Map Event-1 to Start function */
  XMC_CCU4_SLICE_StartConfig(SLICE_PTR, XMC_CCU4_SLICE_EVENT_1, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);

  /* Generate an external start trigger */
  XMC_SCU_SetCcuTriggerHigh(CAPCOM_MASK);

}

void _ccu4_start_timer()
{

}

void _ccu4_stop_timer()
{

}
