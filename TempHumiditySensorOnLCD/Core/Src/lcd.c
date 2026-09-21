
#include "main.h"
#include "lcd.h"
#include <string.h>

void LCD_Init()
{

	// Initialization
	HAL_Delay(50);

	HAL_GPIO_WritePin(lcd_e_GPIO_Port, lcd_e_Pin, GPIO_PIN_RESET);

	LCD_Send4Bits(0x03);
	LCD_PulseEnable();
	HAL_Delay(5);

	LCD_Send4Bits(0x03);
	LCD_PulseEnable();
	HAL_Delay(1);

	LCD_Send4Bits(0x03);
	LCD_PulseEnable();
	HAL_Delay(1);

	LCD_Send4Bits(0x02);
	LCD_PulseEnable();
	HAL_Delay(1);

	// Setup/configuration
	// 4 bit - 2 line display
	LCD_SendByte(0x28);
	// Display off
	LCD_SendByte(0x08);
	// Clear display
	LCD_SendByte(0x01);
	HAL_Delay(2);
	// Cursor moves right
	LCD_SendByte(0x06);
	// Display on
	LCD_SendByte(0x0c);

}

void LCD_WriteString(const char *text) {
	// Iterate over each letter/number
	size_t length = strlen(text);

	HAL_GPIO_WritePin(lcd_rs_GPIO_Port, lcd_rs_Pin, GPIO_PIN_SET);

	for (int i = 0; i < length; i++)
	{
		// send char to LCD_sendbyte
		LCD_SendByte((uint8_t)text[i]);
	}
	//
}

void LCD_SendByte(uint8_t value)
{
	uint8_t upper = value >> 4;
	uint8_t lower = value & 0x0F;

    // Take upper 4 bits and send to LCD_Send4Bits
	LCD_Send4Bits(upper);
    // pulse E
	LCD_PulseEnable();

    // Take lower 4 bits and send to LCD_Send4Bits
	LCD_Send4Bits(lower);
    // pulse E
	LCD_PulseEnable();
}

void LCD_SecondLine()
{
    HAL_GPIO_WritePin(lcd_rs_GPIO_Port, lcd_rs_Pin, GPIO_PIN_RESET);
    LCD_SendByte(0xC0);
}

void LCD_Clear(){
	HAL_GPIO_WritePin(lcd_rs_GPIO_Port, lcd_rs_Pin, GPIO_PIN_RESET);
	LCD_SendByte(0x01);
	HAL_Delay(2);
}

void LCD_PulseEnable(void)
{
    HAL_GPIO_WritePin(lcd_e_GPIO_Port, lcd_e_Pin, GPIO_PIN_SET);

    // Small delay
    HAL_Delay(1);

    HAL_GPIO_WritePin(lcd_e_GPIO_Port, lcd_e_Pin, GPIO_PIN_RESET);

    // Small delay
    HAL_Delay(1);
}

void LCD_Send4Bits(uint8_t value) {
	if (value & 0x01)
		{
			HAL_GPIO_WritePin(lcd_d4_GPIO_Port, lcd_d4_Pin, GPIO_PIN_SET);
		}	else	{
			HAL_GPIO_WritePin(lcd_d4_GPIO_Port, lcd_d4_Pin, GPIO_PIN_RESET);
		}

	if (value & 0x02)
		{
			HAL_GPIO_WritePin(lcd_d5_GPIO_Port, lcd_d5_Pin, GPIO_PIN_SET);
		}	else	{
			HAL_GPIO_WritePin(lcd_d5_GPIO_Port, lcd_d5_Pin, GPIO_PIN_RESET);
		}


	if (value & 0x04)
		{
			HAL_GPIO_WritePin(lcd_d6_GPIO_Port, lcd_d6_Pin, GPIO_PIN_SET);
		}	else	{
			HAL_GPIO_WritePin(lcd_d6_GPIO_Port, lcd_d6_Pin, GPIO_PIN_RESET);
		}

	if (value & 0x08)
		{
			HAL_GPIO_WritePin(lcd_d7_GPIO_Port, lcd_d7_Pin, GPIO_PIN_SET);
		}	else	{
			HAL_GPIO_WritePin(lcd_d7_GPIO_Port, lcd_d7_Pin, GPIO_PIN_RESET);
		}
}
