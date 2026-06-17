#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"
extern void homeScreen();
extern void otaInit();

#define TFT_BL_PIN 21

static int stSel=0;
static bool blAttached=false;

static void applyBrightness(){
    if(!blAttached){
        ledcAttach(TFT_BL_PIN, 5000, 8);
        blAttached=true;
    }
    int val=map((int)gBrightness,1,10,10,255);
    ledcWrite(TFT_BL_PIN,val);
}

static void stDraw(){
    tft.fillScreen(C_BG);
    tft.fillRect(0,0,240,26,tft.color565(0,0,0));
    tft.drawFastHLine(0,26,240,C_CYAN);
    glowText("SETTINGS",6,1,C_CYAN);
    tft.setTextColor(tft.color565(38,48,80)); tft.setTextSize(1);
    tft.setCursor(4,16); tft.print("B: Back");

    const char* labels[]={"VOLUME","BRIGHTNESS","CLEAR ALL DATA","WIRELESS UPDATE"};
    uint16_t cols[]={C_GREEN,C_YELLOW,C_RED,C_CYAN};

    for(int i=0;i<4;i++){
        bool s=(i==stSel);
        int by=30+i*46;
        tft.fillRoundRect(6,by,228,40,7,s?tft.color565(7,14,30):tft.color565(4,6,18));
        tft.drawRoundRect(6,by,228,40,7,s?cols[i]:tft.color565(18,24,46));
        if(s){tft.fillRect(6,by,4,40,cols[i]);tft.fillRect(230,by,4,40,cols[i]);}
        tft.setTextSize(1); tft.setTextColor(s?cols[i]:tft.color565(65,75,105));
        tft.setCursor(16,by+8); tft.print(labels[i]);
        if(i==0){
            tft.fillRect(16,by+22,196,8,tft.color565(8,10,22));
            tft.drawRect(16,by+22,196,8,tft.color565(28,36,58));
            int bw=(gVolume*196)/10;
            uint16_t vc=gVolume>6?C_GREEN:(gVolume>3?C_YELLOW:C_RED);
            if(bw>0)tft.fillRect(16,by+22,bw,8,vc);
            char vb[10]; snprintf(vb,10,"%d/10",gVolume);
            tft.setTextColor(vc); tft.setCursor(202,by+8); tft.print(vb);
        } else if(i==1){
            tft.fillRect(16,by+22,196,8,tft.color565(8,10,22));
            tft.drawRect(16,by+22,196,8,tft.color565(28,36,58));
            int bw=(gBrightness*196)/10;
            uint16_t bc2=gBrightness>6?C_YELLOW:(gBrightness>3?tft.color565(200,200,0):tft.color565(120,120,0));
            if(bw>0)tft.fillRect(16,by+22,bw,8,bc2);
            char vb[12]; snprintf(vb,12,"%d/10",gBrightness);
            tft.setTextColor(bc2); tft.setCursor(202,by+8); tft.print(vb);
        } else {
            tft.setTextColor(tft.color565(65,75,105));
            tft.setCursor(16,by+22);
            if(i==2) tft.print(s?"A: Confirm reset — cannot undo":"Resets scores & progress");
            if(i==3) tft.print(s?"A: Start WiFi & OTA update":"WiFi firmware update");
        }
    }
    tft.fillRect(0,216,240,12,tft.color565(0,0,0));
    int tot=0; for(int i=0;i<NUM_GAMES;i++) tot+=hiScore[i];
    char sb[40]; snprintf(sb,40,"Total Score: %d",tot);
    cText(sb,218,1,tft.color565(38,48,80));
    tft.fillRect(0,230,240,10,tft.color565(0,0,0));
    cText("UP/DOWN: Nav    L/R: Adjust",232,1,tft.color565(24,32,54));
}

void settingsInit(){
    applyBrightness();
    stSel=0; stDraw();
}

void settingsLoop(){
    uint8_t p=btnPressed();
    if(!p){vTaskDelay(1);return;}
    if(p&BTN_B){saveData();appState=APP_HOME;homeScreen();return;}
    if(p&BTN_UP)  {stSel=(stSel+3)%4;stDraw();sfx_ui();}
    if(p&BTN_DOWN){stSel=(stSel+1)%4;stDraw();sfx_ui();}
    if(stSel==0){
        if(p&BTN_LEFT  &&gVolume>0 ){gVolume--;saveData();stDraw();}
        if(p&BTN_RIGHT &&gVolume<10){gVolume++;saveData();stDraw();}
        if(p&BTN_A) playTone(440+gVolume*44,80,0.7f);
    } else if(stSel==1){
        if(p&BTN_LEFT  &&gBrightness>1 ){gBrightness--;applyBrightness();saveData();stDraw();}
        if(p&BTN_RIGHT &&gBrightness<10){gBrightness++;applyBrightness();saveData();stDraw();}
    } else if(stSel==2){
        if(p&BTN_A){
            for(int i=0;i<NUM_GAMES;i++){hiScore[i]=0;gamePlayed[i]=false;savedLevel[i]=0;}
            saveData();sfx_die();
            cText("ALL DATA CLEARED!",120,1,C_RED);delay(1400);stDraw();
        }
    } else {
        if(p&BTN_A){saveData();appState=APP_OTA;otaInit();}
    }
}