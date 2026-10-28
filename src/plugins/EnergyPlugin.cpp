#include "plugins/EnergyPlugin.h"

int EnergyPlugin::greenPercent = 75;

void EnergyPlugin::setGreen(int percent)
{
  greenPercent = constrain(percent, 0, 100);
}

const char *EnergyPlugin::getName() const
{
  return "Energy Grid";
}

void EnergyPlugin::setup()
{
  lastRotate = millis();
  rotorFrame = 0;
  Screen.clear();
}

void EnergyPlugin::loop()
{
  unsigned long now = millis();

  // Speed of rotation: 100% = 80ms, 10% = 400ms
  int delayMs = 450 - (greenPercent * 370 / 100);
  if (delayMs < 70) delayMs = 70;

  if (now - lastRotate > (unsigned long)delayMs)
  {
    lastRotate = now;
    rotorFrame = (rotorFrame + 1) % 4;
  }

  Screen.clear();

  // --- LEFT ZONE: Slender Windmill Turbine (Cols 0..6, Rows 1..15) ---
  int cx = 3;
  int cy = 4;

  // Mast (Col 3, Rows 5 to 14)
  for (int y = 5; y <= 14; y++)
  {
    Screen.setPixel(cx, y, 1, 140);
  }
  // Ground plate
  Screen.setPixel(cx - 1, 14, 1, 180);
  Screen.setPixel(cx, 14, 1, 240);
  Screen.setPixel(cx + 1, 14, 1, 180);

  // Hub center at (3, 4)
  Screen.setPixel(cx, cy, 1, 255);

  // 3 Rotating Blades (Frame 0..3) with clean spacing
  if (rotorFrame == 0)
  {
    // Blade 1: Up
    Screen.setPixel(cx, cy - 1, 1, 240);
    Screen.setPixel(cx, cy - 2, 1, 240);
    Screen.setPixel(cx, cy - 3, 1, 240);
    // Blade 2: Down-Left
    Screen.setPixel(cx - 1, cy + 1, 1, 220);
    Screen.setPixel(cx - 2, cy + 2, 1, 220);
    // Blade 3: Down-Right
    Screen.setPixel(cx + 1, cy + 1, 1, 220);
    Screen.setPixel(cx + 2, cy + 2, 1, 220);
  }
  else if (rotorFrame == 1)
  {
    // Blade 1: Up-Right
    Screen.setPixel(cx + 1, cy - 1, 1, 240);
    Screen.setPixel(cx + 2, cy - 2, 1, 240);
    // Blade 2: Down
    Screen.setPixel(cx, cy + 1, 1, 220);
    Screen.setPixel(cx, cy + 2, 1, 220);
    // Blade 3: Left-Up
    Screen.setPixel(cx - 1, cy, 1, 220);
    Screen.setPixel(cx - 2, cy - 1, 1, 220);
  }
  else if (rotorFrame == 2)
  {
    // Blade 1: Right
    Screen.setPixel(cx + 1, cy, 1, 240);
    Screen.setPixel(cx + 2, cy, 1, 240);
    Screen.setPixel(cx + 3, cy, 1, 240);
    // Blade 2: Down-Left
    Screen.setPixel(cx - 1, cy + 1, 1, 220);
    Screen.setPixel(cx - 2, cy + 2, 1, 220);
    // Blade 3: Up-Left
    Screen.setPixel(cx - 1, cy - 1, 1, 220);
    Screen.setPixel(cx - 2, cy - 2, 1, 220);
  }
  else
  {
    // Blade 1: Down-Right
    Screen.setPixel(cx + 1, cy + 1, 1, 240);
    Screen.setPixel(cx + 2, cy + 2, 1, 240);
    // Blade 2: Up-Left
    Screen.setPixel(cx - 1, cy - 1, 1, 220);
    Screen.setPixel(cx - 2, cy - 2, 1, 220);
    // Blade 3: Right-Up
    Screen.setPixel(cx + 1, cy - 1, 1, 220);
    Screen.setPixel(cx + 2, cy - 2, 1, 220);
  }

  // --- RIGHT ZONE: Clean 3x5 Percentage (Cols 8..15) ---
  std::vector<int> digits;
  if (greenPercent >= 100)
  {
    digits = {1, 0, 0};
  }
  else
  {
    if (greenPercent >= 10) digits.push_back((greenPercent / 10) % 10);
    digits.push_back(greenPercent % 10);
  }

  // Center digits vertically at Rows 2..6
  int startX = (digits.size() == 3) ? 5 : (digits.size() == 2 ? 8 : 11);
  Screen.drawMiniNumbers(startX, 2, digits, 255);

  // Distinct '%' Symbol at Rows 9..13 (Cols 10..14)
  Screen.setPixel(10, 9, 1, 200);   // top-left circle
  Screen.setPixel(14, 9, 1, 200);   // top-right slash
  Screen.setPixel(13, 10, 1, 200);
  Screen.setPixel(12, 11, 1, 200);  // center slash
  Screen.setPixel(11, 12, 1, 200);
  Screen.setPixel(10, 13, 1, 200);  // bottom-left slash
  Screen.setPixel(14, 13, 1, 200);  // bottom-right circle
}
