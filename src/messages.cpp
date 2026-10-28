#include "messages.h"
#include "PluginManager.h"
#include <SPI.h>
#include <cstring>
#include <algorithm>

using namespace std;

Messages_ &Messages_::getInstance()
{
    static Messages_ instance;
    return instance;
}

void Messages_::add(std::string text, int repeat, int id, int delay, std::vector<int> graph, int miny, int maxy)
{
    stopRequested = false;
    remove(id); // replace if already exists with same id
    messages.emplace_back(Message{id, repeat, delay, text, graph, miny, maxy});
    previousMinute = -1; // force immediate display
}

void Messages_::remove(int id)
{
    auto it = std::remove_if(messages.begin(), messages.end(), [id](const Message &msg)
                             { return msg.id == id; });
    messages.erase(it, messages.end());
}

void Messages_::clearAll()
{
    stopRequested = true;
    messages.clear();
    Screen.loadFromStorage();
}

void Messages_::stopContinuous()
{
    stopRequested = true;
    // remove all messages that have repeat == -1
    auto it = std::remove_if(messages.begin(), messages.end(), [](const Message &msg)
                             { return msg.repeat == -1; });
    messages.erase(it, messages.end());
    Screen.loadFromStorage();
}

bool Messages_::hasContinuous() const
{
    for (const auto &msg : messages)
    {
        if (msg.repeat == -1)
            return true;
    }
    return false;
}

void Messages_::runContinuousStep()
{
    for (const auto &msg : messages)
    {
        if (msg.repeat == -1)
        {
            if (stopRequested)
                break;

            if (msg.text.length() > 0)
            {
                Screen.scrollText(msg.text.c_str(), msg.delay);
            }
            if (msg.graph.size() > 0 && !stopRequested)
            {
                Screen.scrollGraph(msg.graph, msg.miny, msg.maxy, msg.delay);
            }
        }
    }
}

void Messages_::scroll()
{
    Screen.persist();
    stopRequested = false;

    for (auto it = messages.begin(); it != messages.end();)
    {
        if (stopRequested)
            break;

        if (it->repeat == -1)
        {
            // Continuous message: skip here, runContinuousStep will handle it
            ++it;
            continue;
        }

        int count = it->repeat > 0 ? it->repeat : 1;
        for (int r = 0; r < count; r++)
        {
            if (stopRequested)
                break;
            if (it->text.length() > 0)
                Screen.scrollText(it->text.c_str(), it->delay);
            if (it->graph.size() > 0 && !stopRequested)
                Screen.scrollGraph(it->graph, it->miny, it->maxy, it->delay);
        }

        it = messages.erase(it);
    }

    if (!hasContinuous())
    {
        Screen.loadFromStorage();
    }
}

void Messages_::scrollMessageEveryMinute()
{
    // If we have non-continuous queued messages waiting, scroll them immediately
    bool hasPendingNormal = false;
    for (const auto &m : messages)
    {
        if (m.repeat != -1)
        {
            hasPendingNormal = true;
            break;
        }
    }

    if (hasPendingNormal && previousMinute == -1)
    {
        scroll();
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            previousMinute = timeinfo.tm_min;
        }
        return;
    }

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        if (timeinfo.tm_sec != previousSecond)
        {
            if (messages.size() > 0)
            {
                indicatorPixel = previousSecond & 0b00000001;
                Screen.setPixel(0, 0, indicatorPixel);
            }
            else
            {
                if (indicatorPixel > 0)
                {
                    indicatorPixel = 0;
                    Screen.setPixel(0, 0, 0);
                }
            }
            previousSecond = timeinfo.tm_sec;
        }
    }
}

Messages_ &Messages = Messages.getInstance();

// http://your-server/message?text=Hello&repeat=3&id=42&graph=1,2,3,4
void handleMessage(AsyncWebServerRequest *request)
{
    std::string text = request->arg("text").c_str();
    int repeat = request->hasArg("repeat") ? request->arg("repeat").toInt() : 1;
    int id = request->arg("id").toInt();
    int delay = request->arg("delay").toInt();
    int miny = request->arg("miny").toInt();
    int maxy = request->arg("maxy").toInt();

    if (delay <= 0)
        delay = 40;

    if (maxy == 0)
        maxy = 15;

    std::string graphParam = request->arg("graph").c_str();
    std::vector<int> graph;

    if (graphParam.length() > 0)
    {
        char *token = strtok(const_cast<char *>(graphParam.c_str()), ",");
        while (token != nullptr)
        {
            graph.push_back(std::stoi(token));
            token = strtok(nullptr, ",");
        }
    }

    Messages.add(text, repeat, id, delay, graph, miny, maxy);
    request->send(200, "text/plain", "Message received");
}

// http://your-server/removemessage?id=42 or /removemessage?all=1
void handleRemove(AsyncWebServerRequest *request)
{
    if (request->hasArg("all") || !request->hasArg("id"))
    {
        Messages.clearAll();
    }
    else
    {
        int id = request->arg("id").toInt();
        Messages.remove(id);
    }
    request->send(200, "text/plain", "Message(s) removed");
}
