#include "plugins/SensorsPlugin.h"
#include <WiFi.h>
#include <cmath>

// ==========================================
// 1. SENSORS OVERVIEW (4-Quadrant Dashboard)
// ==========================================
const char *SensorsOverviewPlugin::getName() const
{
  return "Sensors Overview";
}

void SensorsOverviewPlugin::setup()
{
  lastUpdate = millis();
  animFrame = 0;
  Screen.clear();
}

void SensorsOverviewPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 150)
    return;
  lastUpdate = now;
  animFrame = (animFrame + 1) % 4;

  Screen.clear();

  // Subtle cross grid divider (Row 7, Col 7)
  for (int i = 0; i < 16; i++)
  {
    Screen.setPixel(7, i, 1, 30);
    Screen.setPixel(i, 7, 1, 30);
  }

  // --- Q1: CHIP TEMPERATURE (Top-Left: Cols 0..6, Rows 0..6) ---
  int tempC = (int)roundf(temperatureRead());
  // Mini thermometer tube at Col 0 (Rows 1..5)
  Screen.setPixel(0, 1, 1, 140);
  Screen.setPixel(0, 2, 1, 140);
  Screen.setPixel(0, 3, 1, 200);
  Screen.setPixel(0, 4, 1, 240);
  Screen.setPixel(0, 5, 1, 255); // bulb
  Screen.setPixel(1, 5, 1, 255);

  // 2-digit temperature in 3x5 font at Cols 2..6, Rows 1..5
  std::vector<int> tDigits;
  tDigits.push_back((tempC / 10) % 10);
  tDigits.push_back(tempC % 10);
  Screen.drawMiniDigit(2, 1, tDigits[0], 255);
  Screen.drawMiniDigit(5, 1, tDigits[1], 255);

  // --- Q2: WIFI SIGNAL (Top-Right: Cols 8..15, Rows 0..6) ---
  int rssi = WiFi.RSSI();
  // 4 signal ladder bars at Cols 9, 11, 13, 15
  int bars = (rssi > -50) ? 4 : (rssi > -65 ? 3 : (rssi > -80 ? 2 : 1));
  const int q2Cols[4] = {9, 11, 13, 15};
  for (int b = 0; b < 4; b++)
  {
    int col = q2Cols[b];
    int height = b + 2; // heights: 2, 3, 4, 5
    for (int h = 0; h < height; h++)
    {
      int y = 5 - h;
      uint8_t bright = (b < bars) ? 240 : 40;
      Screen.setPixel(col, y, 1, bright);
    }
  }

  // --- Q3: HEAP / RAM MEMORY (Bottom-Left: Cols 0..6, Rows 8..15) ---
  // 'M' tag (Memory) at Col 1..3, Row 9..11
  Screen.setPixel(1, 9, 1, 180);
  Screen.setPixel(3, 9, 1, 180);
  Screen.setPixel(1, 10, 1, 180);
  Screen.setPixel(2, 10, 1, 180);
  Screen.setPixel(3, 10, 1, 180);
  Screen.setPixel(1, 11, 1, 180);
  Screen.setPixel(3, 11, 1, 180);

  // Free RAM horizontal gauge (Row 13 & 14)
  uint32_t freeHeap = ESP.getFreeHeap(); // typically ~190,000 bytes
  int heapBars = (freeHeap * 6) / 320000;
  for (int x = 0; x < 6; x++)
  {
    uint8_t bright = (x < heapBars) ? 240 : 40;
    Screen.setPixel(x, 14, 1, bright);
  }

  // --- Q4: HALL SENSOR (Bottom-Right: Cols 8..15, Rows 8..15) ---
  int hall = hallRead();
  // Magnet symbol at Cols 9..14, Rows 9..13
  Screen.setPixel(10, 9, 1, 240);  // N pole
  Screen.setPixel(14, 9, 1, 240);  // S pole
  Screen.setPixel(10, 10, 1, 200);
  Screen.setPixel(14, 10, 1, 200);
  Screen.setPixel(10, 11, 1, 200);
  Screen.setPixel(14, 11, 1, 200);
  Screen.setPixel(11, 12, 1, 200);
  Screen.setPixel(12, 12, 1, 200);
  Screen.setPixel(13, 12, 1, 200);

  // Active magnetic field spark in center
  int diff = abs(hall - 130);
  uint8_t hallBright = constrain(60 + diff * 3, 60, 255);
  Screen.setPixel(12, 10, 1, (animFrame % 2 == 0) ? hallBright : (hallBright / 2));
}

// ==========================================
// 2. CHIP TEMP PLUGIN (Thermometer + 53°C)
// ==========================================
const char *ChipTempPlugin::getName() const
{
  return "Chip Temp";
}

void ChipTempPlugin::setup()
{
  lastUpdate = millis();
  flamePhase = 0;
  Screen.clear();
}

void ChipTempPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 80)
    return;
  lastUpdate = now;
  flamePhase = (flamePhase + 1) % 4;

  Screen.clear();

  int tempC = (int)roundf(temperatureRead());

  // --- LEFT ZONE: Glass Thermometer (Cols 1..4, Rows 1..14) ---
  // Outer frame
  Screen.setPixel(1, 1, 1, 160);
  Screen.setPixel(2, 1, 1, 160);
  Screen.setPixel(3, 1, 1, 160);

  for (int y = 2; y <= 10; y++)
  {
    Screen.setPixel(1, y, 1, 120); // tube left wall
    Screen.setPixel(3, y, 1, 120); // tube right wall
  }

  // Mercury bulb at bottom (Cols 1..3, Rows 11..14)
  Screen.setPixel(0, 12, 1, 220);
  Screen.setPixel(4, 12, 1, 220);
  Screen.setPixel(0, 13, 1, 220);
  Screen.setPixel(4, 13, 1, 220);

  for (int x = 1; x <= 3; x++)
  {
    Screen.setPixel(x, 11, 1, 255);
    Screen.setPixel(x, 12, 1, 255);
    Screen.setPixel(x, 13, 1, 255);
    Screen.setPixel(x, 14, 1, 220);
  }

  // Mercury column height in tube (Row 10 up to Row 2): 0 to 8 pixels
  // Scale 40°C..70°C -> 0..8 pixels
  int mercH = constrain((tempC - 40) * 8 / 30, 1, 8);
  for (int h = 0; h < mercH; h++)
  {
    int y = 10 - h;
    Screen.setPixel(2, y, 1, 255); // mercury column
  }

  // --- RIGHT ZONE: 3x5 Temperature Value (Cols 6..15, Rows 2..7) ---
  std::vector<int> digits;
  if (tempC >= 100) digits = {1, 0, 0};
  else
  {
    if (tempC >= 10) digits.push_back((tempC / 10) % 10);
    digits.push_back(tempC % 10);
  }

  Screen.drawMiniNumbers(6, 2, digits, 255);

  // Degree symbol '°' at (Col 13, Row 2)
  Screen.setPixel(13, 2, 1, 200);

  // 'C' symbol at Cols 14..15, Rows 3..6
  Screen.setPixel(15, 3, 1, 220);
  Screen.setPixel(14, 4, 1, 220);
  Screen.setPixel(14, 5, 1, 220);
  Screen.setPixel(15, 6, 1, 220);

  // --- RIGHT BOTTOM: Status Flame (Rows 10..14) ---
  int fx = 10;
  Screen.setPixel(fx, 14, 1, 255);
  Screen.setPixel(fx + 1, 14, 1, 255);
  Screen.setPixel(fx + 2, 14, 1, 255);

  Screen.setPixel(fx, 13, 1, 240);
  Screen.setPixel(fx + 1, 13, 1, 255);
  Screen.setPixel(fx + 2, 13, 1, 240);

  // Flickering flame tip
  int tipY = 11 - (flamePhase % 2);
  Screen.setPixel(fx + 1, tipY, 1, 240);
  if (flamePhase >= 2) Screen.setPixel(fx + (flamePhase % 2), 12, 1, 220);
}

// ==========================================
// 3. HALL SENSOR PLUGIN (Interactive Magnet)
// ==========================================
const char *HallSensorPlugin::getName() const
{
  return "Hall Sensor";
}

void HallSensorPlugin::setup()
{
  lastUpdate = millis();
  pulsePhase = 0;
  Screen.clear();
}

void HallSensorPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 60)
    return;
  lastUpdate = now;
  pulsePhase = (pulsePhase + 1) % 6;

  Screen.clear();

  int hall = hallRead();
  int diff = abs(hall - 130); // deviation from ambient baseline

  // --- LEFT ZONE: Horseshoe Magnet (Cols 0..5, Rows 2..10) ---
  // North pole tip (Col 0..1, Row 2)
  Screen.setPixel(0, 2, 1, 255);
  Screen.setPixel(1, 2, 1, 255);
  // South pole tip (Col 4..5, Row 2)
  Screen.setPixel(4, 2, 1, 255);
  Screen.setPixel(5, 2, 1, 255);

  // Vertical legs
  for (int y = 3; y <= 6; y++)
  {
    Screen.setPixel(0, y, 1, 200);
    Screen.setPixel(1, y, 1, 200);
    Screen.setPixel(4, y, 1, 200);
    Screen.setPixel(5, y, 1, 200);
  }

  // U-curve base (Rows 7 & 8)
  for (int x = 0; x <= 5; x++)
  {
    Screen.setPixel(x, 7, 1, 220);
  }
  for (int x = 1; x <= 4; x++)
  {
    Screen.setPixel(x, 8, 1, 220);
  }

  // --- INTERACTIVE MAGNETIC FLUX ARCS (Cols 2..3, Rows 1..4) ---
  // If magnetic field is detected, flux lines light up and expand!
  uint8_t fluxBright = constrain(80 + diff * 4, 80, 255);
  Screen.setPixel(2, 2, 1, fluxBright);
  Screen.setPixel(3, 2, 1, fluxBright);

  if (diff > 15)
  {
    // Outer arc
    Screen.setPixel(2, 1, 1, fluxBright / 2);
    Screen.setPixel(3, 1, 1, fluxBright / 2);
    Screen.setPixel(1, 1, 1, fluxBright / 3);
    Screen.setPixel(4, 1, 1, fluxBright / 3);
  }

  // --- RIGHT ZONE: Value in 3x5 Digits (Cols 7..15, Rows 2..7) ---
  std::vector<int> digits;
  int temp = abs(hall);
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

  // "GAUSS" or Flux Intensity Bar at Rows 11..13 (Cols 7..15)
  // 8-segment signal bar responding dynamically to magnet
  int activeBars = constrain(diff / 12, 1, 8);
  for (int b = 0; b < 8; b++)
  {
    int col = 7 + b;
    uint8_t bBright = (b < activeBars) ? 255 : 40;
    Screen.setPixel(col, 12, 1, bBright);
    Screen.setPixel(col, 13, 1, bBright);
  }
}

// ==========================================
// 4. WIFI SIGNAL PLUGIN (Tower + -dBm + Bars)
// ==========================================
const char *WifiSignalPlugin::getName() const
{
  return "WiFi Signal";
}

void WifiSignalPlugin::setup()
{
  lastUpdate = millis();
  waveStep = 0;
  Screen.clear();
}

void WifiSignalPlugin::loop()
{
  unsigned long now = millis();
  if (now - lastUpdate < 100)
    return;
  lastUpdate = now;
  waveStep = (waveStep + 1) % 4;

  Screen.clear();

  int rssi = WiFi.RSSI();

  // --- LEFT ZONE: Transmission Tower & Radio Waves (Cols 0..5, Rows 1..14) ---
  int tx = 2;
  // Tower Tip (Row 4)
  Screen.setPixel(tx, 4, 1, 255);
  Screen.setPixel(tx + 1, 4, 1, 255);

  // Tower Mast (Row 5..13)
  for (int y = 5; y <= 13; y++)
  {
    Screen.setPixel(tx, y, 1, 160);
    Screen.setPixel(tx + 1, y, 1, 160);
  }
  // Tower Base
  Screen.setPixel(0, 14, 1, 200);
  Screen.setPixel(1, 14, 1, 200);
  Screen.setPixel(2, 14, 1, 255);
  Screen.setPixel(3, 14, 1, 255);
  Screen.setPixel(4, 14, 1, 200);
  Screen.setPixel(5, 14, 1, 200);

  // Radiating RF Wave arcs (Rows 1..3)
  uint8_t arcBright = (waveStep % 2 == 0) ? 240 : 100;
  Screen.setPixel(tx, 2, 1, arcBright);
  Screen.setPixel(tx + 1, 2, 1, arcBright);
  Screen.setPixel(tx - 1, 3, 1, arcBright);
  Screen.setPixel(tx + 2, 3, 1, arcBright);

  // --- RIGHT ZONE: RSSI Value (Cols 7..15, Rows 2..7) ---
  int absRssi = abs(rssi);
  std::vector<int> digits;
  if (absRssi >= 10) digits.push_back((absRssi / 10) % 10);
  digits.push_back(absRssi % 10);

  // Minus sign at Col 7, Row 4
  Screen.setPixel(7, 4, 1, 220);
  Screen.setPixel(8, 4, 1, 220);

  // 3x5 digits at Col 10..15, Row 2
  Screen.drawMiniNumbers(10, 2, digits, 255);

  // --- RIGHT BOTTOM: 4 Signal Strength Bars (Cols 8..15, Rows 9..14) ---
  int bars = (rssi > -50) ? 4 : (rssi > -65 ? 3 : (rssi > -80 ? 2 : 1));
  const int barCol[4] = {8, 10, 12, 14};
  const int barH[4] = {2, 3, 4, 5};

  for (int b = 0; b < 4; b++)
  {
    int col = barCol[b];
    int h = barH[b];
    for (int y = 14; y > 14 - h; y--)
    {
      uint8_t bright = (b < bars) ? 255 : 40;
      Screen.setPixel(col, y, 1, bright);
    }
  }
}
