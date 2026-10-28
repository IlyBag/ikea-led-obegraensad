#include "plugins/SpacePlugin.h"
#include <cmath>

int SpacePlugin::minutesToPass = 3;
bool SpacePlugin::isVisible = true;

void SpacePlugin::setISS(int min, bool visible)
{
  minutesToPass = min;
  isVisible = visible;
}

const char *SpacePlugin::getName() const
{
  return "ISS Tracker";
}

void SpacePlugin::setup()
{
  issProgress = -4.0f;
  lastUpdate = millis();
  Screen.clear();
}

void SpacePlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 60)
    return;
  lastUpdate = now;

  issProgress += 0.25f;
  if (issProgress > 22.0f)
  {
    issProgress = -5.0f;
  }

  Screen.clear();

  // --- BACKGROUND STARS ---
  Screen.setPixel(2, 2, 1, 80);
  Screen.setPixel(14, 1, 1, 100);
  Screen.setPixel(1, 10, 1, 60);
  Screen.setPixel(13, 9, 1, 90);
  Screen.setPixel(8, 3, 1, 70);

  // Twinkle two stars
  if ((now / 200) % 2 == 0) Screen.setPixel(11, 4, 1, 200);
  if ((now / 350) % 2 == 0) Screen.setPixel(4, 8, 1, 200);

  // --- EARTH HORIZON (Gentle arc at rows 14-15) ---
  for (int x = 0; x < 16; x++)
  {
    Screen.setPixel(x, 15, 1, 70);
  }
  for (int x = 3; x <= 12; x++)
  {
    Screen.setPixel(x, 14, 1, 40);
  }

  // --- ISS ORBIT PARABOLA: x = issProgress, y = 11 - (6 * sin(issProgress * 0.14)) ---
  int cx = (int)roundf(issProgress);
  float norm = (issProgress - 2.0f) / 12.0f; // 0 to 1
  float arcY = 10.0f - sinf(norm * 3.14159f) * 6.0f;
  int cy = (int)roundf(arcY);

  // Draw ISS Station Sprite (5x3 pixels centered on cx, cy)
  // [-#-O-#-]
  //  Col cx-2: Solar panel left
  //  Col cx-1: Solar truss
  //  Col cx:   Center Habitation module
  //  Col cx+1: Solar truss
  //  Col cx+2: Solar panel right
  if (cx >= -2 && cx < 18 && cy >= 0 && cy < 16)
  {
    // Solar Panel Left (vertical 3px)
    Screen.setPixel(cx - 2, cy - 1, 1, 240);
    Screen.setPixel(cx - 2, cy, 1, 255);
    Screen.setPixel(cx - 2, cy + 1, 1, 240);

    // Truss left & right
    Screen.setPixel(cx - 1, cy, 1, 180);
    Screen.setPixel(cx + 1, cy, 1, 180);

    // Central Core Module
    Screen.setPixel(cx, cy, 1, 255);
    Screen.setPixel(cx, cy - 1, 1, 220);

    // Solar Panel Right (vertical 3px)
    Screen.setPixel(cx + 2, cy - 1, 1, 240);
    Screen.setPixel(cx + 2, cy, 1, 255);
    Screen.setPixel(cx + 2, cy + 1, 1, 240);
  }

  // --- IF PASS IS SOON: DISPLAY COUNTDOWN "3m" (Top left, rows 0-4) ---
  if (minutesToPass > 0 && minutesToPass <= 60)
  {
    std::vector<int> digits;
    if (minutesToPass >= 10) digits.push_back((minutesToPass / 10) % 10);
    digits.push_back(minutesToPass % 10);
    Screen.drawNumbers(0, 0, digits, 160);

    // "m" suffix
    Screen.setPixel(5, 3, 1, 120);
    Screen.setPixel(6, 3, 1, 120);
    Screen.setPixel(5, 4, 1, 120);
    Screen.setPixel(7, 4, 1, 120);
  }
}
