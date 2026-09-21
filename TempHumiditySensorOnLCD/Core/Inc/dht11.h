

#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>
#include <stdbool.h>

void DHT_SetOutput();
void DHT_SetInput();
void DHT_ReadData(uint8_t *data);
uint8_t DHT_ReadBit(uint8_t x);
void DHT_Transaction();
bool DHT_CheckInit();
bool DHT_CheckHighLow(uint32_t value);
bool DHT_Checksum(uint8_t *data);

#endif
