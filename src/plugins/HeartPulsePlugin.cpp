#include "plugins/HeartPulsePlugin.h"

// 16x16 Heart shape bitmask (1 = heart pixel)
static const uint16_t heartShape[16] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000111001110000,
    0b0001111111111000,
    0b0011111111111100,
    0b0111111111111110,
    0b0111111111111110,
    0b0111111111111110,
    0b0011111111111100,
    0b0011111111111100,
    0b0001111111111000,
    0b0000111111100000,
    0b0000011111000000,
    0b0000001110000000,
    0b0000000100000000,
    0b0000000000000000
};

// Smaller inner heart for expansion effect
static const uint16_t heartSmall[16] = {
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000011001100000,
    0b0000111111110000,
    0b0000111111110000,
    0b0000111111110000,
    0b0000011111100000,
    0b0000011111100000,
    0b0000001111000000,
    0b0000000110000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000,
    0b0000000000000000
};

void HeartPulsePlugin::setup()
{
  Screen.clear();
  startTime = millis();
  lastFrame = millis();
}

void HeartPulsePlugin::loop()
{
  unsigned long now = millis();
  if (now - lastFrame < 30)
  {
    return;
  }
  lastFrame = now;

  // Heartbeat cycle ~ 900ms (approx 66 BPM)
  // Double beat: 0-150ms (beat 1), 150-250ms (dip), 250-400ms (beat 2), 400-900ms (rest)
  unsigned long cycle = (now - startTime) % 950;
  uint8_t brightness = 40;
  bool useBig = false;

  if (cycle < 160)
  {
    // First beat (Lub) - large heart
    useBig = true;
    brightness = map(cycle, 0, 160, 100, 255);
  }
  else if (cycle < 240)
  {
    // Dip
    useBig = true;
    brightness = map(cycle, 160, 240, 255, 90);
  }
  else if (cycle < 380)
  {
    // Second beat (Dub) - large heart peak
    useBig = true;
    brightness = map(cycle, 240, 380, 110, 240);
  }
  else if (cycle < 550)
  {
    // Fade out to rest
    useBig = false;
    brightness = map(cycle, 380, 550, 200, 40);
  }
  else
  {
    // Rest phase - small heart gently glowing
    useBig = false;
    brightness = 40;
  }

  Screen.clear();
  const uint16_t *shape = useBig ? heartShape : heartSmall;

  for (int y = 0; y < 16; y++)
  {
    uint16_t row = shape[y];
    for (int x = 0; x < 16; x++)
    {
      if ((row >> (15 - x)) & 1)
      {
        Screen.setPixel(x, y, 1, brightness);
      }
    }
  }
}

const char *HeartPulsePlugin::getName() const
{
  return "Heartbeat";
}
