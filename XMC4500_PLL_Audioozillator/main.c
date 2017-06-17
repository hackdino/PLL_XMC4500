/**
 *
 */

/*********************************************************************************************************************
 * HEADER FILES
 ********************************************************************************************************************/
#include <xmc_vadc.h>
#include <xmc_scu.h>
#include <xmc_dac.h>
#include <xmc_gpio.h>
#include <xmc_ccu4.h>
#include <math.h>
#include <stdlib.h>
/*********************************************************************************************************************
 * MACROS
 ********************************************************************************************************************/
/* Pin P14.1 is measured and converted */
#define CHANNEL_NUMBER    (1U)
#define VADC_GROUP_PTR    (VADC_G0)

/* Register result */
#define RES_REG_NUMBER    (0)

/* ADC Conversion rate (ms) */
#define TICK_PERIOD (10U)

#define IN_ZERO P2_10
#define TRIGGER_DAC P3_0
#define OUT_ZERO P3_2
#define PHASE_DIFF P0_0
#define DIR_CORR P1_14

/*********************************************************************************************************************
 * GLOBAL DATA
 ********************************************************************************************************************/

#define R 		         (20000U)
#define ARRAY_SIZE 	     (48U)
#define TICKS_PER_SECOND (10U)


const uint16_t melody[ARRAY_SIZE] =
{
		660U,	R,	660U,	R, 	660U,	R, 	510U,	R,
		660U, R, 	770U,	R,	380U,	R, 	510U,	R,
		380U,	R, 	320U,	R, 	440U,	R, 	480U,   R,
		450U,	R,	430U,	R,	380U,	R,	660U,	R,
		760U,	R,	860U,	R,	700U,   R, 	760U,	R,
		660U, R,	520U,	R,	580U,	R,	480U,	R
};


const uint16_t beats[ARRAY_SIZE] =
{
		10U, 15U, 10U, 30U, 10U, 30U,	10U, 10U,
		10U, 30U, 10U, 55U, 10U, 57U, 10U, 45U,
		10U, 40U,	10U, 50U, 10U, 30U,	 8U, 33U,
		10U, 15U,	10U, 30U, 10U, 20U,	 8U, 20U,
		5U,  15U,	10U, 30U,  8U, 15U,	 5U, 35U,
		8U,  30U,	 8U, 15U,  8U, 15U,	 8U, 50U
};


XMC_DAC_CH_CONFIG_t const ch_config0=
{
		.output_offset	= 0U,
		.data_type 		= XMC_DAC_CH_DATA_TYPE_SIGNED,
		.output_scale 	= XMC_DAC_CH_OUTPUT_SCALE_MUL_64,
		.output_negation = XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
};

XMC_DAC_CH_CONFIG_t const ch_config1=
{
		.output_offset	= 0U,
		.data_type 		= XMC_DAC_CH_DATA_TYPE_SIGNED,
		.output_scale 	= XMC_DAC_CH_OUTPUT_SCALE_NONE,
		.output_negation = XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
};

const uint8_t pattern[] = XMC_DAC_PATTERN_SINE;


/* Initialization data of VADC Global resources */
XMC_VADC_GLOBAL_CONFIG_t g_global_config =
{
		.boundary0 = 1500,
		.boundary1 = 1500,
		.clock_config =
		{
				.analog_clock_divider  = 15,
		},
};

/* Initialization data of a VADC group */
XMC_VADC_GROUP_CONFIG_t g_group_config =
{
		.boundary0 = 1500,
		.boundary1 = 1500,
		.class1 =
		{
				.conversion_mode_standard 	 =  XMC_VADC_CONVMODE_FASTCOMPARE,
				.conversion_mode_emux = XMC_VADC_CONVMODE_FASTCOMPARE,
		}
};

/* Channel configuration data */
XMC_VADC_CHANNEL_CONFIG_t  g_channel_config =
{
		.boundary_flag_output_ch0 = XMC_VADC_GROUP_BOUNDARY_FLAG_MODE_ENABLED,
		.boundary_flag_output_ch1 = XMC_VADC_GROUP_BOUNDARY_FLAG_MODE_ENABLED,
		.lower_boundary_select = XMC_VADC_CHANNEL_BOUNDARY_GROUP_BOUND1,
		.upper_boundary_select = XMC_VADC_CHANNEL_BOUNDARY_GROUP_BOUND0,
		.input_class       = XMC_VADC_CHANNEL_CONV_GROUP_CLASS1,
		.alias_channel = XMC_VADC_CHANNEL_ALIAS_DISABLED,
};

const XMC_VADC_GROUP_CONFIG_t g_group_handle = { };
const XMC_VADC_BACKGROUND_CONFIG_t g_bgn_handle = { };

/* Timer Configuration */

#define SLICE_PTR_1       	CCU40_CC40
#define MODULE_PTR_1        CCU40
#define MODULE_NUMBER_1     (0U)
#define SLICE_NUMBER_1      (0U)

#define SLICE_PTR_2       	CCU41_CC40
#define MODULE_PTR_2        CCU41
#define MODULE_NUMBER_2     (0U)
#define SLICE_NUMBER_2      (0U)

#define SLICE_PTR_3       	CCU42_CC40
#define MODULE_PTR_3        CCU42
#define MODULE_NUMBER_3     (0U)
#define SLICE_NUMBER_3      (0U)


XMC_CCU4_SLICE_COMPARE_CONFIG_t g_timer_object_1 =
{
		.timer_mode 		     = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot   		     = false,
		.shadow_xfer_clear   = 0U,
		.dither_timer_period = 0U,
		.dither_duty_cycle   = 0U,
		.prescaler_mode	     = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable		       = 0U,
		.prescaler_initval   = 3U,
		.float_limit		     = 0U,
		.dither_limit		     = 0U,
		.passive_level 	     = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = 0U
};

XMC_CCU4_SLICE_COMPARE_CONFIG_t g_timer_object_2 =
{
		.timer_mode 		     = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot   		     = false,
		.shadow_xfer_clear   = 0U,
		.dither_timer_period = 0U,
		.dither_duty_cycle   = 0U,
		.prescaler_mode	     = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable		       = 0U,
		.prescaler_initval   = 3U,
		.float_limit		     = 0U,
		.dither_limit		     = 0U,
		.passive_level 	     = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = 0U
};

XMC_CCU4_SLICE_COMPARE_CONFIG_t g_timer_object_3 =
{
		.timer_mode 		     = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot   		     = false,
		.shadow_xfer_clear   = 0U,
		.dither_timer_period = 0U,
		.dither_duty_cycle   = 0U,
		.prescaler_mode	     = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable		       = 0U,
		.prescaler_initval   = 3U,
		.float_limit		     = 0U,
		.dither_limit		     = 0U,
		.passive_level 	     = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = 0U
};


#define FAKTOR_DIV 4


volatile static bool out_signal = false;
volatile static bool in_signal = false;
volatile uint16_t correct_fact=0;
volatile uint16_t old_phase_diff= 0;
volatile static bool dir = false;
volatile static bool not_regel = false;

#define REGELUNG

/*********************************************************************************************************************
 * ISR Handler
 ********************************************************************************************************************/
volatile uint16_t array[2000] = {0};
/**
 * Zerocroos detection for Input signal
 */
void VADC0_G0_0_IRQHandler(void)
{
	uint16_t input_freq= XMC_CCU4_SLICE_GetTimerValue(SLICE_PTR_1);
	if(input_freq < 1000){
		return;
	}
	static uint16_t i=0;

	uint16_t period = (uint16_t)((input_freq/16)/FAKTOR_DIV);
	if(i<2000){
		array[i++]= period;
	}else{
		i=0;
	}

#ifdef REGELUNG
	uint16_t steps = (uint16_t)(correct_fact/10);
	if(dir == true){
		XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR_2, period+steps);
	}else{
		XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR_2, period-steps);
	}
#else
	XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR_2, period);
#endif

	//correct_fact = 0;
	/* Enable shadow transfer */
	XMC_CCU4_EnableShadowTransfer(MODULE_PTR_2, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);


	XMC_CCU4_SLICE_StopClearTimer(SLICE_PTR_1);
	XMC_CCU4_SLICE_StartTimer(SLICE_PTR_1);


	XMC_GPIO_ToggleOutput(IN_ZERO);
	in_signal = !in_signal;
}

/**
 * Timer IST to trigger DAC pattern
 */
void CCU41_0_IRQHandler(void)
{
	static int i = 0;

	if(i<((16*FAKTOR_DIV)-1)){
		i++;
	}else{
		i=0;
		XMC_GPIO_ToggleOutput(OUT_ZERO);
		out_signal = !out_signal;
	}


	XMC_DAC_CH_SoftwareTrigger(XMC_DAC0, 0);
	XMC_GPIO_ToggleOutput(TRIGGER_DAC);
}


/*********************************************************************************************************************
 * Initialize Modules
 ********************************************************************************************************************/

/**
 * Initialize DAC for Output signal
 */
static void init_dac_out(void){

	/* API to initial DAC Module*/
	XMC_DAC_CH_Init(XMC_DAC0, 0U, &ch_config0);

	/* API to initial DAC in Pattern mode. When using a predefined pattern a type cast avoid warnings */
	XMC_DAC_CH_StartPatternMode(XMC_DAC0, 0U, pattern, XMC_DAC_CH_PATTERN_SIGN_OUTPUT_DISABLED, XMC_DAC_CH_TRIGGER_SOFTWARE, 1000U);

	/* Set Frequnecy */
	XMC_DAC_CH_SetPatternFrequency(XMC_DAC0, 0, 1000U);
}

/**
 * Initialize ADC for Input signal Zerocross detection
 */
static void init_adc_zero_in(void){

	/* Initialize the VADC global registers */
	XMC_VADC_GLOBAL_Init(VADC, &g_global_config);

	/* Configure a conversion kernel */
	XMC_VADC_GROUP_Init(VADC_GROUP_PTR, &g_group_config);

	/* Enable the analog converters */
	XMC_VADC_GROUP_SetPowerMode(VADC_GROUP_PTR, XMC_VADC_GROUP_POWERMODE_NORMAL);

	/* Perform calibration of the converter */
	XMC_VADC_GLOBAL_StartupCalibration(VADC);

	XMC_VADC_GLOBAL_BackgroundInit(VADC, &g_bgn_handle);
	XMC_VADC_GROUP_ChannelInit(VADC_G0, CHANNEL_NUMBER, &g_channel_config);
	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, CHANNEL_NUMBER);
	XMC_VADC_GLOBAL_BackgroundEnableContinuousMode(VADC);
	XMC_VADC_GLOBAL_BackgroundTriggerConversion(VADC);

	/* Boundary settings */
	XMC_VADC_GROUP_SetBoundaries(VADC_GROUP_PTR, 3000, 35000);
	XMC_VADC_GROUP_ChannelSetBoundarySelection(VADC_GROUP_PTR, CHANNEL_NUMBER, XMC_VADC_BOUNDARY_SELECT_UPPER_BOUND, XMC_VADC_CHANNEL_BOUNDARY_GROUP_BOUND0);
	XMC_VADC_GROUP_ChannelTriggerEventGenCriteria(VADC_GROUP_PTR, CHANNEL_NUMBER, XMC_VADC_CHANNEL_EVGEN_ALWAYS);
	XMC_VADC_GROUP_ChannelSetEventInterruptNode(VADC_GROUP_PTR, CHANNEL_NUMBER, XMC_VADC_SR_GROUP_SR0);
	/* Connect Request Source Event to the NVIC nodes */

	/* Enable IRQ */
	NVIC_SetPriority(VADC0_G0_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
	NVIC_EnableIRQ(VADC0_G0_0_IRQn);
}

/**
 * Initialize Timer for detect Input frequency
 */
static void init_timer_detect_freq(void){

	/* Timer configuration */
	/* Ensure fCCU reaches CCU42 */
	XMC_CCU4_SetModuleClock(MODULE_PTR_1, XMC_CCU4_CLOCK_SCU);
	XMC_CCU4_Init(MODULE_PTR_1, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
	/* Get the slice out of idle mode */
	XMC_CCU4_EnableClock(MODULE_PTR_1, SLICE_NUMBER_1);
	/* Start the prescaler and restore clocks to slices */
	//XMC_CCU4_SLICE_SetPrescaler(SLICE_PTR, 7);
	XMC_CCU4_StartPrescaler(MODULE_PTR_1);
	/* Initialize the Slice */
	XMC_CCU4_SLICE_CompareInit(SLICE_PTR_1, &g_timer_object_1);
	XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR_1, 65000U);
	/* Enable shadow transfer */
	XMC_CCU4_EnableShadowTransfer(MODULE_PTR_1, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
	XMC_CCU4_SLICE_StartTimer(SLICE_PTR_1);

}

/**
 * Initialize Timer for detect phase difference
 */
static void init_timer_detect_phase_diff(void){

	/* Timer configuration */
	/* Ensure fCCU reaches CCU42 */
	XMC_CCU4_SetModuleClock(MODULE_PTR_3, XMC_CCU4_CLOCK_SCU);
	XMC_CCU4_Init(MODULE_PTR_3, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
	/* Get the slice out of idle mode */
	XMC_CCU4_EnableClock(MODULE_PTR_3, SLICE_NUMBER_3);
	/* Start the prescaler and restore clocks to slices */
	//XMC_CCU4_SLICE_SetPrescaler(SLICE_PTR, 7);
	XMC_CCU4_StartPrescaler(MODULE_PTR_3);
	/* Initialize the Slice */
	XMC_CCU4_SLICE_CompareInit(SLICE_PTR_3, &g_timer_object_3);
	XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR_3, 65000U);
	/* Enable shadow transfer */
	XMC_CCU4_EnableShadowTransfer(MODULE_PTR_3, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
	XMC_CCU4_SLICE_StartTimer(SLICE_PTR_3);

}

/**
 * Initialize Timer to generate output frequency
 */
static void init_timer_generate_out_freq(void){

	/* Timer configuration */
	/* Ensure fCCU reaches CCU42 */
	XMC_CCU4_SetModuleClock(MODULE_PTR_2, XMC_CCU4_CLOCK_SCU);
	XMC_CCU4_Init(MODULE_PTR_2, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
	/* Get the slice out of idle mode */
	XMC_CCU4_EnableClock(MODULE_PTR_2, SLICE_NUMBER_2);
	/* Start the prescaler and restore clocks to slices */
	//XMC_CCU4_SLICE_SetPrescaler(SLICE_PTR, 7);
	XMC_CCU4_StartPrescaler(MODULE_PTR_2);
	/* Initialize the Slice */
	XMC_CCU4_SLICE_CompareInit(SLICE_PTR_2, &g_timer_object_2);
	XMC_CCU4_SLICE_EnableEvent(SLICE_PTR_2, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
	XMC_CCU4_SLICE_SetInterruptNode(SLICE_PTR_2, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);
	/* Set priority */
	NVIC_SetPriority(CCU41_0_IRQn, 10U);
	/* Enable IRQ */
	NVIC_EnableIRQ(CCU41_0_IRQn);

	XMC_CCU4_SLICE_SetTimerPeriodMatch(SLICE_PTR_2, 65000U);
	/* Enable shadow transfer */
	XMC_CCU4_EnableShadowTransfer(MODULE_PTR_2, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
	XMC_CCU4_SLICE_StartTimer(SLICE_PTR_2);
}

/**
 * Initialize GPIO Pins
 */
static void init_gpio(void){

	/* Initialize Output Pin */
	XMC_GPIO_CONFIG_t config;

	config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
	config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
	config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;

	XMC_GPIO_Init(IN_ZERO, &config);
	XMC_GPIO_Init(TRIGGER_DAC, &config);
	XMC_GPIO_Init(OUT_ZERO, &config);
	XMC_GPIO_Init(PHASE_DIFF, &config);
	XMC_GPIO_Init(DIR_CORR, &config);
}

/*********************************************************************************************************************
 * Main Application
 ********************************************************************************************************************/

/* Application entry point */
int main(void)
{
	/* Initialize GPIO */
	init_gpio();

	/* Initialize timer to detect input frequency */
	init_timer_detect_freq();

	/* Initialize Timer to generate Output frequency */
	init_timer_generate_out_freq();

	init_timer_detect_phase_diff();

	/* Initialize Zerocross detection Input */
	init_adc_zero_in();

	/* Initialize DAC for Output */
	init_dac_out();

	/* System timer configuration */
	//SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

	bool help_flag = false;

	while(1){
		/* detect phase difference */
		if(!out_signal != !in_signal){
			if(help_flag == false){
				help_flag = true;
				XMC_GPIO_SetOutputHigh(PHASE_DIFF);
				XMC_CCU4_SLICE_StopClearTimer(SLICE_PTR_3);
				XMC_CCU4_SLICE_StartTimer(SLICE_PTR_3);
			}
		}else{
			if(help_flag == true){
				XMC_GPIO_SetOutputLow(PHASE_DIFF);
				correct_fact = XMC_CCU4_SLICE_GetTimerValue(SLICE_PTR_3);
				help_flag = false;
				if(correct_fact > old_phase_diff){
					dir = !dir;
					XMC_GPIO_ToggleOutput(DIR_CORR);
				}
				old_phase_diff = correct_fact;
			}
		}
	}
}

/*EOF*/

