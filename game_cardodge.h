#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace AR {
    static int   carX,carY,score,hi,level,lives,frameC;
    static bool  paused;
    static FrameTimer ft;

    static const int PLAY_T=22, PLAY_B=210, PLAY_L=10, PLAY_R=230;
    static const uint16_t ROAD_COL = tft.color565(30,30,35);

    struct Lane { int x; };
    static const int NUM_LANES=4;
    static const int LANES[NUM_LANES]={40,90,145,195};

    struct TrafficCar { int x,y,oy,lane,speed; bool alive; uint16_t col; };
    static const int MAX_TC=8;
    static TrafficCar tc[MAX_TC];

    struct Coin2 { int x,y,oy; bool alive; };
    static const int MAX_COINS=5;
    static Coin2 coins[MAX_COINS];

    // Road stripe state
    static int stripeY[6];

    static void drawRoad(){
        tft.fillRect(PLAY_L,PLAY_T,PLAY_R-PLAY_L,PLAY_B-PLAY_T,tft.color565(30,30,35));
        // Side grass
        tft.fillRect(0,PLAY_T,PLAY_L,PLAY_B-PLAY_T,tft.color565(0,80,0));
        tft.fillRect(PLAY_R,PLAY_T,240-PLAY_R,PLAY_B-PLAY_T,tft.color565(0,80,0));
        // Lane dividers
        for(int i=1;i<NUM_LANES;i++){
            int lx=(LANES[i-1]+LANES[i])/2;
            for(int j=0;j<6;j++)
                tft.fillRect(lx-1,stripeY[j],2,16,tft.color565(200,180,0));
        }
        // Edge lines
        tft.fillRect(PLAY_L,PLAY_T,3,PLAY_B-PLAY_T,tft.color565(200,200,200));
        tft.fillRect(PLAY_R-3,PLAY_T,3,PLAY_B-PLAY_T,tft.color565(200,200,200));
    }

    static void drawPlayerCar(bool erase){
        if(erase){
            tft.fillRect(carX-10,carY-18,22,36,tft.color565(30,30,35));
            return;
        }
        // Top-down car
        tft.fillRoundRect(carX-8,carY-16,17,32,3,C_CYAN);
        tft.fillRect(carX-6,carY-12,14,10,tft.color565(30,80,160));
        tft.fillRect(carX-6,carY+4,14,6,tft.color565(30,80,160));
        tft.fillRect(carX-9,carY-14,3,6,tft.color565(20,20,20));
        tft.fillRect(carX+6,carY-14,3,6,tft.color565(20,20,20));
        tft.fillRect(carX-9,carY+8,3,6,tft.color565(20,20,20));
        tft.fillRect(carX+6,carY+8,3,6,tft.color565(20,20,20));
        tft.fillRect(carX-4,carY-17,9,3,C_YELLOW);
        tft.fillRect(carX-3,carY+14,7,4,(frameC%4<2)?C_RED:tft.color565(150,0,0));
    }

    static void drawTrafficCar(int i, bool erase){
        TrafficCar& c=tc[i];
        int y=erase?c.oy:c.y;
        if(erase){
            tft.fillRect(c.x-10,y-18,22,36,tft.color565(30,30,35));
            return;
        }
        tft.fillRoundRect(c.x-8,c.y-16,17,32,3,c.col);
        tft.fillRect(c.x-6,c.y-12,14,10,tft.color565(0,0,40));
        tft.fillRect(c.x-9,c.y-14,3,6,tft.color565(20,20,20));
        tft.fillRect(c.x+6,c.y-14,3,6,tft.color565(20,20,20));
        tft.fillRect(c.x-9,c.y+8,3,6,tft.color565(20,20,20));
        tft.fillRect(c.x+6,c.y+8,3,6,tft.color565(20,20,20));
        tft.fillRect(c.x-4,c.y+14,9,3,C_RED);
        c.oy=c.y;
    }

    static void drawCoin2(int i, bool erase){
        Coin2& c=coins[i];
        int y=erase?c.oy:c.y;
        if(erase){tft.fillRect(c.x-6,y-6,13,13,tft.color565(30,30,35));return;}
        tft.fillCircle(c.x,c.y,5,C_YELLOW);
        tft.fillCircle(c.x,c.y,3,tft.color565(255,200,0));
        tft.drawCircle(c.x,c.y,5,tft.color565(180,130,0));
        c.oy=c.y;
    }

    static void spawnTC(){
        for(int i=0;i<MAX_TC;i++) if(!tc[i].alive){
            int ln=random(NUM_LANES);
            uint16_t cols[]={C_RED,tft.color565(255,140,0),tft.color565(200,200,0),C_GREEN};
            tc[i]={LANES[ln],PLAY_T+10,PLAY_T+10,ln,2+random(3+level),true,cols[random(4)]};
            break;
        }
    }
    static void spawnCoin(){
        for(int i=0;i<MAX_COINS;i++) if(!coins[i].alive){
            int ln=random(NUM_LANES);
            coins[i]={LANES[ln],PLAY_T+5,PLAY_T+5,true};
            break;
        }
    }

    static void drawHUD(){
        statusBar("CAR DODGE",score,hi);
        tft.fillRect(0,210,240,12,tft.color565(0,0,0));
        char lb[40]; snprintf(lb,40,"LV%d  LIVES:%d",level,lives);
        cText(lb,212,1,C_YELLOW);
        tft.fillRect(0,224,240,8,tft.color565(0,0,0));
        cText("L/R=Change Lane  A=Speed  B=Pause",226,1,tft.color565(28,36,62));
    }
}

void arduRacerInit(int lv){
    using namespace AR;
    level=lv; score=0; hi=hiScore[2]; lives=3; paused=false; frameC=0;
    carX=LANES[1]; carY=PLAY_B-30;
    for(int i=0;i<MAX_TC;i++) tc[i].alive=false;
    for(int i=0;i<MAX_COINS;i++) coins[i].alive=false;
    for(int j=0;j<6;j++) stripeY[j]=PLAY_T+j*32;
    ft=FrameTimer(30);
    tft.fillScreen(C_BG);
    drawRoad(); drawPlayerCar(false); drawHUD();
}

void arduRacerLoop(){
    using namespace AR;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawRoad();
            drawPlayerCar(false);
            for(int i=0;i<MAX_TC;i++) if(tc[i].alive) drawTrafficCar(i,false);
            for(int i=0;i<MAX_COINS;i++) if(coins[i].alive) drawCoin2(i,false);
            drawHUD();}
        else if(r==2){savedLevel[2]=level;saveData();arduRacerInit(1);}
        else{savedLevel[2]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed(); uint8_t h=btnHeld();
    if(p&BTN_B){paused=true;return;}

    // Scroll stripes
    int scrollSpd=3+level;
    bool stripeDirty=false;
    for(int j=0;j<6;j++){
        stripeY[j]+=scrollSpd;
        if(stripeY[j]>PLAY_B){ stripeY[j]=PLAY_T-16; stripeDirty=true; }
    }
    if(stripeDirty||frameC%8==0){
        // Redraw road (fast)
        tft.fillRect(PLAY_L,PLAY_T,PLAY_R-PLAY_L,PLAY_B-PLAY_T,tft.color565(30,30,35));
        for(int i=1;i<NUM_LANES;i++){
            int lx=(LANES[i-1]+LANES[i])/2;
            for(int j=0;j<6;j++) tft.fillRect(lx-1,stripeY[j],2,16,tft.color565(200,180,0));
        }
        tft.fillRect(PLAY_L,PLAY_T,3,PLAY_B-PLAY_T,tft.color565(200,200,200));
        tft.fillRect(PLAY_R-3,PLAY_T,3,PLAY_B-PLAY_T,tft.color565(200,200,200));
    }

    // Player input
    drawPlayerCar(true);
    static int targetLane=1;
    if(p&BTN_LEFT  &&targetLane>0) targetLane--;
    if(p&BTN_RIGHT &&targetLane<NUM_LANES-1) targetLane++;
    int targetX=LANES[targetLane];
    if(carX<targetX-2) carX+=4;
    else if(carX>targetX+2) carX-=4;
    else carX=targetX;

    bool boost=(h&BTN_A)!=0;
    int moveSpd=scrollSpd+(boost?3:0);

    // Move traffic cars
    for(int i=0;i<MAX_TC;i++){
        if(!tc[i].alive)continue;
        drawTrafficCar(i,true);
        tc[i].y+=moveSpd-tc[i].speed;
        if(tc[i].y>PLAY_B+20){tc[i].alive=false;score+=5;continue;}
        // Collision
        if(abs(tc[i].x-carX)<14&&abs(tc[i].y-carY)<28){
            tc[i].alive=false;
            lives--; sfx_hit(); vibroPulse(200,100);
            drawHUD();
            if(lives<=0){
                bool r2=showGameOver(score,hi,score>hiScore[2]);
                if(score>hiScore[2])hiScore[2]=score;
                gamePlayed[2]=true;saveData();
                if(r2)arduRacerInit(1);else{appState=APP_HOME;homeScreen();}return;
            }
            continue;
        }
        drawTrafficCar(i,false);
    }

    // Move coins
    for(int i=0;i<MAX_COINS;i++){
        if(!coins[i].alive)continue;
        drawCoin2(i,true);
        coins[i].y+=moveSpd;
        if(coins[i].y>PLAY_B+10){coins[i].alive=false;continue;}
        if(abs(coins[i].x-carX)<12&&abs(coins[i].y-carY)<16){
            coins[i].alive=false; score+=20; sfx_coin();
            if(score>hi){hi=score;hiScore[2]=hi;}
            drawHUD(); continue;
        }
        drawCoin2(i,false);
    }

    // Spawn
    if(frameC%(max(40-level*2,12))==0) spawnTC();
    if(frameC%60==0) spawnCoin();

    score++;
    if(score>hi){hi=score;hiScore[2]=hi;}
    if(score>=level*500){
        level++;if(level>15)level=15;
        showLevelUp(level,C_RED);
        tft.fillScreen(C_BG);drawRoad();drawHUD();
    }

    drawPlayerCar(false);
    if(frameC%8==0) statusBar("CAR DODGE",score,hi);
}