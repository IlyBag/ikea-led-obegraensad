#include "plugins/TransitPlugin.h"

String TransitPlugin::line1 = "U1";
int TransitPlugin::min1 = 3;
String TransitPlugin::line2 = "BUS 12";
int TransitPlugin::min2 = 8;

void TransitPlugin::setTransit(const String &l1, int m1, const String &l2, int m2)
{
  line1 = l1;
  min1 = m1;
  if (l2.length() > 0) line2 = l2;
  if (m2 >= 0) min2 = m2;
}

const char *TransitPlugin::getName() const
{
  return "Transit";
}

void TransitPlugin::setup()
{
  lastToggle = millis();
  lastAnim = millis();
  currentView = 0;
  animationFrame = 0;
  Screen.clear();
}

void TransitPlugin::loop()
{
  unsigned long now = millis();

  if (now - lastAnim > 250)
  {
    lastAnim = now;
    animationFrame = (animationFrame + 1) % 4;
  }

  // Toggle between line 1 and line 2 every 4.5 seconds
  if (min2 >= 0 && now - lastToggle > 4500)
  {
    lastToggle = now;
    currentView = (currentView == 0) ? 1 : 0;
  }

  Screen.clear();

  // --- LEFT ZONE: Compact Tram/Bus Icon (Cols 0..5, Rows 2..8) ---
  // Pantograph / Antenna (Row 1)
  Screen.setPixel(2, 1, 1, 160);
  Screen.setPixel(3, 1, 1, 160);

  // Roof & Body (Rows 2..6)
  for (int x = 0; x <= 5; x++)
  {
    Screen.setPixel(x, 2, 1, 220); // roof
    Screen.setPixel(x, 5, 1, 200); // bottom frame
  }
  // Windows (Row 3 & 4)
  Screen.setPixel(0, 3, 1, 180);
  Screen.setPixel(1, 3, 1, 255); // window 1
  Screen.setPixel(2, 3, 1, 180);
  Screen.setPixel(3, 3, 1, 255); // window 2
  Screen.setPixel(4, 3, 1, 255); // window 2
  Screen.setPixel(5, 3, 1, 180);

  // Blinking headlights (Row 5)
  uint8_t headlight = (animationFrame % 2 == 0) ? 255 : 100;
  Screen.setPixel(0, 5, 1, headlight);
  Screen.setPixel(5, 5, 1, headlight);

  // Wheels (Row 6)
  Screen.setPixel(1, 6, 1, 180);
  Screen.setPixel(4, 6, 1, 180);

  // Moving ground track dots (Row 8..14)
  for (int y = 8; y <= 14; y += 2)
  {
    Screen.setPixel(2, y, 1, ((y + animationFrame) % 4 == 0) ? 140 : 40);
  }

  // --- RIGHT ZONE: Line Name (Top) & Departure Min (Bottom) ---
  int activeMin = (currentView == 0) ? min1 : min2;
  String activeLine = (currentView == 0) ? line1 : line2;

  // 1. Line Badge (Cols 7..15, Rows 1..6)
  // If line starts with "U", draw 'U' at Col 7..9
  if (activeLine.startsWith("U") || activeLine.startsWith("u"))
  {
    // 'U' (3x5 at Col 7..9, Row 1..5)
    Screen.setPixel(7, 1, 1, 240);
    Screen.setPixel(9, 1, 1, 240);
    Screen.setPixel(7, 2, 1, 240);
    Screen.setPixel(9, 2, 1, 240);
    Screen.setPixel(7, 3, 1, 240);
    Screen.setPixel(9, 3, 1, 240);
    Screen.setPixel(7, 4, 1, 240);
    Screen.setPixel(9, 4, 1, 240);
    Screen.setPixel(8, 5, 1, 240);

    // Number after 'U' (e.g. 1 in U1) at Col 11..13, Row 1..5
    int lineNum = 1;
    for (size_t c = 1; c < activeLine.length(); c++)
    {
      if (isdigit(activeLine[c]))
      {
        lineNum = activeLine[c] - '0';
        break;
      }
    }
    Screen.drawMiniDigit(11, 1, lineNum, 255);
  }
  else
  {
    // BUS or generic badge: draw "B" at Col 7..9, number at Col 11..13
    // 'B' (3x5)
    Screen.setPixel(7, 1, 1, 240);
    Screen.setPixel(8, 1, 1, 240);
    Screen.setPixel(7, 2, 1, 240);
    Screen.setPixel(9, 2, 1, 240);
    Screen.setPixel(7, 3, 1, 240);
    Screen.setPixel(8, 3, 1, 240);
    Screen.setPixel(7, 4, 1, 240);
    Screen.setPixel(9, 4, 1, 240);
    Screen.setPixel(7, 5, 1, 240);
    Screen.setPixel(8, 5, 1, 240);

    // First digit of bus line
    int lineNum = 2;
    for (size_t c = 0; c < activeLine.length(); c++)
    {
      if (isdigit(activeLine[c]))
      {
        lineNum = activeLine[c] - '0';
        break;
      }
    }
    Screen.drawMiniDigit(11, 1, lineNum, 255);
  }

  // Row 7 is a dark gap for contrast

  // 2. Countdown Minutes (Cols 7..15, Rows 9..14)
  bool blinkMin = (activeMin > 1) || ((millis() / 350) % 2 == 0);
  if (activeMin >= 0 && blinkMin)
  {
    std::vector<int> digits;
    if (activeMin >= 10) digits.push_back((activeMin / 10) % 10);
    digits.push_back(activeMin % 10);

    int startX = (digits.size() == 2) ? 7 : 8;
    Screen.drawMiniNumbers(startX, 9, digits, 255);

    // "m" tag at Col 13..15, Row 11..13
    Screen.setPixel(13, 11, 1, 180);
    Screen.setPixel(14, 11, 1, 180);
    Screen.setPixel(15, 11, 1, 180);
    Screen.setPixel(13, 12, 1, 180);
    Screen.setPixel(15, 12, 1, 180);
    Screen.setPixel(13, 13, 1, 180);
    Screen.setPixel(15, 13, 1, 180);
  }

  delay(35);
}
