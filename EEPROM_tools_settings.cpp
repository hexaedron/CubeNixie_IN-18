#include <Arduino.h>
#include <I2C_eeprom.h>
#include "settings.h"

extern I2C_eeprom EEPROM;


uint8_t getDayDotsBrightness(void)
{
    return EEPROM.readByte(DOTS_DEFAULT_BRIGHTNESS_EEPROM_ADDRESS);
}

void setDayDotsBrightness(uint8_t dayBrightness)
{
    EEPROM.updateByte(DOTS_DEFAULT_BRIGHTNESS_EEPROM_ADDRESS, dayBrightness);
}

uint8_t getNightDotsBrightness(void)
{
    return EEPROM.readByte(DOTS_NIGHT_BRIGHTNESS_EEPROM_ADDRESS);
}

void setNightDotsBrightness(uint8_t nightBrightness)
{
    EEPROM.updateByte(DOTS_NIGHT_BRIGHTNESS_EEPROM_ADDRESS, nightBrightness);
}

uint8_t getDayBrightness(void)
{
    return EEPROM.readByte(DEFAULT_BRIGHTNESS_EEPROM_ADDRESS);
}

void setDayBrightness(uint8_t dayBrightness)
{
    EEPROM.updateByte(DEFAULT_BRIGHTNESS_EEPROM_ADDRESS, dayBrightness);
}

uint8_t getNightBrightness(void)
{
    return EEPROM.readByte(NIGHT_BRIGHTNESS_EEPROM_ADDRESS);
}

void setNightBrightness(uint8_t nightBrightness)
{
    EEPROM.updateByte(NIGHT_BRIGHTNESS_EEPROM_ADDRESS, nightBrightness);
}

void setGMTOffset(uint32_t offset)
{
    EEPROM.updateByte(GMT_OFFSET_HOURS_EEPROM_ADDRESS, (uint8_t)(offset / 60 / 60)); 
    EEPROM.updateByte(GMT_OFFSET_MINUTES_EEPROM_ADDRESS, (uint8_t)(offset / 60 % 60));
}

int32_t getGMTOffset(void)
{
    int32_t seconds = (int32_t)EEPROM.readByte(GMT_OFFSET_MINUTES_EEPROM_ADDRESS) * 60;
    if (!(seconds % 15))
    {
      seconds = seconds % 15;
    }
    return (int32_t)EEPROM.readByte(GMT_OFFSET_HOURS_EEPROM_ADDRESS) * 60 * 60 + seconds;
}

void EEPROMValuesInit(bool force = false)
{
  if ((EEPROM.readByte((uint16_t)INIT_ADDR) != INIT_KEY) || force)  // первый запуск или принудительная очистка
  {
    EEPROM.updateByte((uint16_t)INIT_ADDR, INIT_KEY);    // записали ключ
    // записали стандартное значение яркости
    // в данном случае это значение переменной, объявленное выше
    setDayBrightness(DEFAULT_BRIGHTNESS);
    setNightBrightness(NIGHT_BRIGHTNESS);
    setGMTOffset(GMT_SECONDS_OFFSET);
    setDayDotsBrightness(DOTS_DAY_BRIGHTNESS);
    setNightDotsBrightness(DOTS_NIGHT_BRIGHTNESS);
  }
}
