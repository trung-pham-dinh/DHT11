# DHT11
implement by HAL on STM32

1. Config a timer which has frequency of 10^6 Hz, Period to maximum number(2^32)
2. Call DHT_init() in "USER CODE 2" and pass the pointer to the timer in step 1 as a parameter
3. Call DHT_EXTI() in HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) callback function
4. Call DHT_read() whenever you need to get the sensor value
5. DHT_getTemperature() and DHT_getHumidity() return sensor value
