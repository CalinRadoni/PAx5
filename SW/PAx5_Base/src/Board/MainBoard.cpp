/**
 * created 2016.02.19 by Calin Radoni
 */

#include "MainBoard.h"

#include "cpu_USART1.h"
#include "cpu_Info.h"
#include "cpu_SPI1.h"
#include "dev_ExternalFlash.h"
#include "dev_RFM69.h"
#include "cpu_Entropy.h"
#include "cpu_ADC.h"
#include "cpu_I2C.h"
#include "dev_LED.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(1)
#define LOG_CODE_START 1 // data = (uint16_t)(RCC->CSR >> 24)
#define LOG_CODE_CLOCK 2 // data = 1 for HSI timeout, 2 for PLL timeout, 3 for clock switching on PLL timeout

// -----------------------------------------------------------------------------

#define TIMEOUT_HSI         ((uint32_t)100U)  // 100 ms
#define TIMEOUT_PLL         ((uint32_t)100U)  // 100 ms
#define TIMEOUT_CLOCKSWITCH	((uint32_t)5000U) // 5 s

// -----------------------------------------------------------------------------

MainBoard board;

// -----------------------------------------------------------------------------

BoardDefinion::BoardDefinion() {}

/*
BoardDefinion& BoardDefinion::operator=(const BoardDefinion& src)
{
	if(&src == this) return *this; // self-assignment

	Use_USART            = src.Use_USART;
	Use_I2C              = src.Use_I2C;
	Use_I2C_Slave        = src.Use_I2C_Slave;
	ExtFLASH             = src.ExtFLASH;
	Radio_RFM69HW        = src.Radio_RFM69HW;
	PowerPeripherals_PA3 = src.PowerPeripherals_PA3;
	...

	return *this;
}
*/

void BoardDefinion::SetByType(BoardType type)
{
	Use_USART     = true;
	Use_I2C       = true; Use_I2C_Slave = false;
	ExtFLASH      = true;
	Radio_RFM69HW = true;

	switch(type){
		case BoardType::PAx5_BaseBoard:
			PowerPeripherals_PA3 = false;
			break;

		case BoardType::PAx5_EnvSensor:
			PowerPeripherals_PA3 = true;
			break;
	}
}

// -----------------------------------------------------------------------------

MainBoard::MainBoard()
{
	brdErr = Error::NotInitialized;
	radioIntFired = false;
}

// -----------------------------------------------------------------------------

MainBoard::Error MainBoard::InitializeBoard(BoardDefinion& boardDef)
{
	uint16_t resetSource;

	// store reset informations from RCC_CSR register ...
	resetSource = (uint16_t)(RCC->CSR >> 24);
	// ... clear reset flags ...
	RCC->CSR |= RCC_CSR_RMVF;
	// ... and add an entry to HWLog
	hwLogger.AddEntry(FileID, LOG_CODE_START, resetSource);

	boardCapabilities = boardDef;

	brdErr = Error::OK;

	// clock configured for 2MHz in SystemInit() function called from startup_stm32l0xx.s
	SysTick_Config(2000); // 1ms
	ConfigureClock();
	SystemCoreClockUpdate();

	if(brdErr == Error::OK){
		SysTick_Config(32000); // 1ms

		ConfigureGPIO();

		if(boardCapabilities.Use_USART) {
			if(!sUSART.Configure())
				brdErr = Error::USART;
		}
	}

	// power the peripherals if needed
	if(boardCapabilities.PowerPeripherals_PA3){
		GPIOA->BSRR = GPIO_BSRR_BR_3;
	}

	if(brdErr == Error::OK){
		sSPI.Configure();
		sSPI.defaultSPISettings = true;

		ConfigureEXTI();

		if(boardCapabilities.ExtFLASH) {
			sExternalFlash.Initialize();
			sExternalFlash.PowerDown();
		}

		radioIntFired = 0;
		if(boardCapabilities.Radio_RFM69HW) {
			sRadio.initUseAFC = false;
			sRadio.Initialize();
			if(sRadio.interfaceError)
				brdErr = Error::Radio;
		}
	}

	return brdErr;
}

// -----------------------------------------------------------------------------

void MainBoard::BlinkError(void){

	if(brdErr == Error::OK) return;

	uint8_t errIn = (uint8_t)brdErr;

	sLED.Blink(1000);
	while(errIn != 0){
		sLED.Blink(300);
		--errIn;
	}
}

// -----------------------------------------------------------------------------

/** @brief Configure system clock to 32 MHz using HSI
 *
 * This function configures the system clock @32MHz and voltage scale 1
 * assuming the registers have their reset value before the call.
 * - POWER SCALE   = RANGE 1
 * - SYSTEM CLOCK  = PLL MUL16 DIV2
 * - PLL SOURCE    = HSI/4
 * - FLASH LATENCY = 1
 */
void MainBoard::ConfigureClock(void)
{
	uint32_t tickstart;

	// Enable power interface clock
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN);

	// Select voltage scale 1 (1.8V)
	PWR->CR = (PWR->CR & ~(PWR_CR_VOS)) | PWR_CR_VOS_0;

	// Select flash to be 1 wait-state (required for 32MHZ on voltage scale 1)
	FLASH->ACR |= FLASH_ACR_LATENCY;

	// Enable HSI and HSI divided by 4 ...
	RCC->CR |= RCC_CR_HSION | RCC_CR_HSIDIVEN;
	// ... and wait for HSI to be ready
	tickstart = sysTickCnt;
	while((RCC->CR & (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)){
		if((sysTickCnt - tickstart) > TIMEOUT_HSI){
			brdErr = Error::HSI_Timeout;
			hwLogger.AddEntry(FileID, LOG_CODE_CLOCK, 1);
			return;
		}
	}

	// Enable PLL for HSI, multiply by 16 and divided by 2, then start PLL ...
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL16 | RCC_CFGR_PLLDIV2;
	RCC->CR |= RCC_CR_PLLON;
	// ... and wait for PLL to be ready
	tickstart = sysTickCnt;
	while((RCC->CR & RCC_CR_PLLRDY) == 0){
		if ((sysTickCnt - tickstart) > TIMEOUT_PLL){
			brdErr = Error::PLL_Timeout;
			hwLogger.AddEntry(FileID, LOG_CODE_CLOCK, 2);
			return;
		}
	}

	// Select PLL as system clock ...
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	// ... and wait for clock to switch on PLL
	tickstart = sysTickCnt;
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0){
		if((sysTickCnt - tickstart ) > TIMEOUT_CLOCKSWITCH){
			brdErr = Error::ClockSwitch_Timeout;
			hwLogger.AddEntry(FileID, LOG_CODE_CLOCK, 3);
			return;
		}
	}
}

// -----------------------------------------------------------------------------
/** @brief GPIO Configuration
 *
 *  This function configures all GPIO:
 *     PA8 as input (UserSW), - EXTI8
 *     PA15 as output (UserLED),
 *     PA5, PA6 and PA7 for SPI1
 *     the pins required through the BoardDefinition object
 *     the other pins as analog
 */
void MainBoard::ConfigureGPIO(void)
{
	uint32_t val;

	/** Default values
	 *
	 * After reset:
	 * - PA4 (SPI1 NSS) default == 0  (input)
	 * - PA13 (SWDIO)   default == 10 (alternate)
	 * - PA14 (SWCLK)   default == 10 (alternate)
	 * - the others are default == 11 (analog mode)
	 */

	// Enable peripheral clock of GPIOx
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN;

	// Configure GPIO pins PA0 - PA2, PA11, PA12 in analog mode
	val = GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12;
	if(!(boardCapabilities.PowerPeripherals_PA3)) {
		// configure PA3 in analog mode
		val |= GPIO_MODER_MODE3;
	}
	if(!(boardCapabilities.Use_USART)) {
		// configure USART1 pins, PA9 and PA10, in analog mode
		val |= GPIO_MODER_MODE9 | GPIO_MODER_MODE10;
	}
	if(!(boardCapabilities.ExtFLASH)) {
		// configure PA4 in analog mode
		val |= GPIO_MODER_MODE4;
	}
	GPIOA->MODER |= val; // It is OK like this because GPIO_MODER_MODE* are all 1

	// configure peripheral power if needed
	if(boardCapabilities.PowerPeripherals_PA3){
		// PA3 (Peripheral power) configured as output push-pull, low speed, value HIGH
		GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE3)) | GPIO_MODER_MODE3_0;
		GPIOA->OTYPER  =  GPIOA->OTYPER & ~(GPIO_OTYPER_OT_3);
		GPIOA->OSPEEDR =  GPIOA->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED3);
		GPIOA->BSRR    =  GPIO_BSRR_BS_3;
	}

	// Configure GPIO pins PB3 - PB5 as analog pins
	val = GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5;
	if(!(boardCapabilities.Radio_RFM69HW)) {
		// configure the pin for selecting the radio module, PB0, in analog mode
		val |= GPIO_MODER_MODE0;
		// configure the pin used as interrupt input from radio module, PB1, in analog mode
		val |= GPIO_MODER_MODE1;
	}
	if(!(boardCapabilities.Use_I2C)) {
		// Configure I2C1 pins, PB6 and PB7, as analog pins
		val |= GPIO_MODER_MODE6 | GPIO_MODER_MODE7;
	}
	GPIOB->MODER |= val; // It is OK like this because GPIO_MODER_MODE* are all 1

	// Configure GPIO pins PC14 PC15 as analog pins
	GPIOC->MODER |= GPIO_MODER_MODE14 | GPIO_MODER_MODE15;

	// No pull-up or pull-down
	GPIOA->PUPDR = 0x24000000; // default value, after reset
	GPIOB->PUPDR = 0;
	GPIOC->PUPDR = 0;

// ======= User switch
	// PA8 (UserSW) configured as input with pull-up
	GPIOA->MODER = GPIOA->MODER & ~(GPIO_MODER_MODE8);
	GPIOA->PUPDR = (GPIOA->PUPDR & ~(GPIO_PUPDR_PUPD8)) | GPIO_PUPDR_PUPD8_0;

// ======= User LED
	// PA15 (UserLED) configured as open-drain, low speed, value HIGH
	GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE15)) | GPIO_MODER_MODE15_0;
	GPIOA->OTYPER |= GPIO_OTYPER_OT_15;
	GPIOA->OSPEEDR = GPIOA->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED15);
	GPIOA->BSRR    = GPIO_BSRR_BS_15;

// ======= USART1 pins
	if(boardCapabilities.Use_USART) {
		// PA9 and PA10 configured as alternate function
		GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10)) | (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
		// Select the alternate function (AFR[0] is AFRL (pin0-7) and AFR[1] is AFRH (pin8-15))
		// UART1 is AF4 (0100) for PA9 and PA10
		GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(0x00000FF0)) | 0x00000440;
	}

// ======= SPI slave select pins
	if(boardCapabilities.ExtFLASH) {
		// PA4 (SPI_SelMem) configured as output push-pull, high speed, value HIGH
		GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE4)) | GPIO_MODER_MODE4_0;
		GPIOA->OTYPER  =  GPIOA->OTYPER & ~(GPIO_OTYPER_OT_4);
		GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED4)) | GPIO_OSPEEDER_OSPEED4_1;
		GPIOA->BSRR    =  GPIO_BSRR_BS_4;
	}
	if(boardCapabilities.Radio_RFM69HW) {
		// PB0 (SPI_SelRadio) configured as output push-pull, high speed, value HIGH
		GPIOB->MODER   = (GPIOB->MODER & ~(GPIO_MODER_MODE0)) | GPIO_MODER_MODE0_0;
		GPIOB->OTYPER  =  GPIOB->OTYPER & ~(GPIO_OTYPER_OT_0);
		GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED0)) | GPIO_OSPEEDER_OSPEED0_1;
		GPIOB->BSRR    =  GPIO_BSRR_BS_0;
	}

// ======= SPI pins
	// PA5 = SPI1 Clock : Output push-pull, Very high speed, value LOW
	GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_0;
	GPIOA->OTYPER  =  GPIOA->OTYPER & ~(GPIO_OTYPER_OT_5);
	GPIOA->OSPEEDR =  GPIOA->OSPEEDR | GPIO_OSPEEDER_OSPEED5;
	GPIOA->BSRR    =  GPIO_BSRR_BR_5;
	// PA6 = SPI1 MISO : Input with pull-up
	GPIOA->MODER   =  GPIOA->MODER & ~(GPIO_MODER_MODE6);
	// TODO !!! Check without setting the speed. This should not matter for inputs !
	//GPIOA->OSPEEDR =  GPIOA->OSPEEDR | GPIO_OSPEEDER_OSPEED6;
	GPIOA->PUPDR   = (GPIOA->PUPDR & ~(GPIO_PUPDR_PUPD6)) | GPIO_PUPDR_PUPD6_0;
	// PA7 = SPI1 MOSI : Output push-pull, Very high speed, value LOW
	GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE7)) | GPIO_MODER_MODE7_0;
	GPIOA->OTYPER  =  GPIOA->OTYPER & ~(GPIO_OTYPER_OT_7);
	GPIOA->OSPEEDR =  GPIOA->OSPEEDR | GPIO_OSPEEDER_OSPEED7;
	GPIOA->BSRR    =  GPIO_BSRR_BR_7;
	// PA5, PA6 and PA7 configured as alternate function
	GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7)) |
						(GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	// Select the alternate function (AFR[0] is AFRL (pin0-7) and AFR[1] is AFRH (pin8-15))
	// SPI1 is AF0 (0000) for PA5, PA6 and PA7
	GPIOA->AFR[0]  = (GPIOA->AFR[0] & 0x000FFFFF);

// ======= Interrupt pin connected to RFM69
	if(boardCapabilities.Radio_RFM69HW) {
		// PB1 (RadioInterrupt) configured as input with pull-down
		GPIOB->MODER = GPIOB->MODER & ~(GPIO_MODER_MODE1);
		GPIOB->PUPDR = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD1)) | GPIO_PUPDR_PUPD1_1;
	}

// ======= I2C pins
	if(boardCapabilities.Use_I2C) {
		// PB6 - PB7 configured as open-drain, high speed, value HIGH and pull-up
		GPIOB->PUPDR   = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7)) |
							(GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);
		GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;
		GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(GPIO_OSPEEDER_OSPEED6 | GPIO_OSPEEDER_OSPEED7)) |
							(GPIO_OSPEEDER_OSPEED6_1 | GPIO_OSPEEDER_OSPEED7_1);
		// Select the alternate function (AFR[0] is AFRL (pin0-7) and AFR[1] is AFRH (pin8-15))
		// I2C1 is AF1 (0001) for PB6 and PB7
		GPIOB->AFR[0]  = (GPIOB->AFR[0] & 0x00FFFFFF) | 0x11000000;
		GPIOB->MODER   = (GPIOB->MODER & ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7)) |
							(GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	}
}

// -----------------------------------------------------------------------------

void MainBoard::ConfigureEXTI(void)
{
	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// PA is the source for EXTI8 interrupt
	SYSCFG->EXTICR[2] = (SYSCFG->EXTICR[2] & ~(SYSCFG_EXTICR3_EXTI8)) | SYSCFG_EXTICR3_EXTI8_PA;

	EXTI->IMR  = EXTI->IMR  | EXTI_IMR_IM8;     // enable Line 8
	EXTI->RTSR = EXTI->RTSR | EXTI_RTSR_TR8;    // enable rising edge
	EXTI->FTSR = EXTI->FTSR & ~(EXTI_FTSR_TR8); // disable falling edge

	NVIC_SetPriority(EXTI4_15_IRQn, NVIC_PRIORITY_EXTI_4_15);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
}

// -----------------------------------------------------------------------------

void MainBoard::EnableTIM6(void)
{
	// The counter counts from 0 to the auto-reload value (contents of the TIMx_ARR register),
	// then restarts from 0 and generates a counter overflow event.

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	TIM6->CR1  = TIM_CR1_ARPE | TIM_CR1_URS | TIM_CR1_UDIS;
	TIM6->DIER = 0; // no interrupts
	TIM6->CR2  = 0;
	TIM6->SR   = TIM6->SR & ~(TIM_SR_UIF);
	TIM6->PSC  = 32000; // 1ms time base
	TIM6->ARR  = 0xFFFF;
	TIM6->EGR  = TIM6->EGR | TIM_EGR_UG; // update PSC, ARR, clear prescaler and counter

	/* To start:
	 *  - TIM6->ARR = milliseconds / reload cycle
	 *  - TIM6->EGR = TIM6->EGR | TIM_EGR_UG; // update PSC, ARR, clear prescaler and counter
	 * 	- set TIM6->CR1 |= TIM_CR1_CEN
	 */
}
void MainBoard::DisableTIM6(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	TIM6->CR1  = TIM6->CR1 & ~(TIM_CR1_CEN); // stop
	TIM6->DIER = 0; // no interrupts
	TIM6->SR   = TIM6->SR & ~(TIM_SR_UIF); // clear flags

	RCC->APB1ENR = RCC->APB1ENR & ~(RCC_APB1ENR_TIM6EN);
}

// -----------------------------------------------------------------------------

void MainBoard::RadioIntFired(void)
{
	radioIntFired = true;
}
void MainBoard::CheckRadioInterrupt(void)
{
	if(radioIntFired){
		radioIntFired = false;
		sRadio.HandleInterrupt();
	}
}

} /* namespace */

// -----------------------------------------------------------------------------
// STM32L0xx Peripherals Interrupt Handlers (peripheral interrupt handler's names are in startup_stm32l0xx.s)
// -----------------------------------------------------------------------------

extern "C" void EXTI0_1_IRQHandler(void)
{
	// EXTI flags are cleared by writting 1 to them (as the manual says)

	if((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0){
		EXTI->PR = EXTI_PR_PR0;
	}

	if((EXTI->PR & EXTI_PR_PR1) == EXTI_PR_PR1){
		EXTI->PR = EXTI_PR_PR1;
		if(PAx5::sSPI.defaultSPISettings){
			PAx5::sRadio.HandleInterrupt();
		}
		else{
			PAx5::board.RadioIntFired();
		}
	}
}

extern "C" void EXTI4_15_IRQHandler(void)
{
	// EXTI flags are cleared by writting 1 to them (as the manual says)

	if((EXTI->PR & EXTI_PR_PR8) == EXTI_PR_PR8){
		EXTI->PR = EXTI_PR_PR8;
		//exti8Rised = true;
	}
}

// -----------------------------------------------------------------------------
