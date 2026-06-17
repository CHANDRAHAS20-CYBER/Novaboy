#pragma once

#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace NSTK {
    static const int GW=9, GH=13, CS=14;
    static const int OX=(240-GW*CS)/2, OY=24;

    static uint8_t grid[GH][GW];
    static uint8_t curShape[4][4];
    static int pieceType, pieceX, pieceY;
    static int score,hi,level,totalLines,frameC,dropCounter;
    static bool paused;
    static FrameTimer ft;

    static const uint8_t PIECE_SHAPE[7][4][4] = {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // I
        {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, // O
        {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // T
        {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // S
        {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}, // Z
        {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // J
        {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}  // L
    };

    static uint16_t pieceColor(int t){
        switch(t){
            case 0: return C_CYAN;
            case 1: return C_YELLOW;
            case 2: return C_MAGENTA;
            case 3: return C_GREEN;
            case 4: return C_RED;
            case 5: return tft.color565(40,90,255);
            case 6: return C_ORANGE;
        }
        return C_WHITE;
    }

    static void rotateCW(uint8_t s[4][4], uint8_t d[4][4]){
        for(int y=0;y<4;y++) for(int x=0;x<4;x++) d[x][3-y]=s[y][x];
    }

    static bool fits(uint8_t s[4][4], int px, int py){
        for(int sy=0;sy<4;sy++) for(int sx=0;sx<4;sx++){
            if(!s[sy][sx]) continue;
            int bx=px+sx, by=py+sy;
            if(bx<0||bx>=GW||by>=GH) return false;
            if(by>=0 && grid[by][bx]) return false;
        }
        return true;
    }

    // Every cell is always fully redrawn (CS-1 x CS-1) — there's no partial
    // erase here, so there's no way for trailing pixels to be left behind.
    static void drawCellAt(int gx,int gy){
        int x=OX+gx*CS, y=OY+gy*CS;
        uint8_t v=grid[gy][gx];
        if(!v){
            tft.fillRect(x,y,CS-1,CS-1,tft.color565(6,10,20));
            tft.drawRect(x,y,CS-1,CS-1,tft.color565(14,18,34));
            return;
        }
        uint16_t col=pieceColor(v-1);
        tft.fillRect(x,y,CS-1,CS-1,col);
        tft.drawRect(x,y,CS-1,CS-1,tft.color565(0,0,0));
        tft.fillRect(x+1,y+1,CS-3,2,tft.color565(255,255,255));
    }

    static void drawBoard(){
        for(int gy=0;gy<GH;gy++) for(int gx=0;gx<GW;gx++) drawCellAt(gx,gy);
    }

    static void drawBorder(){
        tft.drawRect(OX-2,OY-2,GW*CS+3,GH*CS+3,tft.color565(20,30,60));
    }

    static void drawPieceCells(bool erase){
        for(int sy=0;sy<4;sy++) for(int sx=0;sx<4;sx++){
            if(!curShape[sy][sx]) continue;
            int bx=pieceX+sx, by=pieceY+sy;
            if(bx<0||bx>=GW||by<0||by>=GH) continue;
            if(erase){ drawCellAt(bx,by); continue; }
            int x=OX+bx*CS, y=OY+by*CS;
            tft.fillRect(x,y,CS-1,CS-1,pieceColor(pieceType));
            tft.drawRect(x,y,CS-1,CS-1,C_WHITE);
            tft.fillRect(x+1,y+1,CS-3,2,tft.color565(255,255,255));
        }
    }

    static void spawnPiece(){
        pieceType=random(7);
        memcpy(curShape,PIECE_SHAPE[pieceType],sizeof(curShape));
        pieceX=(GW-4)/2;
        pieceY=0;
    }

    static bool tryMove(int dx,int dy){
        int nx=pieceX+dx, ny=pieceY+dy;
        if(!fits(curShape,nx,ny)) return false;
        drawPieceCells(true);
        pieceX=nx; pieceY=ny;
        drawPieceCells(false);
        return true;
    }

    static void tryRotate(){
        uint8_t tmp[4][4];
        rotateCW(curShape,tmp);
        if(fits(tmp,pieceX,pieceY)){
            drawPieceCells(true); memcpy(curShape,tmp,sizeof(tmp)); drawPieceCells(false); sfx_ping(); return;
        }
        if(fits(tmp,pieceX-1,pieceY)){
            drawPieceCells(true); memcpy(curShape,tmp,sizeof(tmp)); pieceX-=1; drawPieceCells(false); sfx_ping(); return;
        }
        if(fits(tmp,pieceX+1,pieceY)){
            drawPieceCells(true); memcpy(curShape,tmp,sizeof(tmp)); pieceX+=1; drawPieceCells(false); sfx_ping(); return;
        }
        sfx_wrong();
    }

    static void lockPiece(){
        for(int sy=0;sy<4;sy++) for(int sx=0;sx<4;sx++){
            if(!curShape[sy][sx]) continue;
            int bx=pieceX+sx, by=pieceY+sy;
            if(by>=0&&by<GH&&bx>=0&&bx<GW){ grid[by][bx]=pieceType+1; drawCellAt(bx,by); }
        }
        sfx_hit();
    }

    static int clearLines(){
        int cleared=0;
        uint8_t newGrid[GH][GW];
        int writeRow=GH-1;
        for(int gy=GH-1; gy>=0; gy--){
            bool full=true;
            for(int gx=0;gx<GW;gx++) if(!grid[gy][gx]){full=false;break;}
            if(full){ cleared++; continue; }
            for(int gx=0;gx<GW;gx++) newGrid[writeRow][gx]=grid[gy][gx];
            writeRow--;
        }
        for(;writeRow>=0;writeRow--) for(int gx=0;gx<GW;gx++) newGrid[writeRow][gx]=0;
        memcpy(grid,newGrid,sizeof(grid));
        return cleared;
    }

    static void drawHUD(){
        statusBar("NEON STACKER",score,hi);
        tft.fillRect(0,206,240,16,tft.color565(0,0,0));
        char lb[40]; snprintf(lb,40,"LINES:%d  LV%d",totalLines,level);
        cText(lb,208,1,C_YELLOW);
        tft.fillRect(0,222,240,16,tft.color565(0,0,0));
        cText("L/R=Move DN=Drop A=Rotate B=Pause",224,1,tft.color565(28,36,62));
    }
}

void mysticBalloonInit(int lv){
    using namespace NSTK;
    level=lv; score=0; hi=hiScore[12]; totalLines=0; paused=false; frameC=0; dropCounter=0;
    ft=FrameTimer(50);
    memset(grid,0,sizeof(grid));
    tft.fillScreen(C_BG);
    drawBorder();
    drawBoard();
    spawnPiece();
    drawPieceCells(false);
    drawHUD();
}

void mysticBalloonLoop(){
    using namespace NSTK;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawBorder();drawBoard();drawPieceCells(false);drawHUD();}
        else if(r==2){savedLevel[12]=level;saveData();mysticBalloonInit(1);}
        else{savedLevel[12]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B){paused=true;return;}
    if(p&BTN_LEFT)  tryMove(-1,0);
    if(p&BTN_RIGHT) tryMove(1,0);
    if(p&BTN_DOWN)  { if(tryMove(0,1)) score++; }
    if(p&BTN_A)     tryRotate();

    dropCounter++;
    int dropInterval = max(20-level,6);
    if(dropCounter>=dropInterval){
        dropCounter=0;
        if(!tryMove(0,1)){
            lockPiece();
            int n=clearLines();
            if(n>0){
                static const int ADD[5]={0,100,300,500,800};
                score+=ADD[n]*level;
                if(score>hi){hi=score;hiScore[12]=hi;}
                totalLines+=n;
                sfx_score();
                drawBoard();
                if(totalLines>=level*5){
                    level++; if(level>15) level=15;
                    showLevelUp(level,C_PINK);
                    tft.fillScreen(C_BG);drawBorder();drawBoard();
                }
                drawHUD();
            }
            spawnPiece();
            if(!fits(curShape,pieceX,pieceY)){
                bool r2=showGameOver(score,hi,score>hiScore[12]);
                if(score>hiScore[12])hiScore[12]=score;
                gamePlayed[12]=true; saveData();
                if(r2) mysticBalloonInit(1); else {appState=APP_HOME; homeScreen();}
                return;
            }
            drawPieceCells(false);
        }
    }

    if(score>hi){hi=score;hiScore[12]=hi;}
    if(frameC%10==0) statusBar("NEON STACKER",score,hi);
}