#include "settings.h"
#include "Timer3Pin2PWM.h"
#include <WDT.h>
#include <I2C_eeprom.h>
#include <TinyGPSPlus.h>
#include "swRTC2000.h"
#include <RtcUtility.h>
#include <RtcDateTime.h>
#include <PostNeoSWSerial.h>

#ifdef FAST_SHIFT_OUT
  #include <FastShiftOut.h>
  FastShiftOut FSO(DATA, CLOCK, LSBFIRST);
  #define shiftOut(DATA_PIN, CLOCK, ORDER, VALUE) FSO.write(VALUE)
#endif

#include "MoscowSetRise.h"
#include "simpleTimer.h"

// Устройства
swRTC2000 rtc; 
I2C_eeprom EEPROM(0b1010000, I2C_DEVICESIZE_24LC02); //Все адресные ножки 24LC02 подключаем к земле, это даёт нам адрес 0b1010000 или 0x50
PostNeoSWSerial GPS_SoftSerial(RX_PIN, TX_PIN);
TinyGPSPlus   ATGM332D;
TinyGPSCustom ATGM332D_year(ATGM332D,  "GNZDA", 4);
TinyGPSCustom ATGM332D_month(ATGM332D, "GNZDA", 3);
TinyGPSCustom ATGM332D_day(ATGM332D,   "GNZDA", 2);

// Буферы
char datetime[] = "0000";
brightness Brightness = {50, 50};

void setup() 
{
  // Сразу поставим небольшую яркость, чтобы выжечь глаза ночью
  initTimer3Pin2PWM_32_2000(95, 75);
  wdt_enable(WTO_1S); // Ставим вотчдог.

  #ifdef INFO_ENABLE
    Serial.begin(115200);
    wdt_reset();
    INFO("Starting");
  #endif

  INFO("EEPROM");
    wdt_reset();
    EEPROMValuesInit();
  INFO("EEPROM ok!");

  pinMode(DATA,    OUTPUT);
  pinMode(LATCH,   OUTPUT);
  pinMode(CLOCK,   OUTPUT);
  pinMode(SW_DOTS, OUTPUT);
  pinMode(TX_PIN,  OUTPUT);
  pinMode(RX_PIN,   INPUT);

  wdt_reset();

  INFO("Start GPS");
  GPS_SoftSerial.begin(SOFT_GPS_BAUD_RATE);
  delay(1000);

  INFO("Begin GPS Fix");
  while(GPS_SoftSerial.available() > 0)
  {
      wdt_reset();
      ATGM332D.encode(GPS_SoftSerial.read());
  }

  INFO("First RTC setting...");
  while(!(GPS_TIME_IS_VALID() && atoi(ATGM332D_year.value()) >= 2023))
  {
    wdt_reset();
    while(GPS_SoftSerial.available() > 0)
    {
        wdt_reset();
        ATGM332D.encode(GPS_SoftSerial.read());
    }
  }
  adjustTime(getGMTOffset());
  INFO("Done!");

  wdt_reset();
  calculateBrightness();
  setTimer3Pin2PWMDuty(Brightness.screen);
  wdt_reset();

}

void loop() 
{
  uint8_t hour = rtc.getHours();
  uint8_t minute = rtc.getMinutes();
  uint8_t second = rtc.getSeconds();
  bool minRefreshFlag = true;
  bool dotRefreshFlag = true;
  Timer16 clockTimer(500);
  
  INFO("Start main cycle!");
  for(;;)
  {
    wdt_reset();
    
    if(clockTimer.ready())
    {
      hour   = rtc.getHours();
      minute = rtc.getMinutes();
      second = rtc.getSeconds();
    }
    
    // Время с датчика надо брать постоянно, чтобы не переполнился буфер
    while(GPS_SoftSerial.available() > 0)
      ATGM332D.encode(GPS_SoftSerial.read());

    if(((second % 2) && dotRefreshFlag))
    {
      analogWrite(SW_DOTS, Brightness.dots);

      makeDateTimeScreen(datetime, hour, minute);
      print_IN_18();
      dotRefreshFlag = !dotRefreshFlag;
    }
    else if((!(second % 2)) && !dotRefreshFlag)
    {
      analogWrite(SW_DOTS, DOTS_OFF);

      makeDateTimeScreen(datetime, hour, minute);
      print_IN_18();
      dotRefreshFlag = !dotRefreshFlag;

      DEBUG("Timestamp2000     = ", rtc.getTimestamp2000());
      DEBUG("Timestamp         = ", rtc.getTimestamp());
      DEBUG("Screen Brightness = ", Brightness.screen);
      DEBUG("Dots Brightness   = ", Brightness.dots);
    }

    // Каждые 5 мин фиксим время и локацию, на всякий
    // Сюда же воткнём установку яркости, супер часто это делать смысла немного
    // И пересчёт восхода/заката сюда же, раз в час, ЧЧ:05
    if(minute % 5 ) 
    {
      if(minRefreshFlag)
      {
          if(ATGM332D.time.isValid())
          {
            adjustTime(getGMTOffset());
          }

          calculateBrightness();
          setTimer3Pin2PWMDuty(Brightness.screen);

          minRefreshFlag = false;
      }
    }
    else
      minRefreshFlag = true;
  }
}

void adjustTime(uint32_t GMTSecondsOffset)
{
  wdt_reset();
  RtcDateTime dt
  (
    atoi(ATGM332D_year.value()), 
    atoi(ATGM332D_month.value()), 
    atoi(ATGM332D_day.value()), 
    ATGM332D.time.hour(), 
    ATGM332D.time.minute(),
    ATGM332D.time.second() 
  );
  dt += GMTSecondsOffset;
  rtc.stopRTC();
    rtc.setDate(dt.Day(), dt.Month(), dt.Year());
    rtc.setTime(dt.Hour(), dt.Minute(), dt.Second());
  rtc.startRTC();
}

// вычисляет яркость точек и самого экрана
void calculateBrightness()
{
  RtcDateTime timeNow(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  uint32_t    timeSet  = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + 
                          (uint32_t)getMoscowSunset(timeNow.Month(), timeNow.Day()) * 60 + getGMTOffset(); 
  uint32_t    timeRise = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + 
                          (uint32_t)getMoscowSunrise(timeNow.Month(), timeNow.Day()) * 60 + getGMTOffset(); 
  
  wdt_reset();

  if((timeNow.TotalSeconds() > timeRise) && (timeNow.TotalSeconds() < timeSet)) // День, поскольку мы между закатом и рассветом
  {
    DEBUG("calculateBrightness(): ", "It's a day!");
    Brightness.screen = getDayBrightness();
    Brightness.dots   = getDayDotsBrightness();
  }
  else
  {
    DEBUG("calculateBrightness(): ", "It's a night!");
    Brightness.screen = getNightBrightness();
    Brightness.dots   = getNightDotsBrightness();
  }
}

// Подготавливает и заполняет ИН-18
void print_IN_18()
{
  wdt_reset();

  byte shiftBytes[5] = {'\0'};
  populateIN18(datetime, shiftBytes);

  digitalWrite(LATCH, LOW);
    for (int8_t i = 4; i >= 0; i--)
    {
      shiftOut(DATA, CLOCK, LSBFIRST, shiftBytes[i]);
    }
  digitalWrite(LATCH, HIGH);
}
