#include "settings.h"
#include "Timer3Pin2PWM.h"
#include <WDT.h>
#include <I2C_eeprom.h>
#include "swRTC2000.h"
#include <RtcUtility.h>
#include <RtcDateTime.h>

#define W5500_ETHERNET_SHIELD
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "NTPClient.h"

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
EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Буферы
char datetime[] = "0000";
brightness Brightness = {50, 50};
byte mac[6] = {0x66, 0xAA, (uint8_t &)GUID0, (uint8_t &)GUID1, (uint8_t &) GUID2, (uint8_t &)GUID3}; // MAC-адрес будет формироваться уникальный для каждого чипа

ISR(TIMER3_vect) // При каждом срабатывании таймера обнуляем вотчдог
{
  if (TIFR3 & (1 << OCF3B)) 
  {
      TIFR3 = 1 << OCF3B;
      wdt_reset();
  }
}

void setup() 
{
  // Сразу поставим небольшую яркость, чтобы выжечь глаза ночью
  initTimer3Pin2PWM_32_2000(95, 75);
  wdt_enable(WTO_128MS); // Ставим вотчдог.
  setTimer3Interrupt();  // Запускаем прерывания по таймеру.

  #ifdef INFO_ENABLE
    Serial.begin(115200);
    INFO("Starting");
  #endif

  INFO("EEPROM");
    EEPROMValuesInit();
  INFO("EEPROM ok!");

  pinMode(DATA,    OUTPUT);
  pinMode(LATCH,   OUTPUT);
  pinMode(CLOCK,   OUTPUT);
  pinMode(SW_DOTS, OUTPUT);

  INFO("Start DHCP");
  // Получим IP-адрес из EEPROM и выставим его на клиенте
  IPAddress poolServerIP(getIPAddress());
  DEBUG("IP = ", getIPAddress());
  timeClient.setPoolServerAdddress(poolServerIP);
  DEBUG("poolServerIP = ", poolServerIP);

  print_IN_18();
  while (Ethernet.begin(mac) == 0) 
  {
  }
  INFO("DHCP ok!");

  INFO("Start NTP");

  print_IN_18();
  while(!adjustTime(getGMTOffset()))
  {
  }
  INFO("NTP ok!");

  calculateBrightness();
  setTimer3Pin2PWMDuty(Brightness.screen);
}

void loop() 
{
  uint8_t hour = rtc.getHours();
  uint8_t minute = rtc.getMinutes();
  uint8_t second = rtc.getSeconds();
  bool minRefreshFlag = true;
  bool dotRefreshFlag = true;
  Timer16 clockTimer(500);

  // Это время в минутах, прибавляемое к периоду обновления NTP, 
  // чтобы девайсы не дёргали NTP сервер одновременно.
  uint8_t minAdd = (uint8_t)map((uint32_t &)GUID0, 0, 0xFFFFFFFF, 1, 10);
  
  INFO("Start main cycle!");
  for(;;)
  { 
    if(clockTimer.ready())
    {
      hour   = rtc.getHours();
      minute = rtc.getMinutes();
      second = rtc.getSeconds();
    }

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
    if(minute % (3 + minAdd)) 
    {
      if(minRefreshFlag)
      {
        adjustTime(getGMTOffset());
        minRefreshFlag = false;
        calculateBrightness();
        setTimer3Pin2PWMDuty(Brightness.screen);
      }
    }
    else
    {
      minRefreshFlag = true;
    }
  }
}

bool adjustTime(uint32_t GMTSecondsOffset)
{ 
  if(timeClient.update())
  {
    timeClient.setTimeOffset(GMTSecondsOffset);
    RtcDateTime dt(timeClient.getEpochTime() - UNIX_2000_OFFSET);
    rtc.stopRTC();
      rtc.setDate(dt.Day(), dt.Month(), dt.Year());
      rtc.setTime(dt.Hour(), dt.Minute(), dt.Second());
    rtc.startRTC();
    return true;
  }
  
  return false;
}

// вычисляет яркость точек и самого экрана
void calculateBrightness()
{
  RtcDateTime timeNow(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  uint32_t    timeSet  = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + 
                          (uint32_t)getMoscowSunset(timeNow.Month(), timeNow.Day()) * 60 + getGMTOffset(); 
  uint32_t    timeRise = RtcDateTime(timeNow.Year(), timeNow.Month(), timeNow.Day(), 0, 0 ,0).TotalSeconds() + 
                          (uint32_t)getMoscowSunrise(timeNow.Month(), timeNow.Day()) * 60 + getGMTOffset(); 
  

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
  byte shiftBytes[5] = {'\0'};
  populateIN18(datetime, shiftBytes);

  digitalWrite(LATCH, LOW);
    for (int8_t i = 4; i >= 0; i--)
    {
      shiftOut(DATA, CLOCK, LSBFIRST, shiftBytes[i]);
    }
  digitalWrite(LATCH, HIGH);
}
