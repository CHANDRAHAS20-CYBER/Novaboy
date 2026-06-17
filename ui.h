#pragma once
#include "globals.h"
#include "audio.h"

extern const char* G_NAME[];
extern const uint16_t G_COL[];


void cText(const char* s, int y, uint8_t sz, uint16_t col){
    tft.setTextSize(sz); tft.setTextColor(col);
    int w=strlen(s)*6*sz;
    tft.setCursor((240-w)/2, y); tft.print(s);
}
void lText(int x, int y, const char* s, uint8_t sz, uint16_t col){
    tft.setTextSize(sz); tft.setTextColor(col);
    tft.setCursor(x,y); tft.print(s);
}
void glowText(const char* s, int y, uint8_t sz, uint16_t col){
    uint16_t gc=dimColor(dimColor(col));
    int w=strlen(s)*6*sz, x=(240-w)/2;
    tft.setTextSize(sz);
    for(int ox=-1;ox<=1;ox++) for(int oy=-1;oy<=1;oy++){
        if(ox==0&&oy==0) continue;
        tft.setTextColor(gc); tft.setCursor(x+ox,y+oy); tft.print(s);
    }
    tft.setTextColor(col); tft.setCursor(x,y); tft.print(s);
}
void neonLine(int y, uint16_t col){
    tft.drawFastHLine(0,y,240,dimColor(col));
    tft.drawFastHLine(4,y+1,232,col);
    tft.drawFastHLine(0,y+2,240,dimColor(col));
}
void neonBtn(int x, int y, int w, int h, const char* lbl, uint16_t col, bool sel){
    tft.fillRoundRect(x,y,w,h,5,sel?tft.color565(8,18,38):C_DARK);
    tft.drawRoundRect(x,y,w,h,5,col);
    if(sel){
        tft.drawRoundRect(x+1,y+1,w-2,h-2,4,dimColor(col));
        tft.fillRect(x,y+4,3,h-8,col);
        tft.fillRect(x+w-3,y+4,3,h-8,col);
    }
    tft.setTextSize(1); tft.setTextColor(sel?col:tft.color565(70,80,110));
    int lw=strlen(lbl)*6;
    tft.setCursor(x+(w-lw)/2,y+(h-8)/2); tft.print(lbl);
}
void neonPanel(int x, int y, int w, int h, uint16_t col){
    tft.fillRoundRect(x,y,w,h,7,tft.color565(4,6,20));
    tft.drawRoundRect(x,y,w,h,7,col);
    tft.drawRoundRect(x+1,y+1,w-2,h-2,6,dimColor(col));
}

// Dark star field — uses C_BG as base, so erasing works
void drawStarBG(uint32_t seed){
    tft.fillScreen(C_BG);
    srand(seed);
    for(int i=0;i<60;i++){
        int x=rand()%240, y=rand()%240;
        uint8_t b=20+rand()%140;
        tft.drawPixel(x,y,tft.color565(b>>1,b>>1,b));
    }
}

void statusBar(const char* name, int score, int hi){
    tft.fillRect(0,0,240,20,tft.color565(2,3,10));
    tft.drawFastHLine(0,20,240,C_NEON_B);
    tft.setTextSize(1); tft.setTextColor(C_CYAN);
    tft.setCursor(4,6); tft.print(name);
    tft.setTextColor(C_YELLOW);
    char sc[20]; snprintf(sc,20,"SCR %d",score);
    tft.setCursor(96,6); tft.print(sc);
    tft.setTextColor(tft.color565(100,120,160));
    char hc[20]; snprintf(hc,20,"HI %d",hi);
    tft.setCursor(178,6); tft.print(hc);
}

uint8_t showPauseMenu(){
    for(int y=44;y<200;y++) tft.drawFastHLine(18,y,204,tft.color565(2,4,16));
    tft.drawRoundRect(18,44,204,156,9,C_NEON_B);
    tft.fillRect(18,44,204,32,tft.color565(0,20,60));
    tft.drawFastHLine(18,76,204,C_NEON_B);
    tft.fillRect(108,52,6,18,C_YELLOW); tft.fillRect(120,52,6,18,C_YELLOW);
    glowText("PAUSED",58,1,C_YELLOW);
    const char* opts[]={"RESUME","RESTART","MAIN MENU"};
    uint16_t oc[]={C_GREEN,C_CYAN,C_RED};
    int sel=0;
    auto draw=[&](){
        for(int i=0;i<3;i++){
            bool s=(i==sel); int by=82+i*36;
            tft.fillRoundRect(26,by,188,28,6,s?tft.color565(8,20,44):tft.color565(4,6,18));
            tft.drawRoundRect(26,by,188,28,6,s?oc[i]:tft.color565(22,28,50));
            if(s){tft.fillRect(26,by+6,3,16,oc[i]);tft.fillRect(211,by+6,3,16,oc[i]);}
            tft.setTextSize(1); tft.setTextColor(s?oc[i]:tft.color565(60,70,100));
            int lw=strlen(opts[i])*6;
            tft.setCursor(26+(188-lw)/2,by+10); tft.print(opts[i]);
        }
        tft.fillRect(18,194,204,6,tft.color565(2,3,10));
        cText("UP/DN  A=Confirm  B=Resume",196,1,tft.color565(30,40,70));
    };
    draw(); delay(160);
    while(true){
        uint8_t p=btnPressed();
        if(p&BTN_UP)  {sel=(sel+2)%3;draw();sfx_ui();delay(70);}
        if(p&BTN_DOWN){sel=(sel+1)%3;draw();sfx_ui();delay(70);}
        if(p&BTN_A){sfx_select();return(uint8_t)(sel+1);}
        if(p&BTN_B){sfx_select();return 1;}
        vTaskDelay(1);
    }
}

bool showGameOver(int score, int hi, bool newHi){
    for(int i=0;i<3;i++){tft.invertDisplay(true);delay(40);tft.invertDisplay(false);delay(40);}
    sfx_die();
    for(int y=58;y<190;y++) tft.drawFastHLine(12,y,216,tft.color565(6,0,0));
    tft.drawRoundRect(12,58,216,132,9,C_RED);
    tft.fillRect(12,58,216,30,tft.color565(80,0,10));
    tft.drawFastHLine(12,88,216,C_RED);
    glowText("GAME  OVER",64,2,C_WHITE);
    char sb[24]; snprintf(sb,24,"SCORE   %d",score); cText(sb,98,1,C_WHITE);
    char hb[24]; snprintf(hb,24,"BEST    %d",hi);   cText(hb,112,1,tft.color565(150,160,200));
    if(newHi){
        tft.fillRoundRect(46,126,148,16,5,tft.color565(50,40,0));
        tft.drawRoundRect(46,126,148,16,5,C_YELLOW);
        cText("NEW HIGH SCORE!",128,1,C_YELLOW);
    }
    neonBtn(18,154,96,28,"A  RETRY",C_GREEN,true);
    neonBtn(126,154,96,28,"B  MENU",C_RED,true);
    while(true){
        uint8_t p=btnPressed();
        if(p&BTN_A){sfx_select();return true;}
        if(p&BTN_B){sfx_back();return false;}
        vTaskDelay(1);
    }
}

void showLevelUp(int lv, uint16_t col){
    sfx_level(); vibroPulse(180,100);
    for(int y=88;y<162;y++) tft.drawFastHLine(10,y,220,tft.color565(0,8,0));
    tft.drawRoundRect(10,88,220,74,8,col);
    tft.fillRect(10,88,220,28,tft.color565(0,36,10));
    tft.drawFastHLine(10,116,220,col);
    glowText("LEVEL CLEAR",92,1,col);
    char lb[24]; snprintf(lb,24,"ENTERING  LEVEL  %d",lv);
    cText(lb,122,1,C_WHITE);
    cText("Keep going!",138,1,tft.color565(120,140,180));
    delay(1500);
}

void gameStartCard(const char* name, uint16_t col){
    tft.fillScreen(C_BG);
    for(int r=70;r>=4;r-=8){
        tft.drawCircle(120,110,r,r%16<8?col:dimColor(col));
        if(r%24==0){playTone(200+r*5,10,0.3f);delay(6);}
    }
    tft.fillRoundRect(10,84,220,52,10,tft.color565(4,6,20));
    tft.drawRoundRect(10,84,220,52,10,col);
    tft.fillRect(10,84,220,16,col);
    tft.setTextColor(C_BG); tft.setTextSize(1);
    cText("INITIALIZING",86,1,C_BG);
    glowText(name,98,2,col);
    cText("A: LAUNCH   B: PAUSE",130,1,tft.color565(100,110,150));
    sfx_start();
    uint32_t t=millis();
    while(true){uint8_t p=btnPressed();if((p&BTN_A)||millis()-t>3000)break;vTaskDelay(1);}
    sfx_select(); tft.fillScreen(C_BG);
}

void bootAnimation(){
    tft.fillScreen(0x0000);
    for(int pass=0;pass<6;pass++){
        for(int cc=0;cc<20;cc++){
            int x=cc*12; uint8_t g=30+random(200);
            tft.setTextSize(1); tft.setTextColor(tft.color565(0,g,0));
            char c2[2]={(char)('!'+random(80)),0};
            tft.setCursor(x,random(220)); tft.print(c2);
        }
        playTone(180+pass*40,16,0.18f); delay(28);
    }
    delay(60); tft.fillScreen(0x0000);
    const char* L="NOVABOY";
    uint16_t lc[]={C_NEON_B,C_CYAN,C_NEON_B,C_NEON_P,C_PINK,C_NEON_P,C_CYAN};
    for(int i=0;i<7;i++){
        tft.setTextSize(4); char ch[2]={L[i],0};
        tft.setTextColor(tft.color565(0,20,40));
        tft.setCursor(10+i*30+1,54); tft.print(ch);
        tft.setTextColor(dimColor(lc[i]));
        tft.setCursor(10+i*30-1,54); tft.print(ch);
        tft.setTextColor(lc[i]);
        tft.setCursor(10+i*30,52);   tft.print(ch);
        playTone(300+i*80,24,0.5f); delay(50);
    }
    for(int x=8;x<232;x+=4){tft.fillRect(x,98,4,3,C_NEON_B);tft.fillRect(x,98,4,1,C_WHITE);delay(3);}
    cText("HANDHELD GAMING CONSOLE",104,1,tft.color565(55,65,115));
    playTone(523,55,0.7f);playTone(659,55,0.8f);playTone(784,55,0.8f);playTone(1047,90,0.9f);
    delay(240);
    for(int i=0;i<3;i++){tft.invertDisplay(true);delay(34);tft.invertDisplay(false);delay(34);}
    tft.fillScreen(C_BG);
}

void homeScreen(){
    tft.fillScreen(C_BG);
    srand(2024);
    for(int i=0;i<55;i++){int x=rand()%240,y=rand()%240;uint8_t b=20+rand()%120;tft.drawPixel(x,y,tft.color565(b>>1,b>>1,b));}
    for(int rr=44;rr>=4;rr-=8) tft.drawCircle(120,90,rr,tft.color565(0,0,20+rr*2));
    const char* L="NOVABOY";
    uint16_t lc[]={C_NEON_B,C_CYAN,C_NEON_B,C_NEON_P,C_PINK,C_NEON_P,C_CYAN};
    for(int i=0;i<7;i++){
        tft.setTextSize(3); char ch[2]={L[i],0};
        tft.setTextColor(tft.color565(0,12,32));
        tft.setCursor(11+i*31+1,38); tft.print(ch);
        tft.setTextColor(dimColor(lc[i]));
        tft.setCursor(11+i*31-1,38); tft.print(ch);
        tft.setTextColor(lc[i]);
        tft.setCursor(11+i*31,36); tft.print(ch);
    }
    tft.fillRect(8,84,224,2,C_NEON_B); tft.fillRect(12,87,216,1,dimColor(C_NEON_B));
    cText("HANDHELD GAMING CONSOLE",92,1,tft.color565(50,60,110));
    tft.fillRoundRect(74,104,92,16,5,tft.color565(0,20,50));
    tft.drawRoundRect(74,104,92,16,5,C_NEON_B); cText("15 GAMES",107,1,C_CYAN);
    tft.fillRoundRect(24,130,192,34,9,tft.color565(0,50,16));
    tft.drawRoundRect(24,130,192,34,9,C_GREEN);
    tft.drawRoundRect(25,131,190,32,8,dimColor(C_GREEN));
    cText("PRESS  A  TO  PLAY",143,1,C_GREEN);
    int tot=0; for(int i=0;i<NUM_GAMES;i++) tot+=hiScore[i];
    int played=0; for(int i=0;i<NUM_GAMES;i++) if(gamePlayed[i]) played++;
    tft.fillRect(0,174,240,20,tft.color565(0,0,0));
    char stat[40]; snprintf(stat,40,"Played %d/15    Score %d",played,tot);
    cText(stat,178,1,tft.color565(50,60,100));
    tft.fillRect(0,196,240,14,tft.color565(0,0,0));
    cText("B = Settings",198,1,tft.color565(35,45,70));
    tft.fillRect(0,226,240,14,tft.color565(0,0,0));
    cText("(c) NovaBoy",228,1,tft.color565(20,25,45));
}

void homeLoop(){
    static uint32_t pulse=0; static bool pst=false;
    if(millis()-pulse>640){
        pulse=millis(); pst=!pst;
        uint16_t bc=pst?C_GREEN:tft.color565(0,40,12);
        uint16_t fc=pst?C_GREEN:tft.color565(0,80,24);
        tft.fillRoundRect(24,130,192,34,9,pst?tft.color565(0,50,16):tft.color565(0,28,8));
        tft.drawRoundRect(24,130,192,34,9,bc);
        tft.drawRoundRect(25,131,190,32,8,dimColor(bc));
        cText("PRESS  A  TO  PLAY",143,1,fc);
    }
    uint8_t p=btnPressed();
    if(p&BTN_A){sfx_select();appState=APP_MENU;menuInit();}
    if(p&BTN_B){sfx_select();appState=APP_SETTINGS;settingsInit();}
    vTaskDelay(1);
}

void showFirstBootIntro(){
    tft.fillScreen(C_BG);
    glowText("WELCOME TO",32,2,C_CYAN);
    glowText("NOVABOY",56,3,C_NEON_B);
    neonLine(98,C_NEON_B);
    cText("15 Games  Inside!",104,1,C_WHITE);
    neonPanel(8,118,224,70,C_CYAN);
    tft.setTextSize(1); tft.setTextColor(C_CYAN);
    tft.setCursor(16,126); tft.print("CONTROLS:");
    tft.setTextColor(C_WHITE);
    tft.setCursor(16,140); tft.print("D-PAD  = Move / Navigate");
    tft.setCursor(16,154); tft.print("A      = Confirm / Action");
    tft.setCursor(16,168); tft.print("B      = Back / Pause");
    tft.fillRect(0,198,240,14,tft.color565(0,0,0));
    cText("Press A to continue",202,1,C_GREEN);
    while(!(btnPressed()&BTN_A)) vTaskDelay(1); sfx_select();

    tft.fillScreen(C_BG);
    glowText("NAVIGATION",26,2,C_YELLOW); neonLine(54,C_YELLOW);
    neonPanel(8,62,224,122,C_YELLOW);
    cText("HOME SCREEN",68,1,C_WHITE);
    tft.drawFastHLine(34,80,172,tft.color565(30,30,50));
    cText("A = Enter Game Menu",86,1,C_GREEN);
    cText("B = Settings",100,1,C_CYAN);
    tft.drawFastHLine(34,114,172,tft.color565(30,30,50));
    cText("IN GAME MENU",120,1,C_WHITE);
    cText("L/R = Browse games",132,1,C_CYAN);
    cText("A = Select   UP = How To Play",146,1,C_GREEN);
    cText("B = Return Home",160,1,C_YELLOW);
    tft.fillRect(0,196,240,14,tft.color565(0,0,0));
    cText("Press A to start!",200,1,C_GREEN);
    while(!(btnPressed()&BTN_A)) vTaskDelay(1); sfx_select();
    tft.fillScreen(C_BG);
}