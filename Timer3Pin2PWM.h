#pragma once
#include <Arduino.h>

#define OCR3A_HIGH_VALUE_32_2000 62499
#define OCR3A_LOW_VALUE_32_2000 999
#define TIMER3_PWM_LOW_HZ_32_2000 32      //OCR3A=62499
#define TIMER3_PWM_HIGH_HZ_32_2000 2000   //OCR3A=999

void initTimer3Pin2PWM_32_2000(uint16_t hz, uint8_t dutyPct)
{
  hz = constrain(hz, TIMER3_PWM_LOW_HZ_32_2000, TIMER3_PWM_HIGH_HZ_32_2000);
  uint32_t ocr3a = OCR3A_HIGH_VALUE_32_2000 / (TIMER3_PWM_HIGH_HZ_32_2000 - TIMER3_PWM_LOW_HZ_32_2000) * (OCR3A_HIGH_VALUE_32_2000 + OCR3A_LOW_VALUE_32_2000) / hz + 
                  (OCR3A_HIGH_VALUE_32_2000 / (TIMER3_PWM_HIGH_HZ_32_2000 - TIMER3_PWM_LOW_HZ_32_2000) * (OCR3A_HIGH_VALUE_32_2000 + OCR3A_LOW_VALUE_32_2000) / hz) * 15 / 1000;
  uint32_t ocr3b;

  if(dutyPct == 0)
    ocr3b = 0;
  else
   ocr3b = 15624UL / 100 * dutyPct;
   
  noInterrupts();
    TCCR3A = 1 << COM3B1 | 1 << WGM31 | 1 << WGM30;
    TCCR3B = 1 << WGM33  | 1 << WGM32 | 1 <<  CS31;
    DDRF = 1 << DDF2;
    OCR3A = (uint16_t)ocr3a;
    OCR3B = (uint16_t)ocr3b;
  interrupts();
}

void initTimer3Pin2PWM_1hz(uint8_t dutyPct)
{
  uint16_t ocr3a = 15624;
  uint32_t ocr3b;

  if(dutyPct == 0)
    ocr3b = 0;
  else
   ocr3b = 15624UL / 100 * dutyPct;

  noInterrupts();
    TCCR3A = 1 << COM3B1 | 1 << WGM31 | 1 << WGM30;
    TCCR3B = 1 << WGM33 | 1 << WGM32 | 1 << CS32 | 1 << CS30;
    DDRF = 1 << DDF2;
    OCR3A = (uint16_t)ocr3a;
    OCR3B = (uint16_t)ocr3b;
  interrupts();
}


void setTimer3Pin2PWMDuty(uint8_t dutyPct)
{
  dutyPct = constrain(dutyPct, 1, 100);
  noInterrupts();
    OCR3B = OCR3A / 100 * dutyPct;
  interrupts();
}