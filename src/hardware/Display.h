#pragma once

#include <SPI.h>
#include <TFT_eSPI.h>
#include "../core/DeviceState.h"
#include "../core/Config.h"

class Display
{
public:
    void begin();
    void update(DeviceState &state);

private:
    TFT_eSPI tft;
    bool initialized = false;
    unsigned long lastUpdate = 0;

    // Предыдущие значения для отслеживания изменений
    int32_t lastEncoderPos = 0;
    int lastMotorSpeed = 0;
    int16_t lastCurrentAdc = 0;
    bool lastWifiConnected = false;
    bool lastApActive = false;
    bool lastMqttConnected = false;
    String lastSsid = "";

    // Флаги для полного обновления экрана
    bool fullRedraw = true;

    // Цвета
    static constexpr uint16_t COLOR_BG = TFT_BLACK;
    static constexpr uint16_t COLOR_TEXT = TFT_WHITE;
    static constexpr uint16_t COLOR_HEADER = TFT_CYAN;
    static constexpr uint16_t COLOR_VALUE = TFT_GREEN;
    static constexpr uint16_t COLOR_ALERT = TFT_RED;
    static constexpr uint16_t COLOR_OK = TFT_GREEN;
    static constexpr uint16_t COLOR_WARNING = TFT_YELLOW;

    void drawHeader();
    void drawStatus(DeviceState &state);
    void drawMotorInfo(DeviceState &state);
    void drawNetworkInfo(DeviceState &state);
    void drawFooter(DeviceState &state);
    void drawStaticLayout();
    bool needsFullRedraw(DeviceState &state);
};

void Display::begin()
{
    Serial0.println("[DISPLAY] Starting display init...");

    delay(500);

    Serial0.println("[DISPLAY] Calling tft.init()...");

    tft.init();

    Serial0.println("[DISPLAY] tft.init() completed");

    initialized = true;

    Serial0.println("[DISPLAY] Setting rotation...");
    tft.setRotation(0);

    Serial0.println("[DISPLAY] Filling screen...");
    tft.fillScreen(TFT_BLACK);

    Serial0.println("[DISPLAY] Display initialized successfully");
}

void Display::update(DeviceState &state)
{
    if (!initialized)
        return;

    unsigned long now = millis();
    if (now - lastUpdate < Config::Display::UPDATE_INTERVAL_MS)
        return;

    lastUpdate = now;

    // Проверяем, нужно ли полное обновление
    if (needsFullRedraw(state))
    {
        fullRedraw = true;
        tft.fillScreen(COLOR_BG);
        drawStaticLayout();
    }

    // Обновляем динамические данные
    drawStatus(state);
    drawMotorInfo(state);
    drawNetworkInfo(state);
    drawFooter(state);

    // Сохраняем текущие значения
    lastEncoderPos = state.encoderPos;
    lastMotorSpeed = state.motorSpeed;
    lastCurrentAdc = state.currentAdc;
    lastWifiConnected = state.wifiConnected;
    lastApActive = state.apActive;
    lastMqttConnected = state.mqttConnected;
    lastSsid = state.savedSsid;
    fullRedraw = false;
}

void Display::drawStaticLayout()
{
    // Заголовок
    tft.setTextColor(COLOR_HEADER, COLOR_BG);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(Config::DEVICE_NAME, Config::Display::WIDTH / 2, 5, 4);

    // Разделительная линия под заголовком
    tft.drawLine(5, 30, Config::Display::WIDTH - 5, 30, COLOR_HEADER);

    // Статические подписи для статуса
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("WiFi:", 10, 40, 2);
    tft.drawString("MQTT:", 10, 60, 2);

    // Разделитель
    tft.drawLine(5, 85, Config::Display::WIDTH - 5, 85, COLOR_HEADER);

    // Статические подписи для мотора
    tft.drawString("Motor Speed:", 10, 95, 2);
    tft.drawString("Encoder Pos:", 10, 115, 2);
    tft.drawString("Current:", 10, 135, 2);

    // Разделитель
    tft.drawLine(5, 160, Config::Display::WIDTH - 5, 160, COLOR_HEADER);

    // Статические подписи для сети
    tft.drawString("IP Address:", 10, 170, 2);
    tft.drawString("SSID:", 10, 190, 2);
    tft.drawString("Mode:", 10, 210, 2);

    // Разделитель
    tft.drawLine(5, 235, Config::Display::WIDTH - 5, 235, COLOR_HEADER);
}

void Display::drawStatus(DeviceState &state)
{
    int x = 80;

    // WiFi статус
    if (fullRedraw || lastWifiConnected != state.wifiConnected || lastApActive != state.apActive)
    {
        tft.setTextDatum(TL_DATUM);
        if (state.apActive)
        {
            tft.setTextColor(COLOR_WARNING, COLOR_BG);
            tft.drawString("AP Mode    ", x, 40, 2);
        }
        else if (state.wifiConnected)
        {
            tft.setTextColor(COLOR_OK, COLOR_BG);
            tft.drawString("Connected  ", x, 40, 2);
        }
        else
        {
            tft.setTextColor(COLOR_ALERT, COLOR_BG);
            tft.drawString("Offline    ", x, 40, 2);
        }
    }

    // MQTT статус
    if (fullRedraw || lastMqttConnected != state.mqttConnected)
    {
        tft.setTextDatum(TL_DATUM);
        if (state.mqttConnected)
        {
            tft.setTextColor(COLOR_OK, COLOR_BG);
            tft.drawString("Online ", x, 60, 2);
        }
        else
        {
            tft.setTextColor(COLOR_ALERT, COLOR_BG);
            tft.drawString("Offline", x, 60, 2);
        }
    }
}

void Display::drawMotorInfo(DeviceState &state)
{
    char buffer[32];
    int x = 120;

    // Скорость мотора
    if (fullRedraw || lastMotorSpeed != state.motorSpeed)
    {
        tft.setTextColor(COLOR_VALUE, COLOR_BG);
        tft.setTextDatum(TL_DATUM);
        sprintf(buffer, "%4d/255   ", state.motorSpeed);
        tft.drawString(buffer, x, 95, 2);

        // Визуальный индикатор скорости
        int barWidth = map(abs(state.motorSpeed), 0, 255, 0, 80);
        uint16_t barColor = (state.motorSpeed > 0) ? COLOR_OK : (state.motorSpeed < 0) ? COLOR_ALERT
                                                                                       : COLOR_TEXT;

        // Фон индикатора
        tft.fillRect(200, 96, 80, 10, COLOR_BG);
        tft.drawRect(200, 96, 80, 10, COLOR_TEXT);
        // Заполнение индикатора
        if (barWidth > 0)
        {
            tft.fillRect(200, 96, barWidth, 10, barColor);
        }
    }

    // Позиция энкодера
    if (fullRedraw || lastEncoderPos != state.encoderPos)
    {
        tft.setTextColor(COLOR_VALUE, COLOR_BG);
        sprintf(buffer, "%8ld    ", state.encoderPos);
        tft.drawString(buffer, x, 115, 2);
    }

    // Ток (ADC)
    if (fullRedraw || lastCurrentAdc != state.currentAdc)
    {
        tft.setTextColor(COLOR_VALUE, COLOR_BG);
        sprintf(buffer, "%4d      ", state.currentAdc);
        tft.drawString(buffer, x, 135, 2);
    }
}

void Display::drawNetworkInfo(DeviceState &state)
{
    char buffer[32];

    // IP адрес
    if (fullRedraw || lastWifiConnected != state.wifiConnected)
    {
        tft.setTextColor(COLOR_VALUE, COLOR_BG);
        tft.setTextDatum(TL_DATUM);
        if (state.wifiConnected || state.apActive)
        {
            sprintf(buffer, "%-15s", state.localIP.toString().c_str());
        }
        else
        {
            strcpy(buffer, "Not connected  ");
        }
        tft.drawString(buffer, 100, 170, 2);
    }

    // SSID
    if (fullRedraw || lastSsid != state.savedSsid)
    {
        tft.setTextColor(COLOR_VALUE, COLOR_BG);
        if (state.savedSsid.length() > 0)
        {
            String ssid = state.savedSsid;
            if (ssid.length() > 16)
                ssid = ssid.substring(0, 13) + "...";
            sprintf(buffer, "%-16s", ssid.c_str());
        }
        else
        {
            strcpy(buffer, "None            ");
        }
        tft.drawString(buffer, 100, 190, 2);
    }

    // Режим работы
    if (fullRedraw || lastApActive != state.apActive)
    {
        tft.setTextColor(COLOR_VALUE, COLOR_BG);
        if (state.apActive)
        {
            sprintf(buffer, "Setup (AP)      ");
        }
        else if (state.wifiConnected)
        {
            sprintf(buffer, "Station         ");
        }
        else
        {
            sprintf(buffer, "Not configured  ");
        }
        tft.drawString(buffer, 100, 210, 2);
    }
}

void Display::drawFooter(DeviceState &state)
{
    // Версия прошивки
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("FW: " + String(Config::FIRMWARE_VERSION), Config::Display::WIDTH / 2, 250, 2);

    // Подсказки по кнопкам
    tft.setTextColor(COLOR_HEADER, COLOR_BG);
    tft.drawString("UP: Start  DOWN: Stop  SETUP: 5s AP", Config::Display::WIDTH / 2, 275, 2);

    // Время работы (uptime)
    unsigned long uptime = millis() / 1000;
    int hours = uptime / 3600;
    int minutes = (uptime % 3600) / 60;
    int seconds = uptime % 60;

    char buffer[32];
    sprintf(buffer, "Uptime: %02d:%02d:%02d", hours, minutes, seconds);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.drawString(buffer, Config::Display::WIDTH / 2, 300, 2);
}

bool Display::needsFullRedraw(DeviceState &state)
{
    // Полное обновление при первом запуске или при смене режима WiFi
    return fullRedraw ||
           lastApActive != state.apActive ||
           (lastWifiConnected != state.wifiConnected && !state.wifiConnected);
}