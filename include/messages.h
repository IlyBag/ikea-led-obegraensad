#pragma once

#include <vector>
#include <string>
#include "screen.h"

class Message
{
public:
  int id;
  int repeat; // -1 = continuous loop, >0 = repeat count
  int delay;
  std::string text;
  std::vector<int> graph;
  int miny;
  int maxy;
};

class Messages_
{
private:
  Messages_() : previousMinute(-1), previousSecond(-1), indicatorPixel(0), stopRequested(false) {}
  std::vector<Message> messages;
  int previousMinute;
  int previousSecond;
  int indicatorPixel;
  volatile bool stopRequested;

public:
  static Messages_ &getInstance();

  Messages_(const Messages_ &) = delete;
  Messages_ &operator=(const Messages_ &) = delete;

  void add(std::string text, int repeat = 1, int id = 0, int delay = 50, std::vector<int> graph = {}, int miny = 0, int maxy = 15);
  void remove(int id = 0);
  void clearAll();
  void stopContinuous();

  bool hasContinuous() const;
  bool isStopRequested() const { return stopRequested; }
  void setStopRequested(bool req) { stopRequested = req; }

  void scroll();
  void runContinuousStep();
  void scrollMessageEveryMinute();
};

extern Messages_ &Messages;

void handleMessage(AsyncWebServerRequest *request);
void handleRemove(AsyncWebServerRequest *request);
