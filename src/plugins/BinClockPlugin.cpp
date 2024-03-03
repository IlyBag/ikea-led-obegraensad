#include "plugins/LinesPlugin.h"

void BinClockPlugin::setup()
{
}

void BinClockPlugin::loop()
{
    if (getLocalTime(&timeinfo))
    {
        if (previousHour != timeinfo.tm_hour || previousMinutes != timeinfo.tm_min || previousSeconds != timeinfo.tm_sec)
        {   //012345679
            // hh mm ss 6
            // hh mm ss 7
            // hh mm ss 8
            // hh mm ss 9
            for (int b = 0; b < 4; b++)
                Screen.setPixel(1, 9 - b, (((previousHour / 10) >> b) % 2));
            for (int b = 0; b < 4; b++)
                Screen.setPixel(2, 9 - b, (((previousHour) >> b) % 2));

            for (int b = 0; b < 4; b++)
                Screen.setPixel(4, 9 - b, (((previousMinutes / 10) >> b) % 2));
            for (int b = 0; b < 4; b++)
                Screen.setPixel(5, 9 - b, (((previousMinutes) >> b) % 2));

            for (int b = 0; b < 4; b++)
                Screen.setPixel(6, 9 - b, (((previousSeconds / 10) >> b) % 2));
            for (int b = 0; b < 4; b++)
                Screen.setPixel(7, 9 - b, (((previousSeconds) >> b) % 2));
        }

        previousMinutes = timeinfo.tm_min;
        previousHour = timeinfo.tm_hour;
        previousSeconds = timeinfo.tm_sec;
    }
}

const char * BinClockPlugin::getName() const
{
  return "BinClock";
}