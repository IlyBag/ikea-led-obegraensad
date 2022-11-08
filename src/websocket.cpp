#include "websocket.h"

AsyncWebSocket ws("/ws");

void sendStateAndInfo(AsyncWebSocketClient *client)
{
  StaticJsonDocument<6144> jsonDocument;
  for (int j = 0; j < sizeof(render_buffer); j++)
  {
    jsonDocument["data"][j] = render_buffer[j];
  }
  jsonDocument["mode"] = currentMode;
  jsonDocument["event"] = "info";
  jsonDocument["rotation"] = currentRotation;

  String output;
  serializeJson(jsonDocument, output);
  client->text(output);
}

void sendModeToAllClients(MODE mode)
{
  StaticJsonDocument<6144> jsonDocument;

  jsonDocument["event"] = "mode";
  jsonDocument["mode"] = mode;
  jsonDocument["rotation"] = currentRotation;

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
    sendStateAndInfo(client);
  }

  if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
      data[len] = 0;

      StaticJsonDocument<6144> wsRequest;
      DeserializationError error = deserializeJson(wsRequest, data);

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else
      {
        const char *event = wsRequest["event"];

        if (!strcmp(event, "mode"))
        {
          setModeByString(wsRequest["mode"], sendModeToAllClients);
        }
        else if (!strcmp(event, "rotate"))
        {
          rotate(positions, !strcmp(wsRequest["direction"], "right"));

          if (currentMode == NONE)
          {
            renderScreen(render_buffer);
          }
        }
        else if (!strcmp(event, "info"))
        {
          sendStateAndInfo(client);
        }

        if (currentMode == NONE)
        {
          if (!strcmp(event, "clear"))
          {
            memset(render_buffer, 0, sizeof(render_buffer));
            renderScreen(render_buffer);
          }
          else if (!strcmp(event, "led"))
          {
            setPixelAtIndex(render_buffer, wsRequest["index"], wsRequest["status"]);
            renderScreen(render_buffer);
          }
          else if (!strcmp(event, "screen"))
          {
            for (int i = 0; i < ROWS * COLS; i++)
            {
              render_buffer[i] = wsRequest["data"][i];
            }
            renderScreen(render_buffer);
          }
          else if (!strcmp(event, "persist"))
          {
            storage.begin("led-wall", false);
            storage.putBytes("data", render_buffer, sizeof(render_buffer));
            storage.end();
          }
          else if (!strcmp(event, "load"))
          {
            clearScreenAndBuffer(render_buffer);
            storage.begin("led-wall", false);
            storage.getBytes("data", render_buffer, sizeof(render_buffer));
            storage.end();
            renderScreen(render_buffer);
            sendStateAndInfo(client);
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