#pragma once

#include "PluginManager.h"

class BinClockPlugin : public Plugin
{
private:
    struct tm timeinfo;

    int previousMinutes;
    int previousHour;
    int previousSeconds;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
