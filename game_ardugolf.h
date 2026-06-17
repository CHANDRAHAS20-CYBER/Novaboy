#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace AG {
    static float bx,by,vx,vy,obx,oby;
    static float angle,oAngle;
    static int   power,maxPower;
    static int   par,shots,score,hi,level,frameC;
    static bool  paused,charging,rolling,inHole;
    static FrameTimer ft;

    struct Hole { int hx,hy,r; };
    static Hole hole;
    struct Wall { int x,y,w,h; };
    static const int MAX_WALLS=5;
    static Wall walls[MAX_WALLS];
    static int numWalls;
    static const uint16_t GRASS = 0x0220;

    static float toRad(float d){return d*M_PI/180.0f;}

    static void restoreGrassAt(int x,int y,int w,int h){
        int x1=max(x,0),y1=max(y,22);
        int x2=min(x+w,240),y2=min(y+h,218);
        if(x2<=x1||y2<=y1)return;
        tft.fillRect(x1,y1,x2-x1,y2-y1,GRASS);
        // Redraw walls that overlap
        for(int i=0;i<numWalls;i++){
            Wall& w2=walls[i];
            if(x1<w2.x+w2.w&&x2>w2.x&&y1<w2.y+w2.h&&y2>w2.y){
                tft.fillRect(w2.x,w2.y,w2.w,w2.h,tft.color565(210,180,90));
                tft.drawRect(w2.x,w2.y,w2.w,w2.h,tft.color565(160,130,60));
            }
        }
        // Redraw hole if overlaps
        if(x1<hole.hx+hole.r&&x2>hole.hx-hole.r&&y1<hole.hy+hole.r&&y2>hole.hy-hole.r){
            tft.fillCircle(hole.hx,hole.hy,hole.r,tft.color565(0,0,0));
            tft.drawCircle(hole.hx,hole.hy,hole.r,tft.color565(60,60,60));
            tft.fillRect(hole.hx,hole.hy-26,2,26,C_WHITE);
            tft.fillRect(hole.hx+2,hole.hy-26,14,9,C_RED);
        }
    }

    static void eraseArrow(){
        if(oAngle>-900){
            float ax=obx+cosf(toRad(oAngle))*26;
            float ay=oby+sinf(toRad(oAngle))*26;
            int x1=(int)min(obx,ax)-4, y1=(int)min(oby,ay)-4;
            int x2=(int)max(obx,ax)+4, y2=(int)max(oby,ay)+4;
            restoreGrassAt(x1,y1,x2-x1+1,y2-y1+1);
            oAngle=-999;
        }
    }

    static void drawArrow(){
        float ax=bx+cosf(toRad(angle))*24;
        float ay=by+sinf(toRad(angle))*24;
        tft.drawLine((int)bx,(int)by,(int)ax,(int)ay,C_YELLOW);
        tft.fillCircle((int)ax,(int)ay,3,C_YELLOW);
        oAngle=angle;
        obx=bx; oby=by;
    }

    static void eraseBall(){
        restoreGrassAt((int)bx-8,(int)by-8,18,18);
    }

    static void drawBall(){
        tft.fillCircle((int)bx,(int)by,5,C_WHITE);
        tft.drawCircle((int)bx,(int)by,5,tft.color565(180,180,180));
    }

    static void genHole(){
        hole.hx=40+random(160); hole.hy=60+random(120); hole.r=8;
        bx=120; by=190; obx=bx; oby=by;
        vx=0; vy=0; angle=270.0f; oAngle=-999;
        power=0; charging=false; rolling=false; inHole=false;
        numWalls=min(level/2,MAX_WALLS);
        for(int i=0;i<numWalls;i++){
            if(random(2)){walls[i]={20+random(180),50+random(120),25+random(60),7};}
            else         {walls[i]={20+random(200),50+random(100),7,25+random(50)};}
        }
        par=2+random(3); shots=0;
    }

    static void drawScene(){
        tft.fillScreen(C_BG);
        tft.fillRect(0,22,240,196,GRASS);
        tft.drawRect(0,22,240,196,tft.color565(0,60,0));
        tft.fillCircle(hole.hx,hole.hy,hole.r,tft.color565(0,0,0));
        tft.drawCircle(hole.hx,hole.hy,hole.r,tft.color565(60,60,60));
        tft.fillRect(hole.hx,hole.hy-26,2,26,C_WHITE);
        tft.fillRect(hole.hx+2,hole.hy-26,14,9,C_RED);
        tft.fillRect(hole.hx+3,hole.hy-25,12,4,C_WHITE);
        for(int i=0;i<numWalls;i++){
            tft.fillRect(walls[i].x,walls[i].y,walls[i].w,walls[i].h,tft.color565(210,180,90));
            tft.drawRect(walls[i].x,walls[i].y,walls[i].w,walls[i].h,tft.color565(160,130,60));
        }
        drawBall();
        if(!rolling) drawArrow();
    }

    static void drawHUD(){
        statusBar("ARDU GOLF",score,hi);
        tft.fillRect(0,218,240,22,tft.color565(0,0,0));
        char lb[40]; snprintf(lb,40,"HOLE %d  PAR %d  SHOTS %d",level,par,shots);
        cText(lb,220,1,C_YELLOW);
        if(rolling) cText("Rolling...",230,1,C_WHITE);
        else if(charging) cText("Release A to shoot!",230,1,C_GREEN);
        else cText("L/R=Aim  Hold A=Power",230,1,tft.color565(55,65,100));
        tft.fillRect(4,210,232,6,tft.color565(8,8,8));
        tft.drawRect(4,210,232,6,tft.color565(30,40,60));
        if(charging){
            int pw=(power*232)/maxPower;
            uint16_t pc=power<maxPower/2?C_GREEN:(power<maxPower*3/4?C_YELLOW:C_RED);
            if(pw>0) tft.fillRect(4,210,pw,6,pc);
        }
    }

    static bool hitWall(){
        for(int i=0;i<numWalls;i++){
            Wall& w=walls[i];
            if(bx>=w.x&&bx<=w.x+w.w&&by>=w.y&&by<=w.y+w.h){
                if(bx<w.x+5||bx>w.x+w.w-5) vx=-vx*0.65f;
                else vy=-vy*0.65f;
                sfx_tap(); return true;
            }
        }
        return false;
    }
}

void arduGolfInit(int lv){
    using namespace AG;
    level=lv; score=0; hi=hiScore[3]; paused=false; frameC=0;
    maxPower=50; ft=FrameTimer(28);
    genHole();
    tft.fillScreen(C_BG);
    drawScene(); drawHUD();
}

void arduGolfLoop(){
    using namespace AG;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawScene();drawHUD();}
        else if(r==2){savedLevel[3]=level;saveData();arduGolfInit(1);}
        else{savedLevel[3]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed(); uint8_t h=btnHeld();
    if(p&BTN_B&&!rolling){paused=true;return;}

    if(!rolling&&!inHole){
        bool changed=false;
        if(h&BTN_LEFT) {angle-=2.5f;changed=true;}
        if(h&BTN_RIGHT){angle+=2.5f;changed=true;}
        if(h&BTN_A){charging=true;power=min(power+2,maxPower);changed=true;}
        if(!(h&BTN_A)&&charging){
            vx=cosf(toRad(angle))*(power*0.14f);
            vy=sinf(toRad(angle))*(power*0.14f);
            power=0; charging=false; rolling=true; shots++;
            eraseArrow();
            sfx_shoot();
        }
        if(changed){
            eraseArrow();
            drawArrow();
            drawBall();
            drawHUD();
        }
    }

    if(rolling){
        // Erase ball at old position
        restoreGrassAt((int)bx-8,(int)by-8,18,18);

        bx+=vx; by+=vy;
        vx*=0.97f; vy*=0.97f;
        if(bx<5){bx=5;vx=-vx*0.8f;}
        if(bx>235){bx=235;vx=-vx*0.8f;}
        if(by<27){by=27;vy=-vy*0.8f;}
        if(by>213){by=213;vy=-vy*0.8f;}
        hitWall();

        float dx2=bx-hole.hx, dy2=by-hole.hy;
        if(dx2*dx2+dy2*dy2<(float)(hole.r*hole.r)){
            inHole=true; rolling=false;
            sfx_success(); vibroPulse(120,100);
            int gain=max(0,(par-shots+5)*20)+level*10;
            score+=gain;
            if(score>hi){hi=score;hiScore[3]=hi;}
            char rb[30]; snprintf(rb,30,"In %d shots  +%d pts",shots,gain);
            cText(rb,150,1,gain>50?C_YELLOW:C_WHITE);
            delay(1500);
            level++; if(level>15)level=15;
            showLevelUp(level,tft.color565(0,200,80));
            genHole(); tft.fillScreen(C_BG); drawScene(); drawHUD(); return;
        }
        if(fabsf(vx)<0.07f&&fabsf(vy)<0.07f){
            vx=0;vy=0;rolling=false;
            if(shots>=par+3){
                sfx_wrong(); score=max(0,score-20); delay(700);
                level++; if(level>15)level=15;
                genHole(); tft.fillScreen(C_BG); drawScene(); drawHUD(); return;
            }
            drawArrow();
        }
        drawBall();
        if(frameC%3==0) drawHUD();
    }
    statusBar("ARDU GOLF",score,hi);
}