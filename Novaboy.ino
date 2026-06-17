#include "globals.h"
#include "audio.h"
#include "ui.h"
#include "menu.h"
#include "how_to_play.h"
#include "settings.h"
#include "ota_update.h"

#include "game_darkdungeon.h"
#include "game_microcity.h"
#include "game_cardodge.h"
#include "game_ardugolf.h"
#include "game_neonsnake.h"
#include "game_microtd.h"
#include "game_pixelpong.h"
#include "game_tankstrike.h"
#include "game_tinysokoban.h"
#include "game_omegahorizon.h"
#include "game_bangi.h"
#include "game_starshooter.h"
#include "game_neonstacker.h"
#include "game_cubepipes.h"
#include "game_reactionblitz.h"

SPIClass         hspi(FSPI);
Adafruit_ST7789  tft(&hspi, -1, TFT_DC, TFT_RST);
Preferences      prefs;

AppState appState = APP_HOME;
uint8_t  gVolume    = 7;
uint8_t  gBrightness = 8;
bool     firstBoot  = false;

uint16_t C_BG, C_BG2, C_WHITE, C_YELLOW, C_CYAN, C_GREEN, C_RED,
         C_MAGENTA, C_ORANGE, C_PURPLE, C_PINK, C_NEON_G, C_NEON_B, C_NEON_P, C_DARK;

int  hiScore[NUM_GAMES]    = {};
bool gamePlayed[NUM_GAMES] = {};
int  savedLevel[NUM_GAMES] = {};
int  gameMenuSel = 0;

static uint8_t _prevRaw = 0xFF;

bool pcf_write(uint8_t v){
    Wire.beginTransmission(PCF8574_ADDR);
    Wire.write(v);
    return Wire.endTransmission()==0;
}
uint8_t pcf_read(){
    Wire.requestFrom((uint8_t)PCF8574_ADDR,(uint8_t)1);
    return Wire.available() ? Wire.read() : 0xFF;
}
uint8_t btnPressed(){
    uint8_t raw    = ~pcf_read() & 0x3F;
    uint8_t pressed = raw & ~_prevRaw;
    _prevRaw = raw;
    return pressed;
}
uint8_t btnHeld(){
    return ~pcf_read() & 0x3F;
}

void vibroSet(uint8_t intensity){
    digitalWrite(VIBRO_PIN, intensity>0 ? HIGH : LOW);
}
void vibroPulse(uint8_t intensity, uint32_t ms){
    vibroSet(intensity); delay(ms); vibroSet(0);
}

uint16_t dimColor(uint16_t c){
    uint8_t r=(c>>11)&0x1F, g=(c>>5)&0x3F, b=c&0x1F;
    return tft.color565((r>>1)<<3, (g>>1)<<2, (b>>1)<<3);
}
uint16_t blendColor(uint16_t a, uint16_t b2, uint8_t t){
    uint8_t ar=(a>>11)&0x1F,  ag=(a>>5)&0x3F,  ab=a&0x1F;
    uint8_t br=(b2>>11)&0x1F, bg2=(b2>>5)&0x3F, bb=b2&0x1F;
    return tft.color565(
        ((ar*(255-t)+br*t)/255)<<3,
        ((ag*(255-t)+bg2*t)/255)<<2,
        ((ab*(255-t)+bb*t)/255)<<3);
}

void saveData(){
    prefs.begin("nb",false);
    prefs.putUChar("vol",   gVolume);
    prefs.putUChar("bright",gBrightness);
    for(int i=0;i<NUM_GAMES;i++){
        char k[12];
        snprintf(k,12,"hi%d",i);  prefs.putInt(k,hiScore[i]);
        snprintf(k,12,"gp%d",i);  prefs.putBool(k,gamePlayed[i]);
        snprintf(k,12,"sl%d",i);  prefs.putInt(k,savedLevel[i]);
    }
    prefs.end();
}
void loadData(){
    prefs.begin("nb",true);
    gVolume     = prefs.getUChar("vol",   7);
    gBrightness = prefs.getUChar("bright",8);
    for(int i=0;i<NUM_GAMES;i++){
        char k[12];
        snprintf(k,12,"hi%d",i);  hiScore[i]    = prefs.getInt(k,0);
        snprintf(k,12,"gp%d",i);  gamePlayed[i] = prefs.getBool(k,false);
        snprintf(k,12,"sl%d",i);  savedLevel[i] = prefs.getInt(k,0);
    }
    firstBoot = !prefs.getBool("boot",false);
    prefs.end();
    if(firstBoot){
        prefs.begin("nb",false);
        prefs.putBool("boot",true);
        prefs.end();
    }
}

void launchGame(int idx, bool cont){
    int lv = cont ? max(1,savedLevel[idx]) : 1;
    gamePlayed[idx]=true; saveData();
    gameStartCard(G_NAME[idx], G_COL[idx]);
    switch(idx){
        case 0:  darkDungeonInit(lv);    appState=APP_G0;  break;
        case 1:  microCityInit(lv);      appState=APP_G1;  break;
        case 2:  arduRacerInit(lv);      appState=APP_G2;  break;
        case 3:  arduGolfInit(lv);       appState=APP_G3;  break;
        case 4:  neonSnakeInit(lv);      appState=APP_G4;  break;
        case 5:  microTDInit(lv);        appState=APP_G5;  break;
        case 6:  catacombsInit(lv);      appState=APP_G6;  break;
        case 7:  tankStrikeInit(lv);     appState=APP_G7;  break;
        case 8:  tinySokobanInit(lv);    appState=APP_G8;  break;
        case 9:  omegaHorizonInit(lv);   appState=APP_G9;  break;
        case 10: bangiInit(lv);          appState=APP_G10; break;
        case 11: gloveInit(lv);          appState=APP_G11; break;
        case 12: mysticBalloonInit(lv);  appState=APP_G12; break;
        case 13: cubePipesInit(lv);      appState=APP_G13; break;
        case 14: binaryTrainerInit(lv);  appState=APP_G14; break;
    }
}

void setup(){
    Serial.begin(115200);
    pinMode(VIBRO_PIN, OUTPUT);
    digitalWrite(VIBRO_PIN, LOW);
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(100000);
    pcf_write(0xFF);
    hspi.begin(TFT_SCLK, -1, TFT_MOSI, -1);
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, HIGH); delay(10);
    digitalWrite(TFT_RST, LOW);  delay(50);
    digitalWrite(TFT_RST, HIGH); delay(120);
    tft.init(240, 240, SPI_MODE3);
    tft.setRotation(3);
    tft.invertDisplay(true);
    tft.setSPISpeed(40000000UL);
    tft.fillScreen(0xF800); delay(200);
    tft.fillScreen(0x0000); delay(100);
    C_BG      = tft.color565(4,6,16);
    C_BG2     = tft.color565(8,12,28);
    C_WHITE   = tft.color565(240,240,255);
    C_YELLOW  = tft.color565(255,220,0);
    C_CYAN    = tft.color565(0,220,255);
    C_GREEN   = tft.color565(0,220,80);
    C_RED     = tft.color565(255,40,40);
    C_MAGENTA = tft.color565(255,0,200);
    C_ORANGE  = tft.color565(255,140,0);
    C_PURPLE  = tft.color565(160,0,220);
    C_PINK    = tft.color565(255,80,180);
    C_NEON_G  = tft.color565(0,255,80);
    C_NEON_B  = tft.color565(0,160,255);
    C_NEON_P  = tft.color565(200,0,255);
    C_DARK    = tft.color565(4,6,18);
    i2s_init();
    loadData();
    ledcAttach(TFT_BL_PIN, 5000, 8);
    ledcWrite(TFT_BL_PIN, map((int)gBrightness,1,10,10,255));
    bootAnimation();
    if(firstBoot) showFirstBootIntro();
    homeScreen();
}

void loop(){
    switch(appState){
        case APP_HOME:        homeLoop();          break;
        case APP_MENU:        menuLoop();          break;
        case APP_GAME_MENU:   gameMenuLoop();      break;
        case APP_HOW_TO_PLAY: howToPlayLoop();     break;
        case APP_SETTINGS:    settingsLoop();      break;
        case APP_OTA:         otaLoop();           break;
        case APP_G0:          darkDungeonLoop();   break;
        case APP_G1:          microCityLoop();     break;
        case APP_G2:          arduRacerLoop();     break;
        case APP_G3:          arduGolfLoop();      break;
        case APP_G4:          neonSnakeLoop();     break;
        case APP_G5:          microTDLoop();       break;
        case APP_G6:          catacombsLoop();     break;
        case APP_G7:          tankStrikeLoop();    break;
        case APP_G8:          tinySokobanLoop();   break;
        case APP_G9:          omegaHorizonLoop();  break;
        case APP_G10:         bangiLoop();         break;
        case APP_G11:         gloveLoop();         break;
        case APP_G12:         mysticBalloonLoop(); break;
        case APP_G13:         cubePipesLoop();     break;
        case APP_G14:         binaryTrainerLoop(); break;
        default: break;
    }
}