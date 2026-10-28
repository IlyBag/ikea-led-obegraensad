#include "plugins/ClusterPlugin.h"
#include "messages.h"

ClusterNode ClusterPlugin::nodes[5] = {
  {"securegw", 18, true},
  {"plantlix", 25, true},
  {"vps-runner", 45, true},
  {"storage-vps", 12, true},
  {"ha-core", 22, true}
};

int ClusterPlugin::threatCount = 0;
bool ClusterPlugin::hasAlert = false;
String ClusterPlugin::lastAlertMessage = "";

void ClusterPlugin::setNode(int index, int load, bool online)
{
  if (index >= 0 && index < 5)
  {
    nodes[index].load = constrain(load, 0, 100);
    nodes[index].online = online;
  }
}

void ClusterPlugin::setThreats(int count)
{
  threatCount = count;
}

void ClusterPlugin::triggerAlert(const String &alertText)
{
  hasAlert = true;
  lastAlertMessage = alertText;
  Messages.add(alertText.c_str(), 1, 999, 35);
}

const char *ClusterPlugin::getName() const
{
  return "Cluster";
}

void ClusterPlugin::setup()
{
  lastSimUpdate = millis();
  lastBlink = millis();
  blinkState = false;
  animOffset = 0;
  Screen.clear();
}

void ClusterPlugin::loop()
{
  unsigned long now = millis();

  if (now - lastSimUpdate > 1500)
  {
    lastSimUpdate = now;
    for (int i = 0; i < 5; i++)
    {
      if (nodes[i].online)
      {
        int delta = (rand() % 9) - 4;
        nodes[i].load = constrain(nodes[i].load + delta, 5, 95);
      }
    }
  }

  if (now - lastBlink > 400)
  {
    lastBlink = now;
    blinkState = !blinkState;
  }

  Screen.clear();

  // --- HEADER (Rows 1..4): Server Rack (Left) & Shield/Alert (Right) ---
  // Mini Rack (Cols 1..5, Rows 1..4)
  Screen.setPixel(1, 1, 1, 160);
  Screen.setPixel(2, 1, 1, 160);
  Screen.setPixel(3, 1, 1, 160);
  Screen.setPixel(4, 1, 1, 160);
  Screen.setPixel(5, 1, 1, 160);

  Screen.setPixel(1, 2, 1, 120);
  Screen.setPixel(4, 2, 1, blinkState ? 255 : 40); // LED 1
  Screen.setPixel(5, 2, 1, 120);

  Screen.setPixel(1, 3, 1, 120);
  Screen.setPixel(4, 3, 1, !blinkState ? 255 : 40); // LED 2
  Screen.setPixel(5, 3, 1, 120);

  Screen.setPixel(1, 4, 1, 160);
  Screen.setPixel(2, 4, 1, 160);
  Screen.setPixel(3, 4, 1, 160);
  Screen.setPixel(4, 4, 1, 160);
  Screen.setPixel(5, 4, 1, 160);

  // Security Status Icon (Cols 10..14, Rows 1..4)
  if (threatCount > 0 || hasAlert)
  {
    if (blinkState)
    {
      // Warning (!)
      Screen.setPixel(12, 1, 1, 255);
      Screen.setPixel(12, 2, 1, 255);
      Screen.setPixel(12, 4, 1, 255); // dot
    }
  }
  else
  {
    // Steady Shield
    Screen.setPixel(11, 1, 1, 180);
    Screen.setPixel(12, 1, 1, 220);
    Screen.setPixel(13, 1, 1, 180);

    Screen.setPixel(10, 2, 1, 180);
    Screen.setPixel(12, 2, 1, 255); // inner check
    Screen.setPixel(14, 2, 1, 180);

    Screen.setPixel(11, 3, 1, 180);
    Screen.setPixel(13, 3, 1, 180);

    Screen.setPixel(12, 4, 1, 180);
  }

  // Row 5 is a dark separator gap!

  // --- 5 NODE LOAD BARS (Cols 1, 4, 7, 10, 13, Rows 6..14) ---
  const int colX[5] = {1, 4, 7, 10, 13};
  for (int n = 0; n < 5; n++)
  {
    int x = colX[n];

    if (!nodes[n].online)
    {
      // Offline: dark
      Screen.setPixel(x, 14, 1, 30);
      Screen.setPixel(x + 1, 14, 1, 30);
    }
    else
    {
      // Online base LED (Row 14)
      uint8_t statBright = (nodes[n].load > 85 && blinkState) ? 80 : 255;
      Screen.setPixel(x, 14, 1, statBright);
      Screen.setPixel(x + 1, 14, 1, statBright);

      // Load bar: max 6 pixels high (from row 12 going up to row 7)
      int barHeight = (nodes[n].load * 6) / 100;
      for (int h = 0; h < barHeight; h++)
      {
        int y = 12 - h;
        uint8_t barBright = 100 + (h * 24);
        Screen.setPixel(x, y, 1, barBright);
        Screen.setPixel(x + 1, y, 1, barBright);
      }
    }
  }

  delay(40);
}
