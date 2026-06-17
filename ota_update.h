#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"
#include <esp_wifi.h>

#define OTA_SSID     "Chandu"
#define OTA_PASS     "9949177088"
#define OTA_HOSTNAME "NovaBoy-S3"
#define OTA_PASSWORD "novaboy123"

#define OTA_TX_POWER WIFI_POWER_15dBm

static bool   _rdy=false, _active=false, _connected=false;
static int    _prog=0;
static volatile int _discReason=0;

static const char* _discReasonStr(int r){
    static char buf[30];
    switch(r){
        case 2:   return "Auth expired";
        case 3:   return "Auth - left network";
        case 4:   return "Assoc expired";
        case 5:   return "Hotspot full";
        case 6:   return "Not authenticated";
        case 7:   return "Not associated";
        case 8:   return "Link lost";
        case 15:  return "Handshake timeout";
        case 200: return "Beacon timeout - too far?";
        case 201: return "SSID NOT FOUND";
        case 202: return "Auth failed - wrong password?";
        case 203: return "Association failed";
        case 204: return "4-way handshake timeout";
        case 0:   return "Connecting...";
        default:  snprintf(buf,30,"Reason code: %d",r); return buf;
    }
}

static void _otaWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info){
    if(event==ARDUINO_EVENT_WIFI_STA_DISCONNECTED){
        _discReason = info.wifi_sta_disconnected.reason;
    } else if(event==ARDUINO_EVENT_WIFI_STA_GOT_IP){
        _discReason = 0;
    }
}

static void _otaDraw(){
    tft.fillScreen(C_BG);
    tft.fillRect(0,0,240,24,tft.color565(0,0,0));
    tft.drawFastHLine(0,24,240,C_CYAN);
    glowText("WIRELESS UPDATE",6,1,C_CYAN);
    tft.setTextColor(tft.color565(38,48,80)); tft.setTextSize(1);
    tft.setCursor(4,14); tft.print("B: Cancel");
    if(!_rdy){ cText("Connecting to WiFi...",120,1,C_YELLOW); return; }
    if(!_connected){
        glowText("FAILED!",62,2,C_RED);
        tft.setTextSize(1); tft.setTextColor(C_WHITE);
        tft.setCursor(8,94); tft.print("Network: "); tft.print(OTA_SSID);
        tft.setTextColor(C_ORANGE);
        tft.setCursor(8,108); tft.print("Reason:");
        tft.setCursor(8,118); tft.print(_discReasonStr(_discReason));
        tft.setTextColor(tft.color565(150,160,195));
        tft.setCursor(8,136); tft.print("Check hotspot is ON");
        tft.setCursor(8,148); tft.print("Use 2.4 GHz band only");
        tft.setCursor(8,160); tft.print("iPhone: Personal Hotspot >");
        tft.setCursor(8,170); tft.print("Maximise Compatibility = ON");
        tft.setTextColor(C_YELLOW);
        tft.setCursor(8,186); tft.print("A=Retry  B=Back");
        neonBtn(8,206,108,26,"A  RETRY",C_GREEN,true);
        neonBtn(124,206,108,26,"B  BACK",C_RED,true);
        return;
    }
    tft.setTextColor(C_GREEN); tft.setTextSize(1);
    tft.setCursor(8,32); tft.print("WiFi Connected!");
    tft.setTextColor(C_WHITE);
    tft.setCursor(8,44); tft.print("Network: "); tft.print(WiFi.SSID());
    tft.setCursor(8,56); tft.print("IP:  "); tft.print(WiFi.localIP().toString());
    tft.setCursor(8,68); tft.print("Signal: "); tft.print(WiFi.RSSI()); tft.print(" dBm");
    tft.drawFastHLine(4,80,232,tft.color565(0,40,40));
    tft.setTextColor(tft.color565(160,170,200));
    tft.setCursor(8,86);  tft.print("Arduino IDE 2.x:");
    tft.setCursor(8,98);  tft.print("1. Tools > Port");
    tft.setCursor(8,110); tft.print("2. Network Ports > NovaBoy-S3");
    tft.setTextColor(C_YELLOW);
    tft.setCursor(8,122); tft.print("3. Upload sketch");
    tft.setTextColor(C_CYAN);
    char pw[32]; snprintf(pw,32,"OTA Password: %s",OTA_PASSWORD);
    tft.setCursor(8,134); tft.print(pw);
    tft.drawFastHLine(4,146,232,tft.color565(0,40,40));
    if(_active){
        char pc[24]; snprintf(pc,24,"Uploading  %d%%",_prog);
        glowText(pc,160,2,C_CYAN);
        tft.drawRect(8,188,224,18,tft.color565(0,55,55));
        tft.fillRect(10,190,(_prog*220)/100,14,C_CYAN);
        glowText("DO NOT POWER OFF",210,1,C_RED);
    } else {
        cText("Waiting for IDE upload...",158,1,tft.color565(0,190,210));
        cText("B: Cancel",212,1,tft.color565(55,65,95));
    }
}

static void _otaDrawProgressOnly(){
    char pc[24]; snprintf(pc,24,"Uploading  %d%%  ",_prog);
    tft.setTextSize(2); tft.setTextColor(C_CYAN);
    tft.setCursor(20,155); tft.print(pc);
    int bw=(_prog*220)/100;
    tft.fillRect(10,190,bw,14,C_CYAN);
    if(_prog<100)
        tft.fillRect(10+bw,190,220-bw,14,tft.color565(0,20,20));
}

static void _otaStartCB(){
    _active=true; _prog=0; sfx_beep(); _otaDraw();
}
static void _otaProgressCB(unsigned int done, unsigned int total){
    _prog=(int)((uint64_t)done*100/total);
    static uint32_t lp=0;
    if(millis()-lp>500){ lp=millis(); _otaDrawProgressOnly(); }
}
static void _otaEndCB(){
    tft.fillScreen(tft.color565(0,20,0));
    glowText("UPDATE DONE!",100,2,C_GREEN);
    cText("Restarting...",130,1,C_WHITE);
    sfx_powerup(); delay(2000); ESP.restart();
}
static void _otaErrorCB(ota_error_t e){
    _active=false;
    tft.fillScreen(tft.color565(20,0,0));
    glowText("UPDATE FAILED",100,2,C_RED);
    char eb[24]; snprintf(eb,24,"Error code: %d",(int)e);
    cText(eb,130,1,C_WHITE);
    sfx_die(); delay(3000); _otaDraw();
}

static bool _otaTryConnect(uint32_t timeoutMs){
    _discReason=0;
    WiFi.begin(OTA_SSID, OTA_PASS);
    uint32_t start=millis(); int dot=0;
    while(WiFi.status()!=WL_CONNECTED){
        if(millis()-start>timeoutMs) return false;
        delay(300);
        int bw=(int)((millis()-start)*220/timeoutMs);
        tft.fillRect(10,100,220,8,tft.color565(8,8,8));
        tft.fillRect(10,100,bw>220?220:bw,8,tft.color565(0,100,200));
        tft.fillRect(20,114,200,12,C_BG);
        tft.setTextSize(1); tft.setTextColor(C_CYAN);
        char ds[12]="Trying";
        for(int d=0;d<(dot%4);d++) ds[6+d]='.';
        ds[6+(dot%4)]=0;
        tft.setCursor(80,116); tft.print(ds); dot++;
        tft.fillRect(4,130,232,12,C_BG);
        tft.setTextColor(_discReason?C_RED:tft.color565(100,100,140));
        tft.setCursor(4,132); tft.print(_discReasonStr(_discReason));
        if(btnPressed()&BTN_B) return false;
    }
    return true;
}

void otaInit(){
    _rdy=false; _active=false; _connected=false; _prog=0; _discReason=0;

    tft.fillScreen(C_BG);
    tft.fillRect(0,0,240,24,tft.color565(0,0,0));
    tft.drawFastHLine(0,24,240,C_CYAN);
    glowText("WIRELESS UPDATE",6,1,C_CYAN);
    tft.setTextSize(1); tft.setTextColor(C_YELLOW);
    tft.setCursor(8,36); tft.print("Connecting to:");
    tft.setTextColor(C_WHITE);
    tft.setCursor(8,50); tft.print(OTA_SSID);
    tft.setTextColor(tft.color565(140,150,180));
    tft.setCursor(8,66); tft.print("Make sure hotspot is ON");
    tft.setCursor(8,80); tft.print("2.4 GHz band required");
    cText("B: Cancel",214,1,tft.color565(55,65,95));

    
    WiFi.onEvent(_otaWifiEvent);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);
    WiFi.setSleep(false);
    WiFi.setTxPower(OTA_TX_POWER);
    esp_wifi_set_ps(WIFI_PS_NONE);
    delay(100);

    bool ok = _otaTryConnect(15000);

    _rdy=true;
    if(!ok){
        _connected=false;
        sfx_die();
        _otaDraw();
        return;
    }

    _connected=true;
    sfx_score();
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.onStart(_otaStartCB);
    ArduinoOTA.onProgress(_otaProgressCB);
    ArduinoOTA.onEnd(_otaEndCB);
    ArduinoOTA.onError(_otaErrorCB);
    ArduinoOTA.begin();
    _otaDraw();
}

void otaLoop(){
    uint8_t p=btnPressed();
    if((p&BTN_B)&&!_active){
        if(_connected) ArduinoOTA.end();
        WiFi.disconnect(true); WiFi.mode(WIFI_OFF);
        appState=APP_HOME; homeScreen(); return;
    }
    if((p&BTN_A)&&!_connected&&_rdy&&!_active){
        sfx_ui(); otaInit(); return;
    }
    if(_connected) ArduinoOTA.handle();
    
}