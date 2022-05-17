#ifndef _HAL_NVIC_H_
#define _HAL_NVIC_H_

#include <stdint.h>

void NVIC_EnableIRQ_Custom(uint8_t irq, uint8_t priority);
void NVIC_DisableIRQ(uint8_t irq);
#endif
