#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class TransitPlugin : public Plugin
{
private:
  unsigned long lastToggle;
  int currentView; // 0 = first line, 1 = second line
  int animationFrame;
  unsigned long lastAnim;

public:
  static String line1;
  static int min1;
  static String line2;
  static int min2;

  static void setTransit(const String &l1, int m1, const String &l2 = "", int m2 = -1);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
