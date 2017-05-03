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

/** sysTickCnt is incremented every millisecond by the SysTick_Handler interrupt handler */
extern volatile uint32_t sysTickCnt;

// -----------------------------------------------------------------------------

/**
 * \brief A very basic, blocking, delay function
 * \param ms The number of milliseconds to wait
 * \warning This function needs the SysTick interrupt to be enabled !
 */
void Delay(uint32_t ms);

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------
#endif /* MainBoard_Base_H_ */
