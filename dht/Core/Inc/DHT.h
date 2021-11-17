/*
 * DHT.h
 *
 *  Created on: Nov 17, 2021
 *      Author: fhdtr
 */

#ifndef INC_DHT_H_
#define INC_DHT_H_

#include "main.h"
#include "DHT_Config.h"

typedef struct {
	TIM_HandleTypeDef* tim;
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;
	float temperature;
	float humidity;
	uint8_t valid;
}DHT;

void DHT_init(TIM_HandleTypeDef* tim);
void DHT_delay_us(uint32_t us);
void DHT_delay_ms(uint32_t ms);
void DHT_output_mode();
void DHT_input_mode();
void DHT_read();
void DHT_EXTI();
void DHT_process();
float DHT_getHumidity();
float DHT_getTemperature();
uint8_t DHT_isValid();

#endif /* INC_DHT_H_ */
