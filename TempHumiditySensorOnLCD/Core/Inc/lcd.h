

#ifndef LCD_H
#define LCD_H

void LCD_Init();
void LCD_WriteString(const char *text);
void LCD_SendByte(uint8_t value);
void LCD_PulseEnable(void);
void LCD_Send4Bits(uint8_t value);
void LCD_SecondLine();
void LCD_Clear();

#endif
