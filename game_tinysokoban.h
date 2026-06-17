#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace SK {
    static const int GW3=8, GH3=8;
    static const int TS=24, OX3=24, OY3=24;
    enum { S_EMPTY=0, S_WALL, S_TARGET, S_CRATE, S_CRATE_OK };
    static uint8_t grid3[GH3][GW3];
    static int  px5,py5,score,hi,level,moves,frameC;
    static bool paused;
    static FrameTimer ft;

    struct State { uint8_t g[GH3][GW3]; int px2,py2; };
    static const int UNDO_DEPTH=10;
    static State undoStack[UNDO_DEPTH];
    static int   undoTop;

    static const uint8_t LEVELS[15][GH3][GW3]={
        {{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,1},{1,0,0,3,0,0,0,1},{1,0,0,0,0,0,0,1},
         {1,0,0,2,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,1,0,0,0,1},{1,0,0,1,3,2,0,1},{1,0,0,0,0,0,0,1},
         {1,0,0,0,0,0,0,1},{1,0,3,1,0,2,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,1},{1,0,2,0,3,0,0,1},{1,0,0,1,0,0,0,1},
         {1,0,0,1,0,3,0,1},{1,0,2,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,1,0,0,1},{1,0,0,3,1,2,0,1},{1,0,0,0,0,0,0,1},
         {1,1,0,2,0,3,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,2,0,0,2,0,1},{1,0,0,1,1,0,0,1},{1,0,3,0,0,3,0,1},
         {1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,1},{1,0,0,0,3,0,2,1},{1,0,0,1,0,0,0,1},
         {1,0,2,1,3,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,2,0,2,0,0,1},{1,0,0,0,0,0,0,1},{1,0,3,1,3,0,0,1},
         {1,0,0,1,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,1},{1,0,0,3,0,2,0,1},{1,0,3,1,0,0,0,1},
         {1,0,0,0,1,2,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,1,2,0,1},{1,0,0,3,0,0,0,1},{1,0,0,0,1,3,0,1},
         {1,0,0,2,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,2,0,0,1,0,0,1},{1,0,3,0,1,3,2,1},{1,0,0,0,0,0,0,1},
         {1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,2,0,2,0,1},{1,0,0,0,0,0,0,1},{1,3,1,0,0,1,3,1},
         {1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,2,1},{1,0,0,3,1,0,0,1},{1,0,0,0,0,3,0,1},
         {1,2,0,1,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,1},{1,0,2,3,1,3,2,1},{1,0,0,0,1,0,0,1},
         {1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,2,0,1,0,0,2,1},{1,0,0,0,0,0,0,1},{1,3,0,0,0,0,3,1},
         {1,0,0,0,1,0,0,1},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}},
        {{1,1,1,1,1,1,1,1},{1,0,2,0,0,2,0,1},{1,0,0,1,1,0,0,1},{1,3,0,0,0,0,3,1},
         {1,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,1},{1,2,0,0,0,0,2,1},{1,1,1,1,1,1,1,1}}
    };

    static void drawTile3(int gx, int gy){
        int px6=OX3+gx*TS, py6=OY3+gy*TS;
        uint8_t t=grid3[gy][gx];
        tft.fillRect(px6,py6,TS-1,TS-1,tft.color565(6,10,24));
        if(t==S_WALL){
            tft.fillRect(px6,py6,TS-1,TS-1,tft.color565(40,50,80));
            tft.drawRect(px6,py6,TS-1,TS-1,tft.color565(60,80,120));
            // Brick pattern
            tft.drawFastHLine(px6,py6+TS/2,TS-1,tft.color565(50,65,100));
            tft.drawFastVLine(px6+TS/4,py6,TS/2,tft.color565(50,65,100));
            tft.drawFastVLine(px6+TS*3/4,py6+TS/2,TS/2,tft.color565(50,65,100));
        } else if(t==S_TARGET){
            tft.drawRect(px6+3,py6+3,TS-7,TS-7,C_YELLOW);
            tft.drawCircle(px6+TS/2-1,py6+TS/2-1,4,C_YELLOW);
            tft.drawPixel(px6+TS/2-1,py6+TS/2-1,C_YELLOW);
        } else if(t==S_CRATE){
            tft.fillRect(px6+2,py6+2,TS-5,TS-5,tft.color565(100,70,20));
            tft.drawRect(px6+2,py6+2,TS-5,TS-5,tft.color565(160,120,60));
            tft.drawLine(px6+2,py6+2,px6+TS-4,py6+TS-4,tft.color565(130,90,30));
            tft.drawLine(px6+TS-4,py6+2,px6+2,py6+TS-4,tft.color565(130,90,30));
        } else if(t==S_CRATE_OK){
            tft.fillRect(px6+2,py6+2,TS-5,TS-5,tft.color565(0,80,0));
            tft.drawRect(px6+2,py6+2,TS-5,TS-5,C_GREEN);
            // Checkmark
            tft.drawLine(px6+5,py6+11,px6+9,py6+15,C_GREEN);
            tft.drawLine(px6+9,py6+15,px6+17,py6+6,C_GREEN);
            tft.drawLine(px6+5,py6+12,px6+9,py6+16,C_GREEN);
        }
    }

    static void drawPlayer3s(){
        int ppx=OX3+px5*TS, ppy=OY3+py5*TS;
        // Pixel-art warehouse worker
        tft.fillRect(ppx+3,ppy+2,TS-7,TS-5,tft.color565(200,120,50)); // body (orange vest)
        tft.fillCircle(ppx+TS/2-1,ppy+4,4,tft.color565(255,210,160)); // head
        tft.fillRect(ppx+2,ppy,TS-5,4,tft.color565(30,30,200)); // hard hat
        tft.fillRect(ppx+1,ppy,TS-3,2,tft.color565(50,50,220));
        tft.fillRect(ppx+3,ppy+12,5,6,tft.color565(30,50,130)); // legs
        tft.fillRect(ppx+9,ppy+12,4,6,tft.color565(30,50,130));
        tft.fillRect(ppx+2,ppy+17,6,3,tft.color565(20,20,20)); // boots
        tft.fillRect(ppx+8,ppy+17,5,3,tft.color565(20,20,20));
        tft.fillCircle(ppx+4,ppy+8,1,tft.color565(50,30,10)); // eyes
        tft.fillCircle(ppx+9,ppy+8,1,tft.color565(50,30,10));
    }

    static void loadLevel(){
        int l=(level-1)%15;
        memcpy(grid3,LEVELS[l],sizeof(grid3));
        // Find player start (use center area default)
        px5=1; py5=2;
        moves=0; undoTop=0;
    }

    static void drawAll3s(){
        tft.fillScreen(C_BG);
        for(int gy=0;gy<GH3;gy++) for(int gx=0;gx<GW3;gx++) drawTile3(gx,gy);
        drawPlayer3s();
        statusBar("TINY SOKOBAN",score,hi);
        tft.fillRect(0,220,240,12,tft.color565(0,0,0));
        char lb[30]; snprintf(lb,30,"MOVES:%d  LV%d",moves,level);
        cText(lb,222,1,C_YELLOW);
        tft.fillRect(0,232,240,8,tft.color565(0,0,0));
        cText("D-PAD=Push  B=Undo",234,1,tft.color565(28,36,62));
    }

    static bool isSolved3(){
        for(int y=0;y<GH3;y++) for(int x=0;x<GW3;x++) if(grid3[y][x]==S_CRATE) return false;
        return true;
    }

    static void saveUndo2(){
        if(undoTop<UNDO_DEPTH){
            memcpy(undoStack[undoTop].g,grid3,sizeof(grid3));
            undoStack[undoTop].px2=px5; undoStack[undoTop].py2=py5;
            undoTop++;
        }
    }
    static void undo2(){
        if(undoTop>0){undoTop--;memcpy(grid3,undoStack[undoTop].g,sizeof(grid3));px5=undoStack[undoTop].px2;py5=undoStack[undoTop].py2;sfx_back();}
    }
}

void tinySokobanInit(int lv){
    using namespace SK;
    level=lv; score=0; hi=hiScore[8]; paused=false; frameC=0;
    ft=FrameTimer(140);
    loadLevel();
    tft.fillScreen(C_BG);
    drawAll3s();
}

void tinySokobanLoop(){
    using namespace SK;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawAll3s();}
        else if(r==2){savedLevel[8]=level;saveData();tinySokobanInit(1);}
        else{savedLevel[8]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;

    uint8_t p=btnPressed();
    if(!p){vTaskDelay(1);return;}
    if(p&BTN_B){undo2();drawAll3s();return;}

    int dx=0,dy=0;
    if(p&BTN_UP)   dy=-1;
    if(p&BTN_DOWN) dy= 1;
    if(p&BTN_LEFT) dx=-1;
    if(p&BTN_RIGHT)dx= 1;
    if(!dx&&!dy){paused=true;return;}

    int nx=px5+dx, ny=py5+dy;
    if(nx<0||nx>=GW3||ny<0||ny>=GH3)return;
    uint8_t t=grid3[ny][nx];
    if(t==S_WALL)return;

    saveUndo2();
    bool redrawNeeded=false;
    if(t==S_CRATE||t==S_CRATE_OK){
        int cx2=nx+dx, cy2=ny+dy;
        if(cx2<0||cx2>=GW3||cy2<0||cy2>=GH3)return;
        uint8_t ct=grid3[cy2][cx2];
        if(ct==S_WALL||ct==S_CRATE||ct==S_CRATE_OK)return;
        grid3[ny][nx]=(t==S_CRATE_OK)?S_TARGET:S_EMPTY;
        grid3[cy2][cx2]=(ct==S_TARGET)?S_CRATE_OK:S_CRATE;
        sfx_hit(); redrawNeeded=true;
    }

    // Erase player from old pos
    drawTile3(px5,py5);
    px5=nx; py5=ny; moves++;
    sfx_step();

    if(redrawNeeded){
        // Redraw changed tiles
        drawTile3(nx,ny); drawTile3(nx+dx,ny+dy);
    }
    drawPlayer3s();

    tft.fillRect(0,220,240,12,tft.color565(0,0,0));
    char lb[30]; snprintf(lb,30,"MOVES:%d  LV%d",moves,level);
    cText(lb,222,1,C_YELLOW);

    if(isSolved3()){
        sfx_success(); vibroPulse(150,120);
        int gain=200-moves*2+level*20; if(gain<20)gain=20;
        score+=gain;
        if(score>hi){hi=score;hiScore[8]=hi;}
        delay(600);
        level++;if(level>15)level=15;
        showLevelUp(level,C_YELLOW);
        loadLevel(); tft.fillScreen(C_BG); drawAll3s();
    }
}