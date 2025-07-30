/*
 * Rotary.h
 *
 *  Created on: Sep 18, 2024
 *      Author: Administrator
 */

#ifndef INC_ROTARY_H_
#define INC_ROTARY_H_
//*********************************************************************************************
typedef struct {
	GPIO_TypeDef *GPIOx;
	uint16_t dataPin;
	uint16_t sckPin;
	uint16_t oldSckPin;
	uint16_t switchPin;
	uint16_t oldSwitchPin;

	uint32_t counter;
	uint32_t switchCounter;

	uint16_t rotaryState;
	uint16_t rotarySwitchState;

}RotaryHandler;

typedef enum {
	RotaryRight ,
	RotaryLeft ,
	RotaryNone ,
	RotarySwitchON ,
	RotarySwitchOFF
}RotaryNUM;
//*********************************************************************************************
void RotaryInit(RotaryHandler *Rotary, GPIO_TypeDef *GPIOx, uint16_t sckPin, uint16_t dataPin, uint16_t switchPin) {
	Rotary->GPIOx = GPIOx;
	Rotary->dataPin = dataPin;
	Rotary->sckPin = sckPin;
	Rotary->switchPin = switchPin;

	Rotary->oldSckPin = HAL_GPIO_ReadPin(GPIOx, sckPin);
	Rotary->oldSwitchPin = HAL_GPIO_ReadPin(GPIOx, switchPin);

	Rotary->counter = 0;
	Rotary->switchCounter = 0;
}
//*********************************************************************************************
void RotaryBegin(RotaryHandler *Rotary) {
	if(HAL_GPIO_ReadPin(Rotary->GPIOx, Rotary->sckPin) != Rotary->oldSckPin) {
		if(HAL_GPIO_ReadPin(Rotary->GPIOx, Rotary->sckPin) == HAL_GPIO_ReadPin(Rotary->GPIOx, Rotary->dataPin)) {
			HAL_Delay(1);

			Rotary->counter++;
			Rotary->rotaryState = RotaryRight;
		} else {
			HAL_Delay(1);

			Rotary->counter--;
			Rotary->rotaryState = RotaryLeft;
		}

		Rotary->oldSckPin = HAL_GPIO_ReadPin(Rotary->GPIOx, Rotary->sckPin);
	} else {
		Rotary->rotaryState = RotaryNone;
	}

	if(HAL_GPIO_ReadPin(Rotary->GPIOx, Rotary->switchPin) != Rotary->oldSwitchPin) {
		HAL_Delay(200);

		Rotary->switchCounter++;
		Rotary->rotarySwitchState = RotarySwitchON;

		Rotary->oldSwitchPin = HAL_GPIO_ReadPin(Rotary->GPIOx, Rotary->switchPin);
	} else {
		Rotary->rotarySwitchState = RotarySwitchOFF;
	}
}
//*********************************************************************************************
uint32_t RotaryGetCounter(RotaryHandler *Rotary) {
	return Rotary->counter;
}

uint32_t RotaryGetSwitchCounter(RotaryHandler *Rotary) {
	return Rotary->switchCounter;
}
////*********************************************************************************************
void RotarySetCounter(RotaryHandler *Rotary, uint32_t couner) {
	Rotary->counter = couner;
}

void RotarySetSwitchCounter(RotaryHandler *Rotary, uint32_t couner) {
	Rotary->switchCounter = couner;
}
////*********************************************************************************************
uint16_t RotaryGetState(RotaryHandler *Rotary) {
	return Rotary->rotaryState;
}

uint16_t RotaryGetSwitchState(RotaryHandler *Rotary) {
	return Rotary->rotarySwitchState;
}
////*********************************************************************************************
#endif /* INC_ROTARY_H_ */
