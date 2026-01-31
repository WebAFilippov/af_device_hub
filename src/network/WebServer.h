#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>

#include "../core/DeviceState.h"

class WebServer
{
public:
    void begin(DeviceState &state);
    void update(DeviceState &state);

private:
    AsyncWebServer server{80};
    Preferences prefs;

    void serveGz(const char *url, const char *file, const char *type);
    void setupRoutes(DeviceState &state);

    void sendJsonResponse(AsyncWebServerRequest *request, int code, bool ok, String error = "")
    {
        JsonDocument doc;
        doc["ok"] = ok;
        if (error != "")
            doc["error"] = error;
        String response;
        serializeJson(doc, response);
        request->send(code, "application/json", response);
    }
};

void WebServer::begin(DeviceState &state)
{
    if (!LittleFS.begin())
    {
        Serial0.println("[WEB] Failed to mount LittleFS");
        return;
    }
    prefs.begin("wifi-cfg", false);
    setupRoutes(state);
    server.begin();
    Serial0.println("[WEB] Server started");
}

void WebServer::serveGz(const char *url, const char *file, const char *type)
{
    server.on(url, HTTP_GET, [file, type](AsyncWebServerRequest *req)
              {
        if (!LittleFS.exists(file)) {
            req->send(404);
            return;
        }
        AsyncWebServerResponse *res = req->beginResponse(LittleFS, file, type);
        res->addHeader("Content-Encoding", "gzip");
        req->send(res); });
}

void WebServer::setupRoutes(DeviceState &state)
{
    serveGz("/", "/index.html.gz", "text/html; charset=utf-8");
    serveGz("/main.css", "/main.css.gz", "text/css; charset=utf-8");
    serveGz("/main.js", "/main.js.gz", "application/javascript; charset=utf-8");
    serveGz("/favicon.ico", "/favicon.ico.gz", "image/x-icon");

    server.onNotFound([](AsyncWebServerRequest *req)
                      {
        if (req->method() == HTTP_OPTIONS) { req->send(200); }
        else { req->redirect("/"); } });

    server.on("/api/status", HTTP_GET, [&state](AsyncWebServerRequest *req)
              {
            JsonDocument doc;
            doc["connected"] = (WiFi.status() == WL_CONNECTED);
            doc["ip"] = WiFi.localIP().toString();
            doc["savedSsid"] = state.savedSsid;
                        
            AsyncResponseStream *response = req->beginResponseStream("application/json");
            serializeJson(doc, *response);
            req->send(response); });

    server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *req)
              {
        int n = WiFi.scanComplete();
        if (n == -2) { 
            if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect(); 
            }

            WiFi.scanNetworks(true); 
            req->send(202, "application/json", "{\"status\":\"started\"}");
        } else if (n == -1) {
            req->send(202, "application/json", "{\"status\":\"scanning\"}");
        } else {
            AsyncResponseStream *response = req->beginResponseStream("application/json");
            JsonDocument doc; 
            JsonArray arr = doc["networks"].to<JsonArray>(); 

            for (int i = 0; i < n; i++) {
                JsonObject net = arr.add<JsonObject>(); 
                net["ssid"] = WiFi.SSID(i);
                net["rssi"] = WiFi.RSSI(i);
                net["secure"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
            }
            serializeJson(doc, *response);
            req->send(response);
            WiFi.scanDelete(); 
        } });

    server.on("/api/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data, len);

            if (error) {
                sendJsonResponse(request, 400, false, "invalid_json");
                return;
            }

            String ssid = doc["ssid"] | "";
            String pass = doc["password"] | "";

            if (ssid.length() < 1) {
                sendJsonResponse(request, 400, false, "empty_ssid");
                return;
            }

            prefs.putString("ssid", ssid);
            prefs.putString("pass", pass);

            sendJsonResponse(request, 200, true);

            xTaskCreate([](void *) {
                vTaskDelay(pdMS_TO_TICKS(2000));
                esp_restart();
            }, "reboot", 2048, nullptr, 1, nullptr); });
}

void WebServer::update(DeviceState &state) {}