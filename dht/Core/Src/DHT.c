/*
 * DHT.c
 *
 *  Created on: Nov 17, 2021
 *      Author: fhdtr
 */

#include "DHT.h"

static DHT dht;
volatile uint32_t currCNT = 0, preCNT = 0;
volatile uint8_t indexing = 0;
static uint8_t data[5] = {0};
volatile uint32_t cnt[41] = {0};
volatile uint8_t start = 0;
volatile uint8_t read = 0;
volatile uint8_t done = 0;

void DHT_init(TIM_HandleTypeDef* tim) {
	dht.GPIO_Pin = DHT_DATA_PIN;
	dht.GPIO_Port = DHT_DATA_PORT;
	dht.tim = tim;
	dht.humidity = 0;
	dht.temperature = 0;
	dht.valid = 0;

	HAL_TIM_Base_Start(tim);
	HAL_Delay(10); // delayyyyyyyyyyyyyyyyyyyyyyyyy
}

void DHT_delay_us(uint32_t us) {
	dht.tim->Instance->CNT = 0;
	while(dht.tim->Instance->CNT < us);
}
void DHT_delay_ms(uint32_t ms) {
	HAL_Delay(ms);
}

void DHT_output_mode() {
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_GPIO_WritePin(dht.GPIO_Port, dht.GPIO_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = dht.GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // we already have pull up
	GPIO_InitStruct.Pull = GPIO_NOPULL; // we already have pull up
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(dht.GPIO_Port, &GPIO_InitStruct);
}

void DHT_input_mode() {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = dht.GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(dht.GPIO_Port, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void DHT_EXTI() {
	static volatile uint8_t start = 1;

	if(start) {
		start = 0;
		dht.tim->Instance->CNT = 0;
		preCNT = 0;
	}
	else {
		if(indexing < 41) {
			cnt[indexing++] = dht.tim->Instance->CNT - preCNT;
			preCNT = dht.tim->Instance->CNT;
			if(indexing == 41) {
				start = 1;
			}
		}
	}

}

void DHT_read() {
	DHT_output_mode();

	HAL_GPIO_WritePin(dht.GPIO_Port, dht.GPIO_Pin, 0);
	DHT_delay_ms(18);
	HAL_GPIO_WritePin(dht.GPIO_Port, dht.GPIO_Pin, 1);
	DHT_delay_us(20);

	indexing = 0;
	DHT_input_mode();



	while(indexing < 41);
	DHT_output_mode();

	DHT_process();
}

void DHT_process() {
	for(int i = 1; i < 41; i++) {
		data[(i-1)/8] <<= 1;
		data[(i-1)/8] |= (cnt[i] > 100);
	}
	if(data[0] + data[1] + data[2] + data[3] == data[4]) { // check sum
		dht.valid = 1;
		dht.humidity = (float)data[0] + 0.1*data[1];
		dht.temperature = (float)data[2] + 0.1*data[3];
	}
	else {
		dht.valid = 0;
	}
}

float DHT_getHumidity() {
	return dht.humidity;
}

float DHT_getTemperature() {
	return dht.temperature;
}

uint8_t DHT_isValid() {
	return dht.valid;
}
