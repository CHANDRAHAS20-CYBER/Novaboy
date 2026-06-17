#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace PP {
    static float bx5,by5,bvx5,bvy5,obx5,oby5;
    static int   p1Y,p2Y,op1Y,op2Y;
    static int   p1Score,p2Score,score,hi,level,frameC;
    static bool  paused;
    static FrameTimer ft;

    static const int PAD_H=30, PAD_W=6;
    static const int P1X=10, P2X=224;
    static const int FIELD_T=22, FIELD_B=210;
    static const int FIELD_H=FIELD_B-FIELD_T;

    static void drawField(){
        tft.fillRect(0,FIELD_T,240,FIELD_H,tft.color565(2,4,14));
        // Center dashed line
        for(int y=FIELD_T;y<FIELD_B;y+=10){
            tft.fillRect(119,y,2,5,tft.color565(30,40,60));
        }
        tft.drawRect(0,FIELD_T,240,FIELD_H,tft.color565(20,30,60));
    }

    static void erasePad(int x, int y){
        tft.fillRect(x-1,y-2,PAD_W+2,PAD_H+4,tft.color565(2,4,14));
    }
    static void drawPad(int x, int y, uint16_t col){
        tft.fillRoundRect(x,y,PAD_W,PAD_H,3,col);
        tft.fillRoundRect(x+1,y+1,PAD_W-2,6,2,tft.color565(255,255,255));
    }

    static void eraseBall5(){
        tft.fillRect((int)obx5-5,(int)oby5-5,12,12,tft.color565(2,4,14));
        // Restore center line if erased
        if((int)obx5>=117&&(int)obx5<=123){
            for(int y=FIELD_T;y<FIELD_B;y+=10) tft.fillRect(119,y,2,5,tft.color565(30,40,60));
        }
    }
    static void drawBall5(){
        tft.fillCircle((int)bx5,(int)by5,5,C_WHITE);
        tft.fillCircle((int)bx5-1,(int)by5-1,2,tft.color565(200,220,255));
        obx5=bx5; oby5=by5;
    }

    static void resetBall(){
        bx5=120; by5=FIELD_T+FIELD_H/2;
        float ang=(random(60)-30)*M_PI/180.0f;
        float spd=2.5f+level*0.2f;
        bvx5=(random(2)?1:-1)*cosf(ang)*spd;
        bvy5=sinf(ang)*spd;
        obx5=bx5; oby5=by5;
    }

    static void drawHUD6(){
        statusBar("PIXEL PONG",score,hi);
        tft.fillRect(0,210,240,12,tft.color565(0,0,0));
        char lb[40]; snprintf(lb,40,"YOU %d  :  %d CPU    LV%d",p1Score,p2Score,level);
        cText(lb,212,1,C_YELLOW);
        tft.fillRect(0,224,240,10,tft.color565(0,0,0));
        cText("UP/DN=Move  B=Pause",226,1,tft.color565(28,36,62));
    }
}

void catacombsInit(int lv){
    using namespace PP;
    level=lv; score=0; hi=hiScore[6]; p1Score=0; p2Score=0; paused=false; frameC=0;
    ft=FrameTimer(28);
    p1Y=FIELD_T+FIELD_H/2-PAD_H/2;
    p2Y=FIELD_T+FIELD_H/2-PAD_H/2;
    op1Y=p1Y; op2Y=p2Y;
    tft.fillScreen(C_BG);
    drawField();
    resetBall();
    drawPad(P1X,p1Y,C_CYAN);
    drawPad(P2X,p2Y,C_RED);
    drawBall5();
    drawHUD6();
}

void catacombsLoop(){
    using namespace PP;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawField();drawPad(P1X,p1Y,C_CYAN);drawPad(P2X,p2Y,C_RED);drawBall5();drawHUD6();}
        else if(r==2){savedLevel[6]=level;saveData();catacombsInit(1);}
        else{savedLevel[6]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed(); uint8_t h=btnHeld();
    if(p&BTN_B){paused=true;return;}

    // Erase paddles
    erasePad(P1X,op1Y);
    erasePad(P2X,op2Y);
    op1Y=p1Y; op2Y=p2Y;

    // Player 1 movement
    float pspd=3.5f;
    if(h&BTN_UP   &&p1Y>FIELD_T+2)    p1Y-=(int)pspd;
    if(h&BTN_DOWN &&p1Y<FIELD_B-PAD_H-2) p1Y+=(int)pspd;

    // CPU AI
    int ballMid=(int)by5;
    int cpuMid=p2Y+PAD_H/2;
    float aspd=2.0f+level*0.2f; if(aspd>4.5f)aspd=4.5f;
    if(cpuMid<ballMid-3&&p2Y<FIELD_B-PAD_H-2) p2Y+=(int)aspd;
    if(cpuMid>ballMid+3&&p2Y>FIELD_T+2)       p2Y-=(int)aspd;

    // Ball movement
    eraseBall5();
    bx5+=bvx5; by5+=bvy5;

    // Top/bottom bounce
    if(by5<FIELD_T+5){by5=FIELD_T+5;bvy5=-bvy5;sfx_tap();}
    if(by5>FIELD_B-5){by5=FIELD_B-5;bvy5=-bvy5;sfx_tap();}

    // Paddle 1 collision
    if(bx5<P1X+PAD_W+5&&bx5>P1X&&by5>p1Y&&by5<p1Y+PAD_H){
        bvx5=fabsf(bvx5);
        float rel=(by5-(p1Y+PAD_H/2))/(PAD_H/2);
        bvy5=rel*4.0f;
        float spd=sqrtf(bvx5*bvx5+bvy5*bvy5);
        if(spd>7.0f){bvx5=bvx5/spd*7.0f;bvy5=bvy5/spd*7.0f;}
        sfx_hit(); score+=5; if(score>hi){hi=score;hiScore[6]=hi;}
    }
    // Paddle 2 collision
    if(bx5>P2X-5&&bx5<P2X+PAD_W&&by5>p2Y&&by5<p2Y+PAD_H){
        bvx5=-fabsf(bvx5);
        float rel=(by5-(p2Y+PAD_H/2))/(PAD_H/2);
        bvy5=rel*4.0f;
        sfx_hit();
    }

    // Scoring
    if(bx5<3){
        p2Score++; sfx_die();
        drawField();
        if(p2Score>=7){
            bool r2=showGameOver(score,hi,score>hiScore[6]);
            if(score>hiScore[6])hiScore[6]=score;
            gamePlayed[6]=true;saveData();
            if(r2)catacombsInit(1);else{appState=APP_HOME;homeScreen();}return;
        }
        resetBall(); drawHUD6(); delay(600);
    }
    if(bx5>237){
        p1Score++; sfx_success(); score+=20;
        if(score>hi){hi=score;hiScore[6]=hi;}
        drawField();
        if(p1Score>=7){
            level++;if(level>15)level=15;
            showLevelUp(level,C_CYAN);
            p1Score=0;p2Score=0;
            catacombsInit(level); return;
        }
        resetBall(); drawHUD6(); delay(600);
    }

    drawPad(P1X,p1Y,C_CYAN);
    drawPad(P2X,p2Y,C_RED);
    drawBall5();
    if(frameC%10==0) drawHUD6();
}