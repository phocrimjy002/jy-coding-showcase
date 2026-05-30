#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiMulti.h> // New: Handles multiple networks
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

// --- CONFIG ---
WiFiMulti wifiMulti;
const char* lat = "1.3521";
const char* lon = "103.8198";

TFT_eSPI tft = TFT_eSPI();

// News Storage
String headlines[5];
int currentNewsIdx = 0;
int showCount = 0;
String activeTickerText = "SCANNING NETWORKS... ";
int scrollX = 320;
unsigned long lastScrollMillis = 0;
unsigned long lastNewsSync = 0;
unsigned long lastWeatherSync = 0;
unsigned long sessionStart;
unsigned long lastUpdate = 0;
const unsigned long cycleDuration = 10 * 60 * 1000;
bool barWiped = false;

// Colors
#define C_ORANGE 0xFD20
#define C_BLUE   0x03FF 

void setup() {
  pinMode(21, OUTPUT); digitalWrite(21, HIGH);
  tft.init(); tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(C_BLUE, TFT_BLACK);
  tft.drawCentreString("INIT MULTI-WIFI...", 160, 110, 2);

  // --- ADD YOUR NETWORKS HERE ---
  wifiMulti.addAP("<set-WIFI-Name>", "<set-WIFI-password>");

  // Attempt connection
  int attempts = 0;
  while (wifiMulti.run() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  configTime(28800, 0, "pool.ntp.org");
  tft.fillScreen(TFT_BLACK);
  sessionStart = millis();
  getLatestNews(); // Initial fetch
}

String cleanText(String text) {
  text.replace("<![CDATA[", ""); text.replace("]]>", "");
  text.replace("&amp;", "&"); text.replace("&quot;", "\"");
  text.trim(); return text;
}

void getLatestNews() {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;
  // Fetching from BBC World RSS
  http.begin(client, "https://feeds.bbci.co.uk/news/world/rss.xml");
  http.setTimeout(4000); 

  if (http.GET() == 200) {
    String payload = http.getString();
    int lastPos = 0;
    for (int i = 0; i < 5; i++) {
      int itemStart = payload.indexOf("<item>", lastPos);
      if (itemStart == -1) break;
      int tStart = payload.indexOf("<title>", itemStart) + 7;
      int tEnd = payload.indexOf("</title>", tStart);
      int dStart = payload.indexOf("<description>", tEnd) + 13;
      int dEnd = payload.indexOf("</description>", dStart);
      
      headlines[i] = "  [ NEWS " + String(i+1) + " ]  " + cleanText(payload.substring(tStart, tEnd)) + 
                     " :: " + cleanText(payload.substring(dStart, dEnd)) + "  *** ";
      headlines[i].toUpperCase();
      lastPos = dEnd;
    }
  }
  http.end();
}

void drawWeather() {
  HTTPClient http;
  String url = "http://api.open-meteo.com/v1/forecast?latitude="+String(lat)+"&longitude="+String(lon)+"&hourly=temperature_2m,weathercode&forecast_days=1";
  http.begin(url);
  if (http.GET() == 200) {
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, http.getString());
    tft.fillRect(0, 182, 320, 38, TFT_BLACK); 
    struct tm ti; getLocalTime(&ti);
    for (int i=0; i<5; i++) {
      int xP = 20 + (i * 60);
      int code = doc["hourly"]["weathercode"][(ti.tm_hour+i+1)%24];
      // Simple Icons
      if (code == 0) tft.fillCircle(xP+10, 195, 5, 0xFFE0); // Sun
      else tft.fillCircle(xP+10, 193, 6, 0x7BEF);           // Cloud
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawFloat(doc["hourly"]["temperature_2m"][(ti.tm_hour+i+1)%24], 1, xP-5, 205, 1);
    }
  }
  http.end();
}

void loop() {
  // Ensure WiFi stays connected / Reconnect if needed
  wifiMulti.run();
  
  unsigned long now = millis();

  // --- TICKER LOGIC ---
  if (now - lastScrollMillis > 30) {
    lastScrollMillis = now;
    tft.setTextColor(C_ORANGE, 0x0010); 
    tft.drawString(activeTickerText, scrollX, 225, 2);
    scrollX -= 2; 

    if (scrollX < -(int)(activeTickerText.length() * 8)) { 
      scrollX = 320; 
      showCount++; 
      
      // Every 2 laps, move to next headline
      if (showCount >= 2) {
        showCount = 0;
        currentNewsIdx++;
        if (currentNewsIdx >= 5 || headlines[currentNewsIdx] == "") {
           currentNewsIdx = 0;
           getLatestNews(); // Fetch FRESH news after cycling all 5 headlines
        }
        activeTickerText = headlines[currentNewsIdx];
      }
    }
    tft.drawFastHLine(0, 220, 320, C_BLUE);
  }

  // --- UI UPDATE ---
  if (now - lastUpdate >= 1000) {
    lastUpdate = now;
    tft.setTextColor(C_ORANGE, TFT_BLACK); tft.drawString("RECTANGULARITY DECK", 10, 8, 2);
    struct tm ti;
    if (getLocalTime(&ti)) {
      char tS[20]; strftime(tS, 20, "%H:%M:%S", &ti);
      tft.setTextColor(C_BLUE, TFT_BLACK); tft.drawRightString(tS, 310, 8, 2);
    }
    unsigned long elapsed = now - sessionStart;
    unsigned long timeInCycle = elapsed % cycleDuration;
    if (timeInCycle < 2000 && !barWiped) { tft.fillRect(0, 145, 320, 35, TFT_BLACK); barWiped = true; } 
    if (timeInCycle > 5000) barWiped = false;
    char cD[10]; sprintf(cD, "%02d:%02d", ((cycleDuration-timeInCycle)/60000), ((cycleDuration-timeInCycle)/1000)%60);
    tft.setTextColor(C_ORANGE, TFT_BLACK); tft.drawCentreString(cD, 160, 45, 7);
    tft.setTextColor(C_BLUE, TFT_BLACK);
    char statS[40]; sprintf(statS, "SESSION: %dm | RPT: %d", (int)(elapsed/60000), (int)(elapsed/cycleDuration));
    tft.drawCentreString(statS, 160, 115, 4);
    int bW = map(timeInCycle, 0, cycleDuration, 0, 280);
    tft.drawRect(20, 150, 282, 14, C_BLUE);
    tft.fillRect(22, 152, bW, 10, C_ORANGE);
  }

  if (now - lastWeatherSync > 600000 || lastWeatherSync == 0) { lastWeatherSync = now; drawWeather(); }
}