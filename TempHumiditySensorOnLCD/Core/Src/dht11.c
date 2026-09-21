
#include "dht11.h"
#include "main.h"
#include "lcd.h"
#include <string.h>

// For testing the duration of HIGH in read bit function
uint8_t data_ms[40];

extern TIM_HandleTypeDef htim6;

void DHT_SetOutput() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = dht_data_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(dht_data_GPIO_Port, &GPIO_InitStruct);
}

void DHT_SetInput(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = dht_data_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(dht_data_GPIO_Port, &GPIO_InitStruct);
}

uint8_t DHT_ReadBit(uint8_t x){

	// Loop while DATA is LOW
	while ((HAL_GPIO_ReadPin(dht_data_GPIO_Port, dht_data_Pin) == GPIO_PIN_RESET))	{
	}
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	//RESET when DATA turns HIGH


	//LOOP to count microseconds on HIGH
	while ((HAL_GPIO_ReadPin(dht_data_GPIO_Port, dht_data_Pin) == GPIO_PIN_SET))	{
			}

	uint32_t pulseTime = __HAL_TIM_GET_COUNTER(&htim6);
	data_ms[x] = pulseTime;

	 return (pulseTime > 45) ? 1 : 0;
}


void DHT_ReadData(uint8_t *data){

	for (int i = 0; i < 5; i++){

		// building the byte.
		uint8_t byte = 0;

		// Loop 8 times per byte of data
		for (int y = 0; y < 8; y++){
			byte = (byte << 1) | DHT_ReadBit(i+y);
		}

		// store the data in array
		data[i] = byte;
	}

}

bool DHT_CheckInit(void)
{

    /*
     * Wait for DHT11 to pull DATA LOW
     */
    __HAL_TIM_SET_COUNTER(&htim6, 0);

    while (HAL_GPIO_ReadPin(
        dht_data_GPIO_Port,
        dht_data_Pin) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 100)
        {
            return false;
        }
    }

    /*
     * DHT11 is now LOW.
     * Wait for it to go HIGH.
     */
    __HAL_TIM_SET_COUNTER(&htim6, 0);

    while (HAL_GPIO_ReadPin(
        dht_data_GPIO_Port,
        dht_data_Pin) == GPIO_PIN_RESET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 100)
        {
            return false;
        }
    }

    /*
     * DHT11 is now HIGH.
     * Wait for it to go LOW.
     */
    __HAL_TIM_SET_COUNTER(&htim6, 0);

    while (HAL_GPIO_ReadPin(
        dht_data_GPIO_Port,
        dht_data_Pin) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim6) > 100)
        {
            return false;
        }
    }

    return true;
}

bool DHT_CheckHighLow(uint32_t value){

	if(value >= 60 && value <= 100){
		return true;
	} else {
		return false;
	}
}

bool DHT_Checksum(uint8_t *data)
{
    uint8_t checksum = 0;

    for (int i = 0; i < 4; i++)
    {
        checksum = checksum + data[i];
    }

    if (checksum == data[4])
    {
        return true;
    }

    return false;
}

void DHT_Transaction(){

	// Set DHT11 as output
	DHT_SetOutput();
	//Write low
	HAL_GPIO_WritePin(dht_data_GPIO_Port, dht_data_Pin, GPIO_PIN_RESET);
	// Wait 18 milliseconds
	HAL_Delay(18);
	// Configure pc10 as input
	DHT_SetInput();
	// Read response if initialization is ok
	uint8_t data[5];

	if (!DHT_CheckInit())
	{
	    LCD_Clear();
	    LCD_WriteString("DHT ERROR");
	    return;
	}
	DHT_ReadData(data);

	LCD_Clear();
	LCD_WriteString("DHT OK");

	/*
	char text[17];

	for (int i = 0; i < 40; i++){

		snprintf(text, sizeof(text), "%lu", (unsigned long)data_ms[i]);

		LCD_Clear();
		LCD_WriteString(text);
		HAL_Delay(500);
		LCD_WriteString(" new");
		HAL_Delay(500);
	}
	*/

	if (DHT_Checksum(data)){
		// Start sending data to LCD?
		char temp[17];
		char humid[17];
		LCD_Clear();
		snprintf(temp, sizeof(temp), "Temp: %d.%d", data[2], data[3]);
		snprintf(humid, sizeof(humid), "Humid: %d.%d", data[0], data[1]);

		LCD_WriteString(temp);
		LCD_SecondLine();
		LCD_WriteString(humid);

	}

}
