/**
 * created 2017.04.23 by Calin Radoni
 */

#include "cpu_GPIO.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(10)
#define LOG_CODE_AttachToPin  1 // data = the wrong GPIOx
#define LOG_CODE_SetMode      2 // data = (uint16_t)pinNumber
#define LOG_CODE_SetModeSpeed 3 // data = 0
#define LOG_CODE_SetModeMode  4 // data = 0
#define LOG_CODE_SetModePull  5 // data = 0
#define LOG_CODE_GetMode      6 // data = (uint16_t)pinNumber
#define LOG_CODE_SetPin       7 // data = (uint16_t)pinNumber
#define LOG_CODE_ResetPin     8 // data = (uint16_t)pinNumber
#define LOG_CODE_GetPin       9 // data = (uint16_t)pinNumber

// -----------------------------------------------------------------------------

CPU_GPIO::CPU_GPIO() { }

CPU_GPIO::~CPU_GPIO() { }

// -----------------------------------------------------------------------------

// TODO based on a define make a 32 bit value that define available pins !!!
// TODO nu sunt define-uri pentru procesor ???
#ifdef STM32L051K8
const uint32_t AvailablePinsA = 0x00FF check !;
const uint32_t AvaliablePinsB = 0x00FF check !;
const uint32_t AvailablePinsC = 0x00C0;
#endif

void CPU_GPIO::AttachToPin(uint8_t pinID)
{
	pinNumber = pinID & 0x0F;

	uint8_t id = pinID & 0xF0;
	switch(id){
		case 0:  GPIOx = GPIOA; break;
		case 1:  GPIOx = GPIOB; break;
		case 2:  GPIOx = GPIOC; break;
		default:
			GPIOx = nullptr;
			hwLogger.AddEntry(FileID, LOG_CODE_AttachToPin, (uint16_t)id);
			break;
	}
}

// -----------------------------------------------------------------------------

bool CPU_GPIO::SetMode(const CPU_GPIO_Definition& pinDefinition)
{
	if((GPIOx == nullptr) || (pinNumber > 0x0F)){
		hwLogger.AddEntry(FileID, LOG_CODE_SetMode, (uint16_t)pinNumber);
		return false;
	}

	uint32_t tmp, val;

	/** GPIO alternate function registers,
	 * 4 bits / pin
	 * GPIOx->AFR[0] = GPIOx->AFRL (address offset 0x20), pins 0 - 7
	 * GPIOx->AFR[1] = GPIOx->AFRH (address offset 0x24), pins 8 - 15
	 *
	 * GPIOx_AFRL[31:28] = AFSEL7[3:0]  ... GPIOx_AFRL[3:0] = AFSEL0[3:0]
	 * GPIOx_AFRH[31:28] = AFSEL15[3:0] ... GPIOx_AFRH[3:0] = AFSEL8[3:0]
	 */
	// if alternate function, set it
    if((pinDefinition.mode == PinMode::Alternate) || (pinDefinition.mode == PinMode::AlternateOD)){
    	val = (uint32_t)(pinDefinition.altFunction & 0x0F);

		uint32_t pinNumberAFRL = pinNumber & (uint32_t)0x07U;
		tmp = GPIOx->AFR[pinNumber >> 3U];
		tmp &= ~(GPIO_AFRL_AFRL0_Msk << (pinNumberAFRL * 4U));
		tmp |= (val << (pinNumberAFRL * 4U));
		GPIOx->AFR[pinNumber >> 3U] = tmp;
    }

    /** GPIO port output speed register and GPIO port output type register
     * 2 bits/pin
     * GPIOx_OSPEEDR[31:30] = OSPEED15[1:0] ... GPIOx_OSPEEDR[1:0] = OSPEED0[1:0]
     * 1 bit/pin
     * GPIOx_OTYPER[15] = OT15 ... GPIOx_OTYPER[0] = OT0
     */
	// if output or alternate ...
    if((pinDefinition.mode == PinMode::Output) || (pinDefinition.mode == PinMode::OutputOD) ||
    		(pinDefinition.mode == PinMode::Alternate) || (pinDefinition.mode == PinMode::AlternateOD)){
    	// ... set the speed ...
		switch(pinDefinition.speed){
			case PinSpeed::Low:      val = 0U; break;
			case PinSpeed::Medium:   val = 1U; break;
			case PinSpeed::High:     val = 2U; break;
			case PinSpeed::VeryHigh: val = 3U; break;
			default:
				val = 0U; // default to low speed
				hwLogger.AddEntry(FileID, LOG_CODE_SetModeSpeed, 0);
				break;
		}
		tmp = GPIOx->OSPEEDR;
		tmp &= ~(GPIO_OSPEEDER_OSPEED0 << (pinNumber * 2U));
		tmp |= (val << (pinNumber * 2U));
		GPIOx->OSPEEDR = tmp;

		// ... and output type
		tmp= GPIOx->OTYPER;
		tmp &= ~(GPIO_OTYPER_OT_0 << pinNumber) ;
		if((pinDefinition.mode == PinMode::OutputOD) || (pinDefinition.mode == PinMode::AlternateOD))
			tmp |= (1U << pinNumber);
		GPIOx->OTYPER = tmp;
    }

    /** GPIO port mode register
     * 2 bits/pin
     * GPIOx_MODER[31:30] = MODE15[1:0] ... GPIOx_MODER[1:0] = MODE0[1:0]
     */
	// set the mode
    switch(pinDefinition.mode){
    	case PinMode::Analog:      val = 3U; break;
    	case PinMode::Input:       val = 0U; break;
    	case PinMode::Output:      val = 1U; break;
    	case PinMode::OutputOD:    val = 1U; break;
    	case PinMode::Alternate:   val = 2U; break;
    	case PinMode::AlternateOD: val = 2U; break;
    	default:
    		val = 3U; // default to Analog
    		hwLogger.AddEntry(FileID, LOG_CODE_SetModeMode, 0);
    		break;
    }
    tmp = GPIOx->MODER;
    tmp &= ~(GPIO_MODER_MODE0 << (pinNumber * 2U));
    tmp |= (val << (pinNumber * 2U));
    GPIOx->MODER = tmp;

    /** GPIO port pull-up/pull-down register
     * 2 bits/pin
     * GPIOx_PUPDR[31:30] = PUPD15[1:0] ... GPIOx_PUPDR[1:0] = PUPD0[1:0]
     */
	// set the pull-up or pull-down
    if(pinDefinition.mode == PinMode::Analog){
    	// in Analog mode no pull-up/pull-down allowed
    	val = 0U;
    }
    else{
		switch(pinDefinition.pull){
			case PinPull::None: val = 0U; break;
			case PinPull::Up:   val = 1U; break;
			case PinPull::Down: val = 2U; break;
			default:
				val = 0U; // default to NO pull-up/pull-down
				hwLogger.AddEntry(FileID, LOG_CODE_SetModePull, 0);
				break;
		}
    }
    tmp = GPIOx->PUPDR;
    tmp &= ~(GPIO_PUPDR_PUPD0 << (pinNumber * 2U));
    tmp |= (val << (pinNumber * 2U));
    GPIOx->PUPDR = tmp;

	return true;
}

void CPU_GPIO::GetMode(CPU_GPIO_Definition& pinDefinition)
{
	if((GPIOx == nullptr) || (pinNumber > 0x0F)){
		hwLogger.AddEntry(FileID, LOG_CODE_GetMode, (uint16_t)pinNumber);
		return;
	}

	uint32_t val;

	uint32_t pinNumberAFRL = pinNumber & (uint32_t)0x07U;
	val = GPIOx->AFR[pinNumber >> 3U];
	val = (val >> (pinNumberAFRL * 4U)) & GPIO_AFRL_AFRL0_Msk;
	pinDefinition.altFunction = val;

	val = GPIOx->OSPEEDR;
    val = (val >> (pinNumber * 2U)) & GPIO_OSPEEDER_OSPEED0;
    switch(val){
    	case 0: pinDefinition.speed = PinSpeed::Low;     break;
    	case 1: pinDefinition.speed = PinSpeed::Medium;       break;
    	case 2: pinDefinition.speed = PinSpeed::High;     break;
    	case 3: pinDefinition.speed = PinSpeed::VeryHigh; break;
    }

	val= GPIOx->OTYPER;
	val = (val >> pinNumber) & GPIO_OTYPER_OT_0;
	bool openDrain = (val != 0);

	val = GPIOx->MODER;
    val = (val >> (pinNumber * 2U)) & GPIO_MODER_MODE0;
    switch(val){
    	case 0: pinDefinition.mode = PinMode::Input;     break;
    	case 1: pinDefinition.mode = openDrain ? PinMode::OutputOD    : PinMode::Output;    break;
    	case 2: pinDefinition.mode = openDrain ? PinMode::AlternateOD : PinMode::Alternate; break;
    	case 3: pinDefinition.mode = PinMode::Analog;    break;
    }

    val = GPIOx->PUPDR;
    val = (val >> (pinNumber * 2U)) & GPIO_PUPDR_PUPD0;
    switch(val){
    	case 0: pinDefinition.pull = PinPull::None;     break;
    	case 1: pinDefinition.pull = PinPull::Up;       break;
    	case 2: pinDefinition.pull = PinPull::Down;     break;
    	case 3: pinDefinition.pull = PinPull::reserved; break;
    }
}

// -----------------------------------------------------------------------------

void CPU_GPIO::SetPin(void)
{
	if((GPIOx == nullptr) || (pinNumber > 0x0F)){
		hwLogger.AddEntry(FileID, LOG_CODE_SetPin, (uint16_t)pinNumber);
		return;
	}

	/** GPIO port bit set/reset register
	 * GPIOx_BSRR[31:16] = BR[15:0] (reset)
	 * GPIOx_BSRR[15:0]  = BS[15:0] (set)
	 */
	uint32_t tmp = 1U;
	GPIOx->BSRR = tmp << pinNumber;
}
void CPU_GPIO::ResetPin(void)
{
	if((GPIOx == nullptr) || (pinNumber > 0x0F)){
		hwLogger.AddEntry(FileID, LOG_CODE_ResetPin, (uint16_t)pinNumber);
		return;
	}

	/** GPIO port bit reset register
	 * GPIOx_BRR[31:16] = reserved
	 * GPIOx_BRR[15:0]  = BR[15:0] (reset)
	 */
	uint32_t tmp = 1U;
	GPIOx->BRR = tmp << pinNumber;
}
bool CPU_GPIO::GetPin(void)
{
	if((GPIOx == nullptr) || (pinNumber > 0x0F)){
		hwLogger.AddEntry(FileID, LOG_CODE_GetPin, (uint16_t)pinNumber);
		return false;
	}

	/** GPIO port input data register
	 * GPIOx_IDR[31:16] = reserved
	 * GPIOx_IDR[15:0]  = ID[15:0] (reset)
	 */
	uint32_t tmp = 1U;
	tmp = tmp << pinNumber;
	tmp = GPIOx->IDR & tmp;
	return (tmp != 0U);
}

// -----------------------------------------------------------------------------

} /* namespace PAx5 */
