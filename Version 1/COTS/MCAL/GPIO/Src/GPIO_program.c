/****************************************************************
/*   @file   GPIO_program.c
 *   @author Ola Raafat
 *   @brief  The GPIO source file, including functions' definitions
 */
#include<stdint.h>
#include"Stm32F446xx.h"
#include"ErrorType.h"

#include"GPIO_interface.h"
#include"GPIO_private.h"

static GPIO_RegDef_t* GPIOPort[GPIO_PERIPPHERAL_NUM]={GPIOA,GPIOB,GPIOC,GPIOD,GPIOE,GPIOF,GPIOG,GPIOH};

/****************************************************************
/*   @fn         GPIO_u8PinInit
 *   @brief      The function initializes the GPIO pin according to the input parameters
 *   @param[in]  PinConfig: the initialization values of the pin
 *   @retVal     ErrorStatus
 */
uint8_t GPIO_u8PinInit(const PinConfig_t* PinConfig)
{
	uint8_t Local_u8ErrorState = OK;
	if (PinConfig != NULL)
	{
		if((PinConfig -> Port < INVALID_PORT) && (PinConfig ->PinNum <INVALID_PIN))
		{
			/*Select GPIO mode: INPUT, OUTPUT, ANALOG, ALTERNATE_FUNCTION */
			(GPIOPort[PinConfig -> Port] -> MODER) &= ~(MODER_MASK << ((PinConfig ->PinNum)*MODER_PIN_ACCESS));
			(GPIOPort[PinConfig -> Port] -> MODER) |= ((PinConfig->Mode) << ((PinConfig ->PinNum)*MODER_PIN_ACCESS));

			/*Select GPIO pull state: PULLUP, PULLDOWN, NOPULL */
			(GPIOPort[PinConfig -> Port] -> PUPDR) &= ~(PUPDR_MASK << ((PinConfig ->PinNum)*PUPDR_PIN_ACCESS));
			(GPIOPort[PinConfig -> Port] -> PUPDR) |= ((PinConfig->PullType) << ((PinConfig ->PinNum)*PUPDR_PIN_ACCESS));

			/*Select GPIO Output type & Output speed in case of general purpose output or Alternate function*/
			if((PinConfig -> Mode ==OUTPUT) || (PinConfig ->Mode == ALTERNATE_FUNCTION))
			{
				/*Select Output type :Pushpull or open drain */
				(GPIOPort[PinConfig -> Port] -> OTYPER) &= ~(OTYPER_MASK << (PinConfig ->PinNum));
				(GPIOPort[PinConfig -> Port] -> OTYPER) |= ((PinConfig->OutputType) << (PinConfig ->PinNum));

				/*Select Output speed :Low, Medium, High, Fast */
				(GPIOPort[PinConfig -> Port] -> OSPEEDR) &= ~(OSPEEDR_MASK << ((PinConfig ->PinNum)*OSPEEDR_PIN_ACCESS));
				(GPIOPort[PinConfig -> Port] -> OSPEEDR) |= ((PinConfig->Speed) << ((PinConfig ->PinNum)*OSPEEDR_PIN_ACCESS));
				if(PinConfig ->Mode == ALTERNATE_FUNCTION)
				{
					uint8_t Local_u8RegNum =(PinConfig ->PinNum / AFR_PIN_SHIFTING);
					uint8_t Local_u8BitNum =(PinConfig ->PinNum % AFR_PIN_SHIFTING);
					(GPIOPort[PinConfig -> Port] -> AFR[Local_u8RegNum]) &= ~(AFR_MASK << (Local_u8BitNum * AFR_PIN_ACCESS));
					(GPIOPort[PinConfig -> Port] -> AFR[Local_u8RegNum]) |= ((PinConfig->AltFunc) << (Local_u8BitNum * AFR_PIN_ACCESS));
				}
			}
		}
		else
		{
			Local_u8ErrorState = NOK;
		}
	}
	else
	{
		Local_u8ErrorState = NULL_PTR_ERR;
	}
	return Local_u8ErrorState;
}

/****************************************************************
/*   @fn         GPIO_u8SetPinValue
 *   @brief      The function outputs a certain value on an output pin
 *   @param[in]  Port: The port number, get options @Port_t enum
 *   @param[in]  PinNum: The pin number, get options @pin_t enum
 *   @param[in]  PinVal: The output value, get options @PinVal_t enum
 *   @retVal     ErrorStatus
 */
uint8_t GPIO_u8SetPinValue(Port_t Port,Pin_t PinNum,PinVal_t PinVal)
{
	uint8_t Local_u8ErrorState = OK;
	if((Port < INVALID_PORT) && (PinNum <INVALID_PIN))
	{
		switch(PinVal)
		{
		case PIN_LOW:
			(GPIOPort[Port] ->BSRR) = 1<< (16+ PinNum);
			/*(GPIOPort[Port] ->ODR) &= ~(ODR_MASK <<PinNum);*/
			break;
		case PIN_HIGH:
			(GPIOPort[Port] ->BSRR) = 1<< (PinNum);
			/*(GPIOPort[ Port] -> ODR) |= (PinVal<< PinNum);*/
			break;
		default:
			Local_u8ErrorState = NOK;
			break;
		}
	}
	else
	{
		Local_u8ErrorState = NOK;
	}

	return Local_u8ErrorState;
}

/****************************************************************/
/*   @fn         GPIO_u8TogglePinValue
 *   @brief      The function sets the pin using PinVal
 *   @param[in]  Port: The port number, get options @Port_t enum
 *   @param[in]  PinNum: The pin number, get options @pin_t enum
 *   @retVal     ErrorStatus
 */
uint8_t GPIO_u8TogglePinValue(Port_t Port,Pin_t PinNum)
{
	uint8_t Local_u8ErrorState = OK;
	if((Port < INVALID_PORT) && (PinNum <INVALID_PIN))
	{
		GPIOPort[Port]->ODR ^= (ODR_MASK<<PinNum);
	}
	else
	{
		Local_u8ErrorState = NOK;
	}

	return Local_u8ErrorState;
}

/****************************************************************/
/*   @fn         GPIO_u8ReadPinValue
 *   @brief      The function sets the pin using PinVal
*    @param[in]  Port: The port number, get options @Port_t enum
 *   @param[in]  PinNum: The pin number, get options @pin_t enum
 *   @retVal     PinVal: The output value, get options @PinVal_t enum
 */
uint8_t GPIO_u8ReadPinValue(Port_t Port,Pin_t PinNum,PinVal_t *PinVal)
{
	uint8_t Local_u8ErrorState = OK;
	if((Port < INVALID_PORT) && (PinNum <INVALID_PIN))
	{
		*PinVal =IDR_MASK &((GPIOPort[Port] ->IDR)>>PinNum);
	}
	else
	{
		Local_u8ErrorState = NOK;
	}

	return Local_u8ErrorState;
}
