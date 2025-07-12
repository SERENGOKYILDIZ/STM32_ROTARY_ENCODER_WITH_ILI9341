/*
 * encoder.h
 *
 *  Created on: Jul 12, 2025
 *      Author: Eren
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include <main.h>

typedef struct{
	GPIO_TypeDef* port;
	uint16_t pin;
}Gpio_pin;

typedef struct{
	Gpio_pin A;
	Gpio_pin B;
	Gpio_pin TG;
	uint16_t MAX;
	uint16_t MIN;
	int count;
	int last_state;
	int tg_state;
}Rotary_Encoder;

void Encoder_Init(Rotary_Encoder* rotary);
void Encoder_Update(Rotary_Encoder* rotary);

#endif /* INC_ENCODER_H_ */
