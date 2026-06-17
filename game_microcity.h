#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace MC {
    static const int GW=10, GH=8;
    static const int TW=20, TH=20, OX=20, OY=24;
    enum ZoneType { Z_EMPTY=0, Z_RES, Z_COM, Z_IND, Z_PARK, Z_ROAD };
    static uint8_t grid[GH][GW];
    static int cx,cy,ocx,ocy,score,hi,level,pop,money,frameC;
    static bool paused;
    static FrameTimer ft;
    static int zoneMode;

    static uint16_t zCol(int z){
        switch(z){
            case Z_RES:  return tft.color565(0,200,80);
            case Z_COM:  return tft.color565(0,180,255);
            case Z_IND:  return tft.color565(255,140,0);
            case Z_PARK: return tft.color565(0,120,40);
            case Z_ROAD: return tft.color565(120,120,130);
        }
        return C_BG;
    }

    static void drawCell(int gx,int gy){
        int px2=OX+gx*TW, py2=OY+gy*TH;
        uint8_t z=grid[gy][gx];
        bool isCur=(gx==cx&&gy==cy);
        uint16_t bg=z==Z_EMPTY?tft.color565(6,10,24):zCol(z);
        tft.fillRect(px2,py2,TW-1,TH-1,bg);
        tft.drawRect(px2,py2,TW-1,TH-1,isCur?C_WHITE:tft.color565(16,22,44));
        if(isCur) tft.drawRect(px2+1,py2+1,TW-3,TH-3,tft.color565(80,100,140));
        if(z!=Z_EMPTY){
            tft.setTextSize(1);
            tft.setTextColor(tft.color565(0,0,0));
            const char* lbl[]={"","R","C","I","P","="};
            int lw=strlen(lbl[z])*6;
            tft.setCursor(px2+(TW-1-lw)/2,py2+6); tft.print(lbl[z]);
        }
    }

    static void drawHUD(){
        statusBar("MICRO CITY",score,hi);
        tft.fillRect(0,188,240,14,tft.color565(0,0,0));
        char pb[40]; snprintf(pb,40,"POP:%d  $%d  LV%d",pop,money,level);
        cText(pb,190,1,C_YELLOW);
        tft.fillRect(0,204,240,18,tft.color565(0,0,0));
        const char* zl[]={"RES","COM","IND","PRK","RD"};
        uint16_t zc[]={tft.color565(0,200,80),tft.color565(0,180,255),tft.color565(255,140,0),tft.color565(0,120,40),tft.color565(120,120,130)};
        for(int i=0;i<5;i++){
            bool s=(i==zoneMode); int bx=2+i*47;
            tft.fillRoundRect(bx,205,44,14,3,s?zc[i]:tft.color565(8,10,24));
            tft.drawRoundRect(bx,205,44,14,3,zc[i]);
            tft.setTextSize(1); tft.setTextColor(s?tft.color565(0,0,0):zc[i]);
            int lw=strlen(zl[i])*6; tft.setCursor(bx+(44-lw)/2,208); tft.print(zl[i]);
        }
        tft.fillRect(0,222,240,10,tft.color565(0,0,0));
        cText("DPAD=Move A=Place L/R=Zone B=Menu",224,1,tft.color565(28,36,62));
    }

    static int countZone(int z){ int c=0; for(int y=0;y<GH;y++) for(int x=0;x<GW;x++) if(grid[y][x]==z)c++; return c; }

    static void simulate(){
        int r=countZone(Z_RES), c2=countZone(Z_COM), ind=countZone(Z_IND), pk=countZone(Z_PARK);
        int newPop=r*10+min(r,c2)*5+pk*8-ind*3; if(newPop<0)newPop=0;
        if(newPop>pop) sfx_coin();
        pop=newPop; money+=c2*2+ind*3-r; if(money<0)money=0;
        score=pop*10+money;
        if(score>hi){hi=score;hiScore[1]=hi;}
        if(score>=(level*1000)){ level++;if(level>15)level=15; showLevelUp(level,C_GREEN); tft.fillScreen(C_BG); for(int y=0;y<GH;y++)for(int x=0;x<GW;x++)drawCell(x,y); drawHUD(); }
    }
}

void microCityInit(int lv){
    using namespace MC;
    level=lv; score=0; hi=hiScore[1]; pop=0; money=200+lv*50;
    cx=0; cy=0; ocx=0; ocy=0; zoneMode=0; paused=false; frameC=0;
    ft=FrameTimer(80);
    memset(grid,0,sizeof(grid));
    for(int x=0;x<GW;x++) grid[GH/2][x]=Z_ROAD;
    for(int y=0;y<GH;y++) grid[y][GW/2]=Z_ROAD;
    tft.fillScreen(C_BG);
    for(int y=0;y<GH;y++) for(int x=0;x<GW;x++) drawCell(x,y);
    MC::drawHUD();
}

void microCityLoop(){
    using namespace MC;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);for(int y=0;y<GH;y++)for(int x=0;x<GW;x++)drawCell(x,y);drawHUD();}
        else if(r==2){savedLevel[1]=level;saveData();microCityInit(1);}
        else{savedLevel[1]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B){paused=true;return;}

    ocx=cx; ocy=cy;
    bool zoneChanged=false;
    if(p&BTN_LEFT){
        if(cx>0)cx--;
        else{zoneMode=(zoneMode+4)%5;zoneChanged=true;}
    }
    if(p&BTN_RIGHT){
        if(cx<GW-1)cx++;
        else{zoneMode=(zoneMode+1)%5;zoneChanged=true;}
    }
    if(p&BTN_UP    &&cy>0)   cy--;
    if(p&BTN_DOWN  &&cy<GH-1)cy++;

    if(ocx!=cx||ocy!=cy){
        drawCell(ocx,ocy);
        drawCell(cx,cy);
        sfx_tap();
    }
    if(zoneChanged) drawHUD();

    if(p&BTN_A){
        int cost[]={50,80,60,40,20};
        if(money>=cost[zoneMode]){
            grid[cy][cx]=(uint8_t)(zoneMode+1);
            money-=cost[zoneMode];
            drawCell(cx,cy); sfx_ping(); drawHUD();
        } else sfx_wrong();
    }

    if(frameC%75==0){ simulate(); drawHUD(); }
    if(frameC%15==0) statusBar("MICRO CITY",score,hi);
}