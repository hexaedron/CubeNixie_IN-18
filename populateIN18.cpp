#include <Arduino.h>

// Нам надо отобразить 10 битные ИН-18
// в 10 битные посадочные места.
void populateIN18(char* datetime, byte* shiftBytes)
{
  
  //// Обработка 2 байта segBytes
  //// Фиксим ошибку в плате - инвертированные ножки у U7
  //for (uint8_t i = 0; i <= 4; i++)
  //{
  //  bitWrite(shiftBytes[2], i, bitRead(segBytes[2], 7 - i));
  //}
  //shiftBytes[3]  = segBytes[2] << 4;

  switch(datetime[0])
  {
    case '0':
    break;

    case '1':
    break;

    case '2':
    break;

    case '3':
    break;

    case '4':
    break;

    case '5':
    break;

    case '6':
    break;

    case '7':
    break;

    case '8':
    break;

    case '9':
    break;
  }

  switch(datetime[1])
  {
    case '0':
    break;

    case '1':
    break;

    case '2':
    break;

    case '3':
    break;

    case '4':
    break;

    case '5':
    break;

    case '6':
    break;

    case '7':
    break;

    case '8':
    break;

    case '9':
    break;
  }

  switch(datetime[2])
  {
    case '0':
    break;

    case '1':
    break;

    case '2':
    break;

    case '3':
    break;

    case '4':
    break;

    case '5':
    break;

    case '6':
    break;

    case '7':
    break;

    case '8':
    break;

    case '9':
    break;
  }

  switch(datetime[3])
  {
    case '0':
    break;

    case '1':
    break;

    case '2':
    break;

    case '3':
    break;

    case '4':
    break;

    case '5':
    break;

    case '6':
    break;

    case '7':
    break;

    case '8':
    break;

    case '9':
    break;
  }
}