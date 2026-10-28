#include "plugins/MatrixRainPlugin.h"

void MatrixRainPlugin::setup()
{
  Screen.clear();
  for (int x = 0; x < COLS; x++)
  {
    cols[x].y = -random(1, 10);
    cols[x].speed = random(60, 150);
    cols[x].length = random(4, 9);
    cols[x].nextSpawn = millis() + random(10, 400);
  }
  lastUpdate = millis();
}

void MatrixRainPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 50)
  {
    return;
  }
  lastUpdate = now;

  Screen.clear();

  for (int x = 0; x < COLS; x++)
  {
    if (now >= cols[x].nextSpawn)
    {
      cols[x].y++;
      cols[x].nextSpawn = now + cols[x].speed;

      // Reset when trail has completely scrolled off
      if (cols[x].y - cols[x].length > ROWS)
      {
        cols[x].y = 0;
        cols[x].speed = random(50, 130);
        cols[x].length = random(4, 8);
        cols[x].nextSpawn = now + random(50, 500);
      }
    }

    // Draw the stream with fading brightness trail
    int headY = cols[x].y;
    for (int i = 0; i < cols[x].length; i++)
    {
      int currentY = headY - i;
      if (currentY >= 0 && currentY < ROWS)
      {
        uint8_t brightness;
        if (i == 0)
        {
          brightness = 255; // Leading bright glyph
        }
        else if (i == 1)
        {
          brightness = 190;
        }
        else if (i == 2)
        {
          brightness = 130;
        }
        else if (i == 3)
        {
          brightness = 80;
        }
        else
        {
          brightness = 35;
        }
        Screen.setPixel(x, currentY, 1, brightness);
      }
    }
  }
}

const char *MatrixRainPlugin::getName() const
{
  return "Matrix Rain";
}
