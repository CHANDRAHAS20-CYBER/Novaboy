#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

void darkDungeonInit(int lv=1);   void microCityInit(int lv=1);
void arduRacerInit(int lv=1);     void arduGolfInit(int lv=1);
void neonSnakeInit(int lv=1);     void microTDInit(int lv=1);
void catacombsInit(int lv=1);     void tankStrikeInit(int lv=1);
void tinySokobanInit(int lv=1);   void omegaHorizonInit(int lv=1);
void bangiInit(int lv=1);         void gloveInit(int lv=1);
void mysticBalloonInit(int lv=1); void cubePipesInit(int lv=1);
void binaryTrainerInit(int lv=1);
void settingsInit(); void otaInit();
void showHowToPlay(int); void showFirstBootIntro();

const char* G_NAME[NUM_GAMES]={
    "DARK DUNGEON","MICRO CITY","CAR DODGE","ARDU GOLF","NEON SNAKE",
    "MICRO TD","PIXEL PONG","TANK STRIKE","TINY SOKOBAN","OMEGA HORIZON",
    "BANGI","STAR SHOOTER","NEON STACKER","CUBE PIPES","REACTION BLITZ"
};
const char* G_TAG[NUM_GAMES]={
    "Roguelike Dungeon","City Builder","Dodge Traffic","Mini Golf","Classic Snake",
    "Tower Defense","2-Player Pong","Top-Down Combat","Block Puzzle","Endless Runner",
    "Arcade Breakout","Space Shooter","Falling Blocks","Pipe Puzzle","Reflex Challenge"
};
const uint16_t G_COL[NUM_GAMES]={
    0x07FF,0x07E0,0xF800,0x3FE0,0x07E0,
    0xFD20,0xF81F,0xFC60,0xFFE0,0xFC00,
    0xF8DF,0x07FF,0xF81F,0xFBE0,0x07FF
};
const AppState G_STATE[NUM_GAMES]={
    APP_G0,APP_G1,APP_G2,APP_G3,APP_G4,
    APP_G5,APP_G6,APP_G7,APP_G8,APP_G9,
    APP_G10,APP_G11,APP_G12,APP_G13,APP_G14
};
const uint8_t G_DIFF[NUM_GAMES] ={4,2,3,2,3,4,2,3,4,4,3,3,2,3,2};
const uint8_t G_STARS[NUM_GAMES]={5,4,5,3,4,5,4,5,4,5,4,5,3,4,3};

static int mnSel=0;

static void mnDiff(int x,int y,int d){
    for(int i=0;i<5;i++) tft.fillCircle(x+i*9,y,3,i<d?C_YELLOW:tft.color565(18,22,40));
}

static void mnCard(int idx,int bx,int by,int w,int h,bool sel){
    uint16_t col=G_COL[idx], dcol=dimColor(col);
    tft.fillRoundRect(bx,by,w,h,8,sel?tft.color565(8,12,30):tft.color565(4,6,16));
    tft.drawRoundRect(bx,by,w,h,8,sel?col:dcol);
    if(sel){tft.drawRoundRect(bx+1,by+1,w-2,h-2,7,dimColor(col));tft.fillRect(bx,by+10,3,h-20,col);}
    tft.fillRoundRect(bx,by,w,20,8,col); tft.fillRect(bx,by+12,w,8,col);
    tft.setTextSize(1); tft.setTextColor(tft.color565(0,0,0));
    char nb[4]; snprintf(nb,4,"%02d",idx+1);
    tft.setCursor(bx+4,by+6); tft.print(nb);
    int nw=strlen(G_NAME[idx])*6; if(nw>w-28)nw=w-28;
    tft.setCursor(bx+(w-nw)/2,by+6); tft.print(G_NAME[idx]);
    if(!sel){
        tft.setTextSize(1); tft.setTextColor(dcol);
        int tw=strlen(G_TAG[idx])*6; if(tw>w-8)tw=w-8;
        tft.setCursor(bx+(w-tw)/2,by+26); tft.print(G_TAG[idx]); return;
    }
    int cy=by+26;
    tft.setTextSize(1); tft.setTextColor(tft.color565(100,110,150));
    int tw=strlen(G_TAG[idx])*6;
    tft.setCursor(bx+(w-tw)/2,cy); cy+=14; tft.print(G_TAG[idx]);
    tft.setTextColor(tft.color565(70,80,110));
    tft.setCursor(bx+6,cy); tft.print("DIFF"); mnDiff(bx+36,cy+3,G_DIFF[idx]); cy+=14;
    tft.setCursor(bx+6,cy); tft.print("HYPE");
    for(int i=0;i<G_STARS[idx];i++) tft.fillCircle(bx+36+i*8,cy+3,3,C_YELLOW);
    for(int i=G_STARS[idx];i<5;i++) tft.drawCircle(bx+36+i*8,cy+3,3,tft.color565(22,28,45));
    cy+=14;
    if(G_STARS[idx]==5){tft.fillRoundRect(bx+w-34,by+22,30,12,4,C_RED);tft.setTextColor(C_WHITE);tft.setCursor(bx+w-31,by+25);tft.print("HOT");}
    tft.fillRoundRect(bx+6,cy,w-12,16,4,tft.color565(0,16,0));
    tft.drawRoundRect(bx+6,cy,w-12,16,4,tft.color565(0,60,0));
    tft.setTextColor(C_GREEN);
    char hb[20]; snprintf(hb,20,"BEST %d",hiScore[idx]);
    int hw=strlen(hb)*6; tft.setCursor(bx+(w-hw)/2,cy+4); tft.print(hb); cy+=20;
    if(savedLevel[idx]>0){
        tft.fillRoundRect(bx+6,cy,w-12,14,4,tft.color565(0,18,38));
        tft.drawRoundRect(bx+6,cy,w-12,14,4,C_CYAN);
        tft.setTextColor(C_CYAN);
        char lb[22]; snprintf(lb,22,"SAVED LV %d",savedLevel[idx]);
        int lw=strlen(lb)*6; tft.setCursor(bx+(w-lw)/2,cy+3); tft.print(lb);
    } else if(!gamePlayed[idx]){
        tft.fillRoundRect(bx+6,cy,32,14,4,C_NEON_P);
        tft.setTextColor(C_WHITE); tft.setCursor(bx+9,cy+3); tft.print("NEW!");
    }
    cy+=18;
    int dotW=NUM_GAMES*10, dotX=bx+(w-dotW)/2;
    for(int i=0;i<NUM_GAMES;i++){
        int dx=dotX+i*10+4;
        if(i==idx) tft.fillRoundRect(dx-4,cy,10,5,2,col);
        else tft.fillCircle(dx,cy+2,2,tft.color565(20,26,44));
    }
    cy+=12;
    int pbx=bx+(w-100)/2;
    tft.fillRoundRect(pbx,cy,100,22,7,col);
    tft.drawRoundRect(pbx,cy,100,22,7,C_WHITE);
    tft.fillTriangle(pbx+8,cy+5,pbx+8,cy+17,pbx+18,cy+11,C_BG);
    tft.setTextColor(C_BG); tft.setTextSize(1);
    tft.setCursor(pbx+22,cy+7); tft.print("A = OPEN MENU");
}

static void mnHeader(){
    tft.fillRect(0,0,240,24,tft.color565(0,0,0));
    tft.drawFastHLine(0,24,240,G_COL[mnSel]);
    tft.setTextSize(1); tft.setTextColor(C_CYAN);
    tft.setCursor(4,8); tft.print("NOVABOY");
    tft.drawFastVLine(62,4,16,tft.color565(24,36,64));
    tft.setTextColor(tft.color565(55,65,105));
    char cnt[16]; snprintf(cnt,16,"GAME %02d / %02d",mnSel+1,NUM_GAMES);
    tft.setCursor(68,8); tft.print(cnt);
    tft.setTextColor(tft.color565(32,40,70));
    tft.setCursor(172,8); tft.print("[< / >]");
}

static void mnFooter(){
    tft.fillRect(0,226,240,14,tft.color565(0,0,0));
    cText("L/R=Browse  UP=Guide  B=Home",228,1,tft.color565(32,42,70));
}

static void mnFullRedraw(){
    tft.fillScreen(C_BG);
    srand(888);
    for(int i=0;i<40;i++){
        int x=rand()%240,y=26+rand()%200;
        uint8_t b=16+rand()%100;
        tft.drawPixel(x,y,tft.color565(b>>1,b>>1,b));
    }
    int prev=(mnSel+NUM_GAMES-1)%NUM_GAMES;
    int next=(mnSel+1)%NUM_GAMES;
    mnCard(prev,4,34,110,190,false);
    mnCard(next,126,34,110,190,false);
    mnCard(mnSel,18,26,204,200,true);
    mnHeader(); mnFooter();
}

static void mnSwap(int newSel){
    mnSel=newSel;
    tft.fillRect(4,26,236,200,C_BG);
    srand(888);
    for(int i=0;i<40;i++){
        int x=rand()%240,y=26+rand()%200;
        uint8_t b=16+rand()%100;
        tft.drawPixel(x,y,tft.color565(b>>1,b>>1,b));
    }
    int prev=(mnSel+NUM_GAMES-1)%NUM_GAMES;
    int next=(mnSel+1)%NUM_GAMES;
    mnCard(prev,4,34,110,190,false);
    mnCard(next,126,34,110,190,false);
    mnCard(mnSel,18,26,204,200,true);
    mnHeader();
}

void menuInit(){ mnFullRedraw(); appState=APP_MENU; }

void menuLoop(){
    uint8_t p=btnPressed();
    if(!p){vTaskDelay(1);return;}
    if(p&BTN_LEFT) {sfx_ui();mnSwap((mnSel+NUM_GAMES-1)%NUM_GAMES);}
    if(p&BTN_RIGHT){sfx_ui();mnSwap((mnSel+1)%NUM_GAMES);}
    if(p&BTN_DOWN) {sfx_ui();mnSwap((mnSel+1)%NUM_GAMES);}
    if(p&BTN_UP)   {sfx_select();showHowToPlay(mnSel);}
    if(p&BTN_A)    {sfx_select();showGameMenu(mnSel);}
    if(p&BTN_B)    {sfx_back();appState=APP_HOME;homeScreen();}
}

static int gmSel=0;

static void gmDraw(){
    int idx=gameMenuSel;
    uint16_t col=G_COL[idx], dcol=dimColor(col);
    bool hasSave=(savedLevel[idx]>0);
    tft.fillScreen(C_BG);
    tft.fillRect(0,0,240,30,tft.color565(0,0,0));
    tft.drawFastHLine(0,30,240,col); tft.drawFastHLine(0,31,240,dcol);
    tft.setTextSize(1); tft.setTextColor(col);
    tft.setCursor(4,8); tft.print(G_NAME[idx]);
    tft.setTextColor(tft.color565(40,50,80));
    tft.setCursor(4,18); tft.print(G_TAG[idx]);
    tft.setTextColor(C_YELLOW);
    char hb[16]; snprintf(hb,16,"BEST %d",hiScore[idx]);
    int hw=strlen(hb)*6; tft.setCursor(236-hw,8); tft.print(hb);

    const char* labels[4]={"NEW GAME","CONTINUE","HIGH SCORE","HOW TO PLAY"};
    uint16_t icols[4]={C_GREEN,C_CYAN,C_YELLOW,C_ORANGE};
    for(int i=0;i<4;i++){
        bool s=(i==gmSel), dis=(i==1&&!hasSave);
        int by=36+i*48;
        uint16_t ic=dis?tft.color565(28,32,50):icols[i];
        tft.fillRoundRect(8,by,224,42,8,s?tft.color565(8,12,32):tft.color565(4,6,16));
        tft.drawRoundRect(8,by,224,42,8,s?ic:tft.color565(20,26,48));
        if(s){tft.fillRect(8,by+9,4,24,ic);tft.fillRect(228,by+9,4,24,ic);tft.drawRoundRect(9,by+1,222,40,7,dimColor(ic));}
        tft.fillRoundRect(14,by+8,28,26,5,s?dimColor(ic):tft.color565(10,14,28));
        tft.drawRoundRect(14,by+8,28,26,5,s?ic:tft.color565(20,26,48));
        tft.setTextSize(2); tft.setTextColor(dis?tft.color565(30,34,55):(s?ic:dimColor(ic)));
        const char* sym[]={">>",">>"," *","??"};
        tft.setCursor(16,by+14); tft.print(sym[i]);
        tft.setTextSize(1); tft.setTextColor(dis?tft.color565(30,34,55):(s?ic:tft.color565(75,85,115)));
        tft.setCursor(48,by+10); tft.print(labels[i]);
        if(i==1&&hasSave){tft.setTextColor(s?C_CYAN:tft.color565(35,55,75));char cb[24];snprintf(cb,24,"Resume Level %d",savedLevel[idx]);tft.setCursor(48,by+22);tft.print(cb);}
        else if(i==1){tft.setTextColor(tft.color565(30,34,55));tft.setCursor(48,by+22);tft.print("No saved progress");}
        else if(i==0){tft.setTextColor(s?tft.color565(0,180,60):tft.color565(24,44,24));tft.setCursor(48,by+22);tft.print("Start from Level 1");}
        else if(i==2){tft.setTextColor(s?C_YELLOW:tft.color565(55,50,18));char hb2[16];snprintf(hb2,16,"Record: %d",hiScore[idx]);tft.setCursor(48,by+22);tft.print(hb2);}
        else{tft.setTextColor(s?C_ORANGE:tft.color565(55,42,16));tft.setCursor(48,by+22);tft.print("Tips & Controls");}
        if(s&&!dis){tft.setTextColor(ic);tft.setCursor(218,by+16);tft.print(">");}
    }
    tft.fillRect(0,230,240,10,tft.color565(0,0,0));
    cText("UP/DN=Sel  A=Confirm  B=Back",232,1,tft.color565(32,42,68));
}

void showGameMenu(int idx){ gameMenuSel=idx; gmSel=0; gmDraw(); appState=APP_GAME_MENU; }

void gameMenuLoop(){
    uint8_t p=btnPressed();
    if(!p){vTaskDelay(1);return;}
    int idx=gameMenuSel; bool hasSave=(savedLevel[idx]>0);
    if(p&BTN_UP)  {gmSel=(gmSel+3)%4;gmDraw();sfx_ui();}
    if(p&BTN_DOWN){gmSel=(gmSel+1)%4;gmDraw();sfx_ui();}
    if(p&BTN_A){
        switch(gmSel){
            case 0: sfx_start(); launchGame(idx,false); break;
            case 1: if(hasSave){sfx_start();launchGame(idx,true);}else sfx_wrong(); break;
            case 2:{
                sfx_score();
                tft.fillRoundRect(28,90,184,62,9,tft.color565(0,0,0));
                tft.drawRoundRect(28,90,184,62,9,C_YELLOW);
                tft.fillRect(28,90,184,24,tft.color565(50,40,0));
                tft.drawFastHLine(28,114,184,C_YELLOW);
                cText("HIGH SCORE",94,1,C_YELLOW);
                tft.setTextSize(2); tft.setTextColor(C_WHITE);
                char hb2[14]; snprintf(hb2,14,"%d",hiScore[idx]);
                int hw2=strlen(hb2)*12; tft.setCursor((240-hw2)/2,120); tft.print(hb2);
                tft.setTextSize(1); tft.setTextColor(tft.color565(70,80,110));
                cText("Press any button",144,1,tft.color565(70,80,110));
                while(!btnPressed())vTaskDelay(1); sfx_ui(); gmDraw(); break;
            }
            case 3: sfx_select(); showHowToPlay(idx); break;
        }
    }
    if(p&BTN_B){sfx_back();appState=APP_MENU;mnFullRedraw();}
}