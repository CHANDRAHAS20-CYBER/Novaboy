#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <math.h>

#define TFT_MOSI  11
#define TFT_SCLK  12
#define TFT_DC     9
#define TFT_RST   10
#define I2S_BCLK   6
#define I2S_LRC    7
#define I2S_DOUT   8
#define I2C_SDA    3
#define I2C_SCL    4
#define VIBRO_PIN  2
#define PCF8574_ADDR 0x20

#define BTN_UP    (1<<0)
#define BTN_DOWN  (1<<1)
#define BTN_LEFT  (1<<2)
#define BTN_RIGHT (1<<3)
#define BTN_A     (1<<4)
#define BTN_B     (1<<5)

extern uint16_t C_BG, C_BG2, C_WHITE, C_YELLOW, C_CYAN, C_GREEN, C_RED,
                C_MAGENTA, C_ORANGE, C_PURPLE, C_PINK, C_NEON_G, C_NEON_B, C_NEON_P, C_DARK;

enum AppState {
    APP_HOME, APP_MENU, APP_GAME_MENU, APP_SETTINGS, APP_OTA, APP_HOW_TO_PLAY,
    APP_G0=10, APP_G1, APP_G2,  APP_G3,  APP_G4,
    APP_G5,    APP_G6, APP_G7,  APP_G8,  APP_G9,
    APP_G10,   APP_G11,APP_G12, APP_G13, APP_G14
};
extern AppState appState;

extern Adafruit_ST7789 tft;
extern Preferences     prefs;
extern uint8_t         gVolume;
extern uint8_t         gBrightness;
extern bool            firstBoot;

#define NUM_GAMES 15
extern int  hiScore[NUM_GAMES];
extern bool gamePlayed[NUM_GAMES];
extern int  savedLevel[NUM_GAMES];
extern int  gameMenuSel;

void saveData();
void loadData();
bool    pcf_write(uint8_t v);
uint8_t pcf_read();
uint8_t btnPressed();
uint8_t btnHeld();
void    vibroSet(uint8_t intensity);
void    vibroPulse(uint8_t intensity, uint32_t ms);
uint16_t dimColor(uint16_t c);
uint16_t blendColor(uint16_t a, uint16_t b, uint8_t t);

inline uint32_t u32max(uint32_t a, uint32_t b){ return a>b?a:b; }
inline uint32_t u32min(uint32_t a, uint32_t b){ return a<b?a:b; }

struct FrameTimer {
    uint32_t last = 0;
    uint16_t ms;
    FrameTimer(uint16_t m=33):ms(m){}
    bool ready(){
        uint32_t now = millis();
        if(now - last >= ms){ last = now; return true; }
        vTaskDelay(1);
        return false;
    }
};

void menuInit();
void settingsInit();
void homeScreen();
void showGameMenu(int idx);
void launchGame(int idx, bool continueGame);