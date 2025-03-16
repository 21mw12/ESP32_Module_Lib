#ifndef __ADC_H_
#define __ADC_H_

#include <stdio.h>
#include "hal/adc_types.h"

void ADC_Init(void);

void ADC_Read(uint16_t* data);

// uint16_t ADC_GetValue(adc_channel_t channel);

void ADC_DeInit(void);

#endif
