/*
 * BSP_ccu4.h
 *
 *  Created on: 07. Juni 2017
 *      Author: thomas
 */

#ifndef INCLUDE_BSP_CCU4_H_
#define INCLUDE_BSP_CCU4_H_

/******************************************************************** GLOBALS */

#define SLICE_PTR         CCU40_CC40
#define MODULE_PTR        CCU40
#define MODULE_NUMBER     (0U)
#define SLICE_NUMBER      (0U)
#define CAPCOM_MASK       (SCU_GENERAL_CCUCON_GSC40_Msk) /**< Only CCU40 */


/******************************************************************** FUNCTION PROTOTYPES */
void _initCCU40_0();
void _ccu4_start_timer();
void _ccu4_stop_timer();


#endif /* INCLUDE_BSP_CCU4_H_ */
