#include "plugins/SpectrumPlugin.h"

void SpectrumPlugin::setup()
{
  Screen.clear();
  for (int x = 0; x < COLS; x++)
  {
    heights[x] = 0;
    targets[x] = random(2, 14);
    peakY[x] = 0.0f;
  }
  lastUpdate = millis();
  lastBeat = millis();
}

void SpectrumPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 40)
  {
    return;
  }
  lastUpdate = now;

  // Simulate musical beat pulses
  bool isBeat = false;
  if (now - lastBeat > 480)
  {
    isBeat = true;
    lastBeat = now;
  }

  Screen.clear();

  for (int x = 0; x < COLS; x++)
  {
    // Generate organic targets
    if (random(10) < 3)
    {
      int maxH = (x < 5) ? 15 : ((x < 11) ? 12 : 9); // Bass heavier on left
      targets[x] = random(1, maxH);
      if (isBeat && x < 4)
      {
        targets[x] = 14;
      }
    }

    // Smooth transition
    if (heights[x] < targets[x])
    {
      heights[x] += 2;
      if (heights[x] > targets[x]) heights[x] = targets[x];
    }
    else if (heights[x] > targets[x])
    {
      heights[x]--;
    }

    // Update floating peaks
    if ((float)heights[x] > peakY[x])
    {
      peakY[x] = (float)heights[x];
    }
    else
    {
      peakY[x] -= 0.35f; // Gravity fall
      if (peakY[x] < 0) peakY[x] = 0;
    }

    // Draw the vertical bar (from bottom up)
    for (int y = 0; y < heights[x]; y++)
    {
      int drawY = 15 - y;
      uint8_t brightness = (y > 11) ? 255 : ((y > 6) ? 180 : 120);
      Screen.setPixel(x, drawY, 1, brightness);
    }

    // Draw peak dot
    int pY = 15 - (int)peakY[x];
    if (pY >= 0 && pY < ROWS)
    {
      Screen.setPixel(x, pY, 1, 255);
    }
  }
}

const char *SpectrumPlugin::getName() const
{
  return "Audio Spectrum";
}
