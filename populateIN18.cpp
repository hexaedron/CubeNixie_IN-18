#include <Arduino.h>

// Нам надо отобразить 10 битные ИН-18
// в 10 битные посадочные места.
void populateIN18(char* datetime, byte* shiftBytes)
{
  shiftBytes[0] = 0;
  shiftBytes[1] = 0;
  shiftBytes[2] = 0;
  shiftBytes[3] = 0;
  shiftBytes[4] = 0;

  //// Обработка 2 байта segBytes
  //// Фиксим ошибку в плате - инвертированные ножки у U7
  //for (uint8_t i = 0; i <= 4; i++)
  //{
  //  bitWrite(shiftBytes[2], i, bitRead(segBytes[2], 7 - i));
  //}
  //shiftBytes[3]  = segBytes[2] << 4;

  switch(datetime[0])
  {
    case '7':
      bitSet(shiftBytes[0], 0);
    break;

    case '6':
      bitSet(shiftBytes[0], 1);
    break;

    case '5':
      bitSet(shiftBytes[0], 2);
    break;

    case '4':
      bitSet(shiftBytes[0], 3);
    break;

    case '3':
      bitSet(shiftBytes[0], 4);
    break;

    case '2':
      bitSet(shiftBytes[0], 5);
    break;

    case '1':
      bitSet(shiftBytes[0], 6);
    break;

    case '0':
      bitSet(shiftBytes[0], 7);
    break;

    case '8':
      bitSet(shiftBytes[1], 1);
    break;

    case '9':
      bitSet(shiftBytes[1], 0);
    break;
  }

  switch(datetime[1])
  {
    case '0':
      bitSet(shiftBytes[1], 7);
    break;

    case '1':
      bitSet(shiftBytes[1], 6);
    break;

    case '2':
      bitSet(shiftBytes[1], 5);
    break;

    case '3':
      bitSet(shiftBytes[1], 4);
    break;

    case '4':
      bitSet(shiftBytes[1], 3);
    break;

    case '5':
      bitSet(shiftBytes[1], 2);
    break;

    case '6':
      bitSet(shiftBytes[2], 0);
    break;

    case '7':
      bitSet(shiftBytes[2], 1);
    break;

    case '8':
      bitSet(shiftBytes[2], 2);
    break;

    case '9':
      bitSet(shiftBytes[2], 3);
    break;
  }

  switch(datetime[2])
  {
    case '0':
      bitSet(shiftBytes[2], 7);
    break;

    case '1':
      bitSet(shiftBytes[2], 6);
    break;

    case '2':
      bitSet(shiftBytes[2], 5);
    break;

    case '3':
      bitSet(shiftBytes[2], 4);
    break;

    case '4':
      bitSet(shiftBytes[3], 5);
    break;

    case '5':
      bitSet(shiftBytes[3], 4);
    break;

    case '6':
      bitSet(shiftBytes[3], 3);
    break;

    case '7':
      bitSet(shiftBytes[3], 2);
    break;

    case '8':
      bitSet(shiftBytes[3], 1);
    break;

    case '9':
      bitSet(shiftBytes[3], 0);
    break;
  }

  switch(datetime[3])
  {
    case '0':
      bitSet(shiftBytes[3], 7);
    break;

    case '1':
      bitSet(shiftBytes[3], 6);
    break;

    case '2':
      bitSet(shiftBytes[4], 7);
    break;

    case '3':
      bitSet(shiftBytes[4], 6);
    break;

    case '4':
      bitSet(shiftBytes[4], 5);
    break;

    case '5':
      bitSet(shiftBytes[4], 4);
    break;

    case '6':
      bitSet(shiftBytes[4], 3);
    break;

    case '7':
      bitSet(shiftBytes[4], 2);
    break;

    case '8':
      bitSet(shiftBytes[4], 1);
    break;

    case '9':
      bitSet(shiftBytes[4], 0);
    break;
  }
}