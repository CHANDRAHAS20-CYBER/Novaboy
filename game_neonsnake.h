#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace NS {
    static const int GW=20, GH=17;
    static const int TS=11, OX=10, OY=22;

    struct Seg { int x,y; };
    static Seg snake[GW*GH];
    static int  snakeLen, dir, newDir; // 0=R,1=D,2=L,3=U
    static int  foodX,foodY;
    static int  score,hi,level,frameC;
    static bool paused, dead;
    static FrameTimer ft;

    static uint8_t grid[GH][GW]; // 0=empty,1=snake,2=food

    static void placeFood(){
        do{ foodX=random(GW); foodY=random(GH); } while(grid[foodY][foodX]!=0);
        grid[foodY][foodX]=2;
        int px2=OX+foodX*TS, py2=OY+foodY*TS;
        tft.fillRect(px2,py2,TS-1,TS-1,tft.color565(4,6,16));
        tft.fillCircle(px2+5,py2+5,4,tft.color565(255,50,50));
        tft.fillCircle(px2+4,py2+4,2,tft.color565(255,150,150));
        tft.drawCircle(px2+5,py2+5,5,tft.color565(180,0,0));
    }

    static void drawSegment(int idx, bool erase){
        int px2=OX+snake[idx].x*TS, py2=OY+snake[idx].y*TS;
        if(erase){ tft.fillRect(px2,py2,TS-1,TS-1,tft.color565(4,6,16)); return; }
        uint16_t col=(idx==0)?tft.color565(0,255,100):
                     (idx<3)?tft.color565(0,220,80):
                     tft.color565(0,160,50);
        tft.fillRoundRect(px2+1,py2+1,TS-3,TS-3,2,col);
        tft.drawRect(px2,py2,TS-1,TS-1,tft.color565(0,80,30));
        if(idx==0){
            int ex1=px2+4, ey1=py2+3;
            int ex2=px2+4, ey2=py2+7;
            if(dir==0){ex1=px2+7;ey1=py2+2;ex2=px2+7;ey2=py2+6;}
            if(dir==2){ex1=px2+1;ey1=py2+2;ex2=px2+1;ey2=py2+6;}
            if(dir==3){ex1=px2+2;ey1=py2+1;ex2=px2+6;ey2=py2+1;}
            if(dir==1){ex1=px2+2;ey1=py2+7;ex2=px2+6;ey2=py2+7;}
            tft.fillCircle(ex1,ey1,1,C_BG);
            tft.fillCircle(ex2,ey2,1,C_BG);
        }
    }

    static void drawGrid(){
        for(int y=0;y<GH;y++) for(int x=0;x<GW;x++){
            int px2=OX+x*TS, py2=OY+y*TS;
            tft.fillRect(px2,py2,TS-1,TS-1,tft.color565(4,6,16));
            tft.drawRect(px2,py2,TS-1,TS-1,tft.color565(8,10,22));
        }
        tft.drawRect(OX-1,OY-1,GW*TS+1,GH*TS+1,C_NEON_G);
        tft.drawRect(OX-2,OY-2,GW*TS+3,GH*TS+3,dimColor(C_NEON_G));
    }

    static void drawHUD(){
        statusBar("NEON SNAKE",score,hi);
        tft.fillRect(0,212,240,12,tft.color565(0,0,0));
        char lb[30]; snprintf(lb,30,"LENGTH:%d  LV%d",snakeLen,level);
        cText(lb,214,1,C_NEON_G);
        tft.fillRect(0,226,240,14,tft.color565(0,0,0));
        cText("D-PAD=Direction  B=Pause",228,1,tft.color565(28,36,62));
    }
}

void neonSnakeInit(int lv){
    using namespace NS;
    level=lv; score=0; hi=hiScore[4]; paused=false; dead=false; frameC=0;
    ft=FrameTimer(max(120-lv*7,40));
    memset(grid,0,sizeof(grid));
    snakeLen=4; dir=0; newDir=0;
    for(int i=0;i<snakeLen;i++){
        snake[i]={GW/2-i,GH/2};
        grid[GH/2][GW/2-i]=1;
    }
    tft.fillScreen(C_BG);
    drawGrid();
    for(int i=0;i<snakeLen;i++) drawSegment(i,false);
    placeFood();
    drawHUD();
}

void neonSnakeLoop(){
    using namespace NS;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawGrid();for(int i=0;i<snakeLen;i++)drawSegment(i,false);placeFood();drawHUD();}
        else if(r==2){savedLevel[4]=level;saveData();neonSnakeInit(1);}
        else{savedLevel[4]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B){paused=true;return;}
    if((p&BTN_RIGHT)&&dir!=2) newDir=0;
    if((p&BTN_DOWN) &&dir!=3) newDir=1;
    if((p&BTN_LEFT) &&dir!=0) newDir=2;
    if((p&BTN_UP)   &&dir!=1) newDir=3;
    dir=newDir;

    int dx[]={1,0,-1,0}, dy[]={0,1,0,-1};
    int nx=snake[0].x+dx[dir];
    int ny=snake[0].y+dy[dir];

    if(nx<0||nx>=GW||ny<0||ny>=GH){
        sfx_die(); vibroPulse(200,100);
        bool r=showGameOver(score,hi,score>hiScore[4]);
        if(score>hiScore[4]) hiScore[4]=score;
        gamePlayed[4]=true; saveData();
        if(r) neonSnakeInit(1); else{appState=APP_HOME;homeScreen();}
        return;
    }
    if(grid[ny][nx]==1){
        sfx_die(); vibroPulse(200,100);
        bool r=showGameOver(score,hi,score>hiScore[4]);
        if(score>hiScore[4]) hiScore[4]=score;
        gamePlayed[4]=true; saveData();
        if(r) neonSnakeInit(1); else{appState=APP_HOME;homeScreen();}
        return;
    }

    bool ate=(grid[ny][nx]==2);

    if(!ate){
        int tx=snake[snakeLen-1].x, ty=snake[snakeLen-1].y;
        grid[ty][tx]=0;
        drawSegment(snakeLen-1,true);
        snakeLen--;
    }

    for(int i=snakeLen;i>0;i--) snake[i]=snake[i-1];
    snake[0]={nx,ny};
    grid[ny][nx]=1;
    snakeLen++;

    drawSegment(1,false);
    drawSegment(0,false);

    if(ate){
        score+=10*level; sfx_coin();
        if(score>hi){hi=score;hiScore[4]=hi;}
        int threshold=50*level;
        if(score>=threshold*level){level++;if(level>15)level=15;ft=FrameTimer(max(120-level*7,40));showLevelUp(level,C_NEON_G);}
        placeFood();
        drawHUD();
    }
    if(frameC%20==0) statusBar("NEON SNAKE",score,hi);
}