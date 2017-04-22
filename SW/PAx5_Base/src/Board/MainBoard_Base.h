/**
 * created 2016.02.19 by Calin Radoni
 */

#ifndef MainBoard_Base_H_
#define MainBoard_Base_H_

#include "defines.h"
#include "MainBoard_Logger.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

#define NVIC_PRIORITY_SPIDMA     1
#define NVIC_PRIORITY_EXTI_0_1   2 // radio, must be higher then NVIC_PRIORITY_SPIDMA !
#define NVIC_PRIORITY_USART      3
#define NVIC_PRIORITY_EXTI_4_15  3 // user switch
#define NVIC_PRIORITY_TIM21      3 // for entropy
#define NVIC_PRIORITY_ADC        3 // for ADC and ADC entropy
#define NVIC_PRIORITY_I2C        3

// -----------------------------------------------------------------------------

#define PAx5_BOARD_HW_USART        ((uint32_t)0x00000001)    ///< Use USART1
#define PAx5_BOARD_HW_I2C          ((uint32_t)0x00000002)    ///< Use I2C (master or slave)
#define PAx5_BOARD_HW_ExtFLASH     ((uint32_t)0x01000000)    ///< External FLASH present
#define PAx5_BOARD_HW_RFM69HW      ((uint32_t)0x02000000)    ///< RFM69HW module present

// -----------------------------------------------------------------------------

/**
 * sysTickCnt is incremented every milisecond by
 * the SysTick_Handler interrupt handler
 */
extern volatile uint32_t sysTickCnt;

// -----------------------------------------------------------------------------

/** A very basic delay function */
void Delay(uint32_t ms);

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------
#endif /* MainBoard_Base_H_ */
