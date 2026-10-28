#include "plugins/RainRadarPlugin.h"

int RainRadarPlugin::minutesToRain = 12;
int RainRadarPlugin::rainIntensity = 65;

void RainRadarPlugin::setRain(int minutes, int intensity)
{
  minutesToRain = minutes;
  rainIntensity = constrain(intensity, 0, 100);
}

const char *RainRadarPlugin::getName() const
{
  return "Rain Radar";
}

void RainRadarPlugin::setup()
{
  lastUpdate = millis();
  for (int i = 0; i < 5; i++)
  {
    dropX[i] = (i % 2 == 0) ? 1 : 4;
    dropY[i] = 4 + (i * 2);
  }
  Screen.clear();
}

void RainRadarPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 60)
    return;
  lastUpdate = now;

  Screen.clear();

  // --- LEFT ZONE: Compact Rain Cloud (Cols 0..6, Rows 1..3) ---
  Screen.setPixel(1, 1, 1, 200);
  Screen.setPixel(2, 1, 1, 200);
  Screen.setPixel(4, 1, 1, 200);

  for (int x = 0; x <= 5; x++)
  {
    Screen.setPixel(x, 2, 1, 240);
    Screen.setPixel(x, 3, 1, 240);
  }

  // Animated falling raindrops under cloud (Cols 1 and 4, Rows 4..8)
  static int drop1Y = 4;
  static int drop2Y = 6;
  if ((now / 120) % 2 == 0)
  {
    drop1Y = (drop1Y >= 8) ? 4 : drop1Y + 1;
    drop2Y = (drop2Y >= 8) ? 4 : drop2Y + 1;
  }
  Screen.setPixel(1, drop1Y, 1, 255);
  Screen.setPixel(4, drop2Y, 1, 255);

  // --- RIGHT ZONE: Countdown Value + "min" (Cols 8..15) ---
  if (minutesToRain >= 0)
  {
    std::vector<int> digits;
    if (minutesToRain >= 10)
    {
      digits.push_back((minutesToRain / 10) % 10);
    }
    digits.push_back(minutesToRain % 10);

    // 3x5 digits at (Col 8, Row 1..5)
    int startX = (digits.size() == 1) ? 11 : 8;
    Screen.drawMiniNumbers(startX, 1, digits, 255);

    // "min" label at Row 7 (Cols 9..14)
    // 'm'
    Screen.setPixel(9, 7, 1, 180);
    Screen.setPixel(10, 7, 1, 180);
    Screen.setPixel(11, 7, 1, 180);
    Screen.setPixel(9, 8, 1, 180);
    Screen.setPixel(11, 8, 1, 180);
    // 'i'
    Screen.setPixel(13, 7, 1, 180);
    Screen.setPixel(13, 8, 1, 180);
  }
  else
  {
    // Trocken: Sonnen-Symbol (Cols 9..14, Rows 2..7)
    Screen.setPixel(11, 3, 1, 255);
    Screen.setPixel(12, 3, 1, 255);
    Screen.setPixel(11, 4, 1, 255);
    Screen.setPixel(12, 4, 1, 255);
    // Strahlen
    Screen.setPixel(11, 1, 1, 160);
    Screen.setPixel(12, 1, 1, 160);
    Screen.setPixel(11, 6, 1, 160);
    Screen.setPixel(12, 6, 1, 160);
    Screen.setPixel(9, 3, 1, 160);
    Screen.setPixel(9, 4, 1, 160);
    Screen.setPixel(14, 3, 1, 160);
    Screen.setPixel(14, 4, 1, 160);
  }

  // --- BOTTOM: 60-Minute Timeline (Rows 14 & 15) ---
  int rainStartCol = (minutesToRain >= 0 && minutesToRain <= 60) ? (minutesToRain * 16) / 60 : -1;

  for (int x = 0; x < 16; x++)
  {
    Screen.setPixel(x, 15, 1, 40); // base track
    if (rainStartCol >= 0 && x >= rainStartCol)
    {
      Screen.setPixel(x, 14, 1, 255);
      Screen.setPixel(x, 15, 1, 180);
    }
  }
}
