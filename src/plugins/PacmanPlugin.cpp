#include "plugins/PacmanPlugin.h"

// 5x5 Pac-Man sprites facing right
static const uint8_t pacmanOpenR[5] = {
    0b01110,
    0b11111,
    0b11000,
    0b11111,
    0b01110
};

static const uint8_t pacmanClosed[5] = {
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b01110
};

// Facing left
static const uint8_t pacmanOpenL[5] = {
    0b01110,
    0b11111,
    0b00011,
    0b11111,
    0b01110
};

// 5x5 Ghost sprite
static const uint8_t ghostSprite[5] = {
    0b01110,
    0b10101,
    0b11111,
    0b11111,
    0b10101
};

void PacmanPlugin::setup()
{
  Screen.clear();
  pacmanX = -5;
  ghostX = -12;
  direction = 1;
  poweredUp = false;
  mouthFrame = 0;
  for (int i = 0; i < 16; i++)
  {
    dots[i] = (i % 3 == 0); // Food dots
  }
  lastStep = millis();
}

void PacmanPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastStep < 140)
  {
    return;
  }
  lastStep = now;

  Screen.clear();

  // Draw maze lines top and bottom
  for (int x = 0; x < 16; x++)
  {
    Screen.setPixel(x, 2, 1, 50);
    Screen.setPixel(x, 13, 1, 50);
  }

  // Draw food dots
  if (!poweredUp)
  {
    for (int x = 0; x < 16; x++)
    {
      if (dots[x] && (pacmanX < x || direction < 0))
      {
        Screen.setPixel(x, 8, 1, (x == 15) ? 255 : 120); // Big pellet at end
      }
    }
  }

  // Step movement
  pacmanX += direction;
  ghostX += direction;
  mouthFrame = (mouthFrame + 1) % 2;

  // Check pellet eating
  if (direction == 1 && pacmanX >= 0 && pacmanX < 16)
  {
    dots[pacmanX] = false;
  }

  // Check end of track (Right edge)
  if (direction == 1 && pacmanX > 18)
  {
    direction = -1;
    poweredUp = true;
    pacmanX = 20;
    ghostX = 14;
  }
  // Check end of track (Left edge)
  else if (direction == -1 && pacmanX < -15)
  {
    direction = 1;
    poweredUp = false;
    pacmanX = -5;
    ghostX = -12;
    for (int i = 0; i < 16; i++)
    {
      dots[i] = (i % 3 == 0);
    }
  }

  // Draw Ghost
  const uint8_t *ghost = ghostSprite;
  uint8_t ghostBrightness = poweredUp ? ((now / 200) % 2 ? 60 : 150) : 255;
  for (int r = 0; r < 5; r++)
  {
    for (int c = 0; c < 5; c++)
    {
      int drawX = ghostX + c;
      int drawY = 6 + r;
      if (drawX >= 0 && drawX < 16 && ((ghost[r] >> (4 - c)) & 1))
      {
        Screen.setPixel(drawX, drawY, 1, ghostBrightness);
      }
    }
  }

  // Draw Pacman
  const uint8_t *pacman = (mouthFrame == 0) ? pacmanClosed : ((direction == 1) ? pacmanOpenR : pacmanOpenL);
  for (int r = 0; r < 5; r++)
  {
    for (int c = 0; c < 5; c++)
    {
      int drawX = pacmanX + c;
      int drawY = 6 + r;
      if (drawX >= 0 && drawX < 16 && ((pacman[r] >> (4 - c)) & 1))
      {
        Screen.setPixel(drawX, drawY, 1, 255);
      }
    }
  }
}

const char *PacmanPlugin::getName() const
{
  return "Pacman";
}
