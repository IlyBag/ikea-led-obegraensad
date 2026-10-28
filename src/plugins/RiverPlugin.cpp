#include "plugins/RiverPlugin.h"
#include <cmath>

int RiverPlugin::levelCm = 142;
int RiverPlugin::trend = 1;

void RiverPlugin::setRiver(int cm, int t)
{
  levelCm = cm;
  trend = t;
}

const char *RiverPlugin::getName() const
{
  return "River";
}

void RiverPlugin::setup()
{
  wavePhase = 0.0f;
  lastUpdate = millis();
  Screen.clear();
}

void RiverPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 45)
    return;
  lastUpdate = now;

  wavePhase += 0.09f;
  if (wavePhase > 6.28318f)
    wavePhase -= 6.28318f;

  Screen.clear();

  // --- 1. TOP HEADER: Trend Arrow + Clean 3x5 Level in cm ---
  // Trend Arrow at (x=0..2, y=2..4)
  if (trend > 0)
  {
    // ▲ rising
    Screen.setPixel(1, 2, 1, 255);
    Screen.setPixel(0, 3, 1, 255);
    Screen.setPixel(1, 3, 1, 255);
    Screen.setPixel(2, 3, 1, 255);
  }
  else if (trend < 0)
  {
    // ▼ falling
    Screen.setPixel(0, 3, 1, 255);
    Screen.setPixel(1, 3, 1, 255);
    Screen.setPixel(2, 3, 1, 255);
    Screen.setPixel(1, 4, 1, 255);
  }
  else
  {
    // — steady
    Screen.setPixel(0, 3, 1, 220);
    Screen.setPixel(1, 3, 1, 220);
    Screen.setPixel(2, 3, 1, 220);
  }

  // Draw 3x5 digits for cm (Col 4..14, Row 1..5)
  std::vector<int> digits;
  int temp = levelCm;
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

  // Position digits cleanly: if 2 digits (e.g. 85), start at x=6; if 3 digits (185), start at x=4
  int startX = (digits.size() == 2) ? 6 : 4;
  Screen.drawMiniNumbers(startX, 1, digits, 255);

  // Subtle "c" "m" dot indicators at (14, 5) and (15, 5)
  Screen.setPixel(15, 4, 1, 140);
  Screen.setPixel(15, 5, 1, 140);

  // Row 6 is an intentional dark gap for contrast

  // --- 2. BOTTOM HALF: Filigree Wave Contours (No blinding solid block!) ---
  int boatY = 9;

  for (int x = 0; x < 16; x++)
  {
    // Wave 1: Upper surface line (Row 9-11)
    float off1 = sinf(wavePhase + x * 0.45f) * 1.2f;
    int y1 = 9 + (int)roundf(off1);
    if (y1 >= 7 && y1 <= 11)
    {
      Screen.setPixel(x, y1, 1, 240);
    }

    if (x == 7)
    {
      boatY = y1 - 1;
    }

    // Wave 2: Lower depth ripple (Row 13-15)
    float off2 = sinf(wavePhase + x * 0.45f + 2.2f) * 1.0f;
    int y2 = 13 + (int)roundf(off2);
    if (y2 >= 12 && y2 <= 15)
    {
      Screen.setPixel(x, y2, 1, 120); // softer secondary wave
    }
  }

  // --- 3. FLOATING BOAT ON WAVE 1 (at x=6..9) ---
  if (boatY >= 5 && boatY <= 12)
  {
    Screen.setPixel(7, boatY - 1, 1, 255); // mast
    Screen.setPixel(6, boatY, 1, 220);     // hull left
    Screen.setPixel(7, boatY, 1, 220);     // hull mid
    Screen.setPixel(8, boatY, 1, 220);     // hull right
    Screen.setPixel(9, boatY, 1, 220);     // hull prow
  }
}
