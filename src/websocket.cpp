#include "PluginManager.h"

#ifdef ENABLE_SERVER

AsyncWebSocket ws("/ws");

void sendInfo()
{
  DynamicJsonDocument jsonDocument(6144);
  if (currentStatus == NONE)
  {
    for (int j = 0; j < ROWS * COLS; j++)
    {
      jsonDocument["data"][j] = Screen.getRenderBuffer()[j];
    }
  }
  jsonDocument["status"] = currentStatus;
  jsonDocument["plugin"] = pluginManager.getActivePlugin()->getId();
  jsonDocument["event"] = "info";
  jsonDocument["rotation"] = Screen.currentRotation;
  jsonDocument["brightness"] = Screen.getCurrentBrightness();

  JsonArray plugins = jsonDocument.createNestedArray("plugins");

  std::vector<Plugin *> &allPlugins = pluginManager.getAllPlugins();
  for (Plugin *plugin : allPlugins)
  {
    JsonObject object = plugins.createNestedObject();

    object["id"] = plugin->getId();
    object["name"] = plugin->getName();
  }

  String output;
  serializeJson(jsonDocument, output);
  ws.textAll(output);
}

void sendTelemetry()
{
  if (ws.count() == 0) return;
  DynamicJsonDocument doc(1024);
  doc["event"] = "telemetry";
#ifdef ESP32
  float tempC = temperatureRead();
  float tempF = tempC * 1.8f + 32.0f;
  doc["temp_c"] = (int)(tempC * 10.0f) / 10.0f;
  doc["temp_f"] = (int)(tempF * 10.0f) / 10.0f;
  doc["hall"] = hallRead();
  doc["cpu_mhz"] = ESP.getCpuFreqMHz();
#else
  doc["temp_c"] = 0;
  doc["temp_f"] = 0;
  doc["hall"] = 0;
  doc["cpu_mhz"] = 80;
#endif
  doc["free_heap"] = ESP.getFreeHeap();
  doc["total_heap"] = ESP.getHeapSize();
  doc["flash_size"] = ESP.getFlashChipSize();
  doc["sketch_size"] = ESP.getSketchSize();
  doc["free_sketch"] = ESP.getFreeSketchSpace();
  doc["rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["ip"] = WiFi.localIP().toString();
  doc["mac"] = WiFi.macAddress();
  doc["brightness"] = Screen.getCurrentBrightness();
  Plugin *active = pluginManager.getActivePlugin();
  doc["active_plugin"] = active ? active->getId() : 1;
  doc["active_name"] = active ? active->getName() : "None";

  String output;
  serializeJson(doc, output);
  ws.textAll(output);
}

void sendMinimalInfo()
{
  DynamicJsonDocument jsonDocument(6144);

  jsonDocument["status"] = currentStatus;
  jsonDocument["plugin"] = pluginManager.getActivePlugin()->getId();
  jsonDocument["event"] = "info";
  jsonDocument["rotation"] = Screen.currentRotation;
  jsonDocument["brightness"] = Screen.getCurrentBrightness();

  String output;
  serializeJson(jsonDocument, output);
  ws.textAll(output);
}

void onWsEvent(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    sendInfo();
  }

  if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
      if (info->opcode == WS_BINARY && currentStatus == WSBINARY && info->len == 256)
      {
        Screen.setRenderBuffer(data, true);
      }
      else if (info->opcode == WS_TEXT)
      {
        data[len] = 0;

        DynamicJsonDocument wsRequest(6144);
        DeserializationError error = deserializeJson(wsRequest, data);

        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        else
        {
          pluginManager.getActivePlugin()->websocketHook(wsRequest);

          const char *event = wsRequest["event"];

          if (!strcmp(event, "plugin"))
          {
            int pluginId = wsRequest["plugin"];

            pluginManager.setActivePluginById(pluginId);

            sendMinimalInfo();
          }
          else if (!strcmp(event, "persist-plugin"))
          {
            pluginManager.persistActivePlugin();
          }
          else if (!strcmp(event, "rotate"))
          {
            bool isRight = (bool)!strcmp(wsRequest["direction"], "right");
            int newRot = (Screen.currentRotation + (isRight ? 1 : 3)) % 4;
            Screen.setRotation(newRot);
            sendMinimalInfo();
          }
          else if (!strcmp(event, "info"))
          {
            sendInfo();
          }
          else if (!strcmp(event, "telemetry"))
          {
            sendTelemetry();
          }
          else if (!strcmp(event, "brightness"))
          {
            Screen.setBrightness(wsRequest["brightness"].as<uint8_t>());
          }
        }
      }
    }
  }
}

void initWebsocketServer(AsyncWebServer &server)
{
  server.addHandler(&ws);
  ws.onEvent(onWsEvent);
}

void cleanUpClients()
{
  ws.cleanupClients();
}

#endif