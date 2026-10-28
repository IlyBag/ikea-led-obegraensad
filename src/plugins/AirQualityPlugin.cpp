#include "plugins/AirQualityPlugin.h"
#include <cmath>

int AirQualityPlugin::aqi = 28;

void AirQualityPlugin::setAQI(int value)
{
  aqi = constrain(value, 0, 500);
}

const char *AirQualityPlugin::getName() const
{
  return "Air Quality";
}

void AirQualityPlugin::setup()
{
  breathPhase = 0.0f;
  lastUpdate = millis();
  Screen.clear();
}

void AirQualityPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 50)
    return;
  lastUpdate = now;

  breathPhase += 0.08f;
  if (breathPhase > 6.28318f)
    breathPhase -= 6.28318f;

  Screen.clear();

  // Breathing intensity for the icon
  uint8_t pulse = (uint8_t)(170 + sinf(breathPhase) * 80.0f);

  // --- LEFT ZONE: Air Quality Icon (Cols 0..5, Rows 2..7) ---
  // Cute Cloud silhouette
  Screen.setPixel(1, 2, 1, pulse);
  Screen.setPixel(2, 2, 1, pulse);
  Screen.setPixel(3, 1, 1, pulse);
  Screen.setPixel(4, 2, 1, pulse);

  for (int x = 0; x <= 5; x++)
  {
    Screen.setPixel(x, 3, 1, pulse);
    Screen.setPixel(x, 4, 1, pulse);
  }
  for (int x = 1; x <= 4; x++)
  {
    Screen.setPixel(x, 5, 1, pulse);
  }

  // Inside indicator: if bad air (>100), show blinking dot
  if (aqi > 100 && (now / 250) % 2 == 0)
  {
    Screen.setPixel(2, 3, 0, 0);
    Screen.setPixel(3, 3, 0, 0);
  }

  // --- RIGHT ZONE: AQI 3x5 Digits (Cols 7..15, Rows 2..6) ---
  std::vector<int> digits;
  int temp = aqi;
  if (temp <= 0) digits.push_back(0);
  else
  {
    while (temp > 0)
    {
      digits.insert(digits.begin(), temp % 10);
      temp /= 10;
    }
  }
  if (digits.size() > 3) digits.resize(3);

  int startX = 16 - (digits.size() * 4 - 1);
  Screen.drawMiniNumbers(startX, 2, digits, 255);

  // "AQI" tiny tag under the digits at Row 8
  Screen.setPixel(9, 8, 1, 160);
  Screen.setPixel(10, 8, 1, 160);
  Screen.setPixel(12, 8, 1, 160);
  Screen.setPixel(14, 8, 1, 160);

  // --- BOTTOM: 5-Level Air Quality Indicator (Row 13..14) ---
  // Level: 1 (AQI<=50), 2 (51-100), 3 (101-150), 4 (151-200), 5 (>200)
  int level = (aqi <= 50) ? 1 : (aqi <= 100 ? 2 : (aqi <= 150 ? 3 : (aqi <= 200 ? 4 : 5)));

  const int barCols[5] = {1, 4, 7, 10, 13};
  for (int i = 0; i < 5; i++)
  {
    int x = barCols[i];
    if (i < level)
    {
      // Active block
      Screen.setPixel(x, 13, 1, 240);
      Screen.setPixel(x + 1, 13, 1, 240);
      Screen.setPixel(x, 14, 1, 240);
      Screen.setPixel(x + 1, 14, 1, 240);
    }
    else
    {
      // Inactive dim dot
      Screen.setPixel(x, 14, 1, 40);
      Screen.setPixel(x + 1, 14, 1, 40);
    }
  }
}
