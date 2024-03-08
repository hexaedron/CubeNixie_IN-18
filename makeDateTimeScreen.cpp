#include <Arduino.h>

void makeDateTimeScreen(char* datetime, uint8_t hr, uint8_t min)
{
  char varchr[3];

  if(hr == 0)
    {
      datetime[0] = ' ';
      datetime[1] = '0';
    }
    else if ((hr > 0) && (hr <= 9))
    {
      datetime[0] = ' ';
      itoa(hr, varchr, 10);
      datetime[1] = varchr[0];
    }
    else
    {
      itoa(hr, varchr, 10);
      datetime[0] = varchr[0];
      datetime[1] = varchr[1];
    }

    if( min == 0 )
    {
      datetime[2] = '0';
      datetime[3] = '0';
    }
    else if ((min > 0) && (min <= 9))
    {
      datetime[2] = '0';
      itoa(min, varchr, 10);
      datetime[3] = varchr[0];
    }
    else
    {
      itoa(min, varchr, 10);
      datetime[2] = varchr[0];
      datetime[3] = varchr[1];
    }
    
    datetime[4] = '\0';
}

