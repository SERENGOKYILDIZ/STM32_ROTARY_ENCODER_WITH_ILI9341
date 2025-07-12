/*
 * encoder.c
 *
 *  Created on: Jul 12, 2025
 *      Author: Eren
 */
#include "encoder.h"

void Encoder_Init(Rotary_Encoder* rotary)
{
	if(rotary->MAX==0)rotary->MAX=100;
	if(rotary->MIN==0)rotary->MIN=0;

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = rotary->A.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(rotary->A.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = rotary->B.pin;
    HAL_GPIO_Init(rotary->B.port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = rotary->TG.pin;
    HAL_GPIO_Init(rotary->TG.port, &GPIO_InitStruct);

    uint8_t a = HAL_GPIO_ReadPin(rotary->A.port, rotary->A.pin);
    uint8_t b = HAL_GPIO_ReadPin(rotary->B.port, rotary->B.pin);

    rotary->last_state = (a << 1) | b;

    rotary->tg_state=0;
}

void Encoder_Update(Rotary_Encoder* rotary) {
    uint8_t a = HAL_GPIO_ReadPin(rotary->A.port, rotary->A.pin);
    uint8_t b = HAL_GPIO_ReadPin(rotary->B.port, rotary->B.pin);
    uint8_t current_state = (a << 1) | b;

    if (current_state != rotary->last_state) {
        if ((rotary->last_state == 0b00 && current_state == 0b01) ||
            (rotary->last_state == 0b01 && current_state == 0b11) ||
            (rotary->last_state == 0b11 && current_state == 0b10) ||
            (rotary->last_state == 0b10 && current_state == 0b00)) {
            if (rotary->count < rotary->MAX)
            	rotary->count ++;
        } else {
            if (rotary->count  > rotary->MIN)
            	rotary->count --;
        }
        rotary->last_state = current_state;
    }

    rotary->tg_state = HAL_GPIO_ReadPin(rotary->TG.port, rotary->TG.pin);
}
