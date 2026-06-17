#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace CP {
    static const int GW4=6, GH4=6;
    static const int TS2=32, OX4=24, OY4=28;
    static uint8_t grid4[GH4][GW4];
    static uint8_t rot4[GH4][GW4];
    static int  cx3,cy3,score,hi,level,frameC;
    static bool paused,solved2;
    static FrameTimer ft;

    static uint8_t rotatePipe(uint8_t p){ return ((p<<1)|(p>>3))&0xF; }
    static uint8_t effective(int gx,int gy){
        uint8_t p=grid4[gy][gx];
        for(int r=0;r<rot4[gy][gx];r++) p=rotatePipe(p);
        return p;
    }
    static bool connected2(int x1,int y1,int x2,int y2){
        int dx=x2-x1, dy=y2-y1;
        if(dx==0&&dy==-1) return (effective(x1,y1)&1)&&(effective(x2,y2)&4);
        if(dx==1&&dy==0)  return (effective(x1,y1)&2)&&(effective(x2,y2)&8);
        if(dx==0&&dy==1)  return (effective(x1,y1)&4)&&(effective(x2,y2)&1);
        if(dx==-1&&dy==0) return (effective(x1,y1)&8)&&(effective(x2,y2)&2);
        return false;
    }

    static void drawPipe2(int gx,int gy){
        int px6=OX4+gx*TS2, py6=OY4+gy*TS2;
        bool isCur=(gx==cx3&&gy==cy3);
        tft.fillRect(px6,py6,TS2-2,TS2-2,tft.color565(6,10,24));
        tft.drawRect(px6,py6,TS2-2,TS2-2,isCur?C_WHITE:tft.color565(12,16,36));
        if(isCur) tft.drawRect(px6+1,py6+1,TS2-4,TS2-4,tft.color565(40,50,80));
        uint8_t ef=effective(gx,gy);
        if(!ef)return;
        int cx4=px6+TS2/2-1, cy4=py6+TS2/2-1;
        uint16_t pc;
        if(gx==0&&gy==0)         pc=C_GREEN;  // source
        else if(gx==GW4-1&&gy==GH4-1) pc=C_RED; // sink
        else pc=solved2?tft.color565(0,220,80):C_CYAN;
        tft.fillCircle(cx4,cy4,5,pc);
        tft.fillCircle(cx4,cy4,2,C_WHITE);
        if(ef&1){tft.fillRect(cx4-2,py6,5,TS2/2,pc);tft.fillRect(cx4-1,py6+2,3,TS2/2-2,tft.color565(150,200,255));}
        if(ef&2){tft.fillRect(cx4,cy4-2,TS2/2,5,pc);}
        if(ef&4){tft.fillRect(cx4-2,cy4,5,TS2/2,pc);}
        if(ef&8){tft.fillRect(px6,cy4-2,TS2/2,5,pc);}
    }

    static void drawAll4s(){
        tft.fillScreen(C_BG);
        tft.drawRect(OX4-2,OY4-2,GW4*TS2+2,GH4*TS2+2,tft.color565(20,30,60));
        for(int gy=0;gy<GH4;gy++) for(int gx=0;gx<GW4;gx++) drawPipe2(gx,gy);
        statusBar("CUBE PIPES",score,hi);
        tft.fillRect(0,224,240,10,tft.color565(0,0,0));
        char lb[20]; snprintf(lb,20,"LV%d  BEST:%d",level,hi);
        cText(lb,226,1,C_YELLOW);
        tft.fillRect(0,232,240,8,tft.color565(0,0,0));
        cText("DPAD=Move  A=Rotate  B=Pause",234,1,tft.color565(28,36,62));
    }

    static void genPuzzle2(){
        bool onPath[GH4][GW4]={}; uint8_t pathPipe[GH4][GW4]={};
        int x=0,y=0; onPath[y][x]=true;
        while(x<GW4-1||y<GH4-1){
            if(x==GW4-1){pathPipe[y][x]|=4;y++;pathPipe[y][x]|=1;onPath[y][x]=true;}
            else if(y==GH4-1){pathPipe[y][x]|=2;x++;pathPipe[y][x]|=8;onPath[y][x]=true;}
            else if(random(2)){pathPipe[y][x]|=2;x++;pathPipe[y][x]|=8;onPath[y][x]=true;}
            else{pathPipe[y][x]|=4;y++;pathPipe[y][x]|=1;onPath[y][x]=true;}
        }
        for(int gy=0;gy<GH4;gy++) for(int gx=0;gx<GW4;gx++){
            grid4[gy][gx]=onPath[gy][gx]?pathPipe[gy][gx]:(uint8_t)(random(2)?0x05:0x06);
            rot4[gy][gx]=random(4);
        }
        grid4[0][0]=0x06; rot4[0][0]=0;
        grid4[GH4-1][GW4-1]=0x09; rot4[GH4-1][GW4-1]=0;
        solved2=false;
    }

    static bool checkSolved2(){
        bool vis[GH4][GW4]={};
        int qx[GH4*GW4],qy[GH4*GW4],head=0,tail=0;
        qx[tail]=0;qy[tail]=0;tail++;vis[0][0]=true;
        int dx[]={0,1,0,-1},dy[]={-1,0,1,0};
        while(head<tail){
            int x2=qx[head],y2=qy[head];head++;
            for(int d=0;d<4;d++){
                int nx=x2+dx[d],ny=y2+dy[d];
                if(nx<0||nx>=GW4||ny<0||ny>=GH4||vis[ny][nx])continue;
                if(connected2(x2,y2,nx,ny)){vis[ny][nx]=true;qx[tail]=nx;qy[tail]=ny;tail++;}
            }
        }
        return vis[GH4-1][GW4-1];
    }
}

void cubePipesInit(int lv){
    using namespace CP;
    level=lv; score=0; hi=hiScore[13]; cx3=0; cy3=0; paused=false; frameC=0;
    ft=FrameTimer(90);
    genPuzzle2();
    tft.fillScreen(C_BG);
    drawAll4s();
}

void cubePipesLoop(){
    using namespace CP;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawAll4s();}
        else if(r==2){savedLevel[13]=level;saveData();cubePipesInit(1);}
        else{savedLevel[13]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;

    uint8_t p=btnPressed();
    if(!p){vTaskDelay(1);return;}
    if(p&BTN_B){paused=true;return;}

    int ocx=cx3,ocy=cy3;
    if(p&BTN_UP    &&cy3>0)    cy3--;
    if(p&BTN_DOWN  &&cy3<GH4-1)cy3++;
    if(p&BTN_LEFT  &&cx3>0)    cx3--;
    if(p&BTN_RIGHT &&cx3<GW4-1)cx3++;
    if(cx3!=ocx||cy3!=ocy){
        // Only redraw the 2 changed cells - no fillScreen!
        drawPipe2(ocx,ocy);
        drawPipe2(cx3,cy3);
        sfx_tap();
    }

    if(p&BTN_A){
        rot4[cy3][cx3]=(rot4[cy3][cx3]+1)%4;
        sfx_ping();
        drawPipe2(cx3,cy3); // only this cell
        if(checkSolved2()&&!solved2){
            solved2=true;
            // Redraw all to show green
            for(int gy=0;gy<GH4;gy++) for(int gx=0;gx<GW4;gx++) drawPipe2(gx,gy);
            sfx_success();
            score+=100+level*30;
            if(score>hi){hi=score;hiScore[13]=hi;}
            delay(900);
            level++;if(level>15)level=15;
            showLevelUp(level,C_CYAN);
            genPuzzle2(); tft.fillScreen(C_BG); drawAll4s();
        }
    }
}