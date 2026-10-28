#pragma once

#include "PluginManager.h"
#include <Arduino.h>

struct ClusterNode
{
  const char *name;
  int load;     // 0 - 100%
  bool online;
};

class ClusterPlugin : public Plugin
{
private:
  unsigned long lastSimUpdate;
  unsigned long lastBlink;
  bool blinkState;
  int animOffset;

public:
  static ClusterNode nodes[5];
  static int threatCount;
  static bool hasAlert;
  static String lastAlertMessage;

  static void setNode(int index, int load, bool online);
  static void setThreats(int count);
  static void triggerAlert(const String &alertText);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
