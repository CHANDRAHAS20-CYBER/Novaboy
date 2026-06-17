#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace RB {
    enum Phase { PH_WAIT, PH_SHOW, PH_RESULT };
    static Phase phase;
    static int   score,hi,level,lives5,streak3,frameC;
    static bool  paused;
    static FrameTimer ft;
    static uint32_t showTime,waitTime,reactionTime;
    static bool  responded,correct3;
    static uint8_t correctBtn;
    static int   waitFrames,showFrames;
    static char  prompt[32];
    static uint16_t promptCol;

    struct Challenge {
        const char* text;
        const char* sub;
        uint8_t btn; // BTN_UP/DOWN/LEFT/RIGHT/A
        uint16_t col;
    };

    static const Challenge CH[]={
        {"UP!","Press UP",BTN_UP,C_CYAN},
        {"DOWN!","Press DOWN",BTN_DOWN,C_GREEN},
        {"LEFT!","Press LEFT",BTN_LEFT,C_YELLOW},
        {"RIGHT!","Press RIGHT",BTN_RIGHT,C_ORANGE},
        {"A!","Press A button",BTN_A,C_PINK},
        {"DODGE!","Press LEFT",BTN_LEFT,C_RED},
        {"GO!","Press UP",BTN_UP,C_NEON_G},
        {"HIT!","Press A button",BTN_A,C_CYAN},
    };
    static const int NUM_CH=8;
    static int curCh;
    static int maxWait; // frames to wait before showing
    static int maxReact; // ms to react

    static void drawWaiting(){
        tft.fillScreen(C_BG);
        statusBar("REACTION BLITZ",score,hi);
        // Draw animated "get ready" with suspense
        tft.fillRoundRect(20,60,200,100,12,tft.color565(6,10,24));
        tft.drawRoundRect(20,60,200,100,12,tft.color565(30,40,70));
        cText("GET READY...",90,2,tft.color565(60,70,100));
        cText("React when prompt appears!",170,1,tft.color565(40,50,80));
        // Lives
        tft.fillRect(0,190,240,14,tft.color565(0,0,0));
        for(int i=0;i<lives5;i++) tft.fillCircle(8+i*16,197,5,C_RED);
        for(int i=lives5;i<3;i++) tft.drawCircle(8+i*16,197,5,tft.color565(50,0,0));
        char lb[20]; snprintf(lb,20,"LV%d  STREAK:%d",level,streak3);
        tft.setTextColor(C_YELLOW); tft.setTextSize(1); tft.setCursor(100,193); tft.print(lb);
        tft.fillRect(0,208,240,10,tft.color565(0,0,0));
        cText("B=Pause",210,1,tft.color565(28,36,62));
    }

    static void drawPrompt(){
        tft.fillScreen(C_BG);
        statusBar("REACTION BLITZ",score,hi);
        const Challenge& c=CH[curCh];
        // Flash background
        tft.fillRect(0,22,240,188,dimColor(c.col));
        tft.fillRoundRect(10,50,220,120,16,tft.color565(0,0,0));
        tft.drawRoundRect(10,50,220,120,16,c.col);
        tft.drawRoundRect(11,51,218,118,15,dimColor(c.col));
        glowText(c.text,70,3,c.col);
        cText(c.sub,130,1,C_WHITE);
        // Timer bar
        uint32_t elapsed=millis()-showTime;
        int barW=(int)(200-(elapsed*200)/maxReact);
        if(barW<0)barW=0;
        tft.fillRect(20,150,200,12,tft.color565(30,0,0));
        tft.drawRect(20,150,200,12,c.col);
        if(barW>0) tft.fillRect(20,150,barW,12,c.col);
        tft.fillRect(0,190,240,14,tft.color565(0,0,0));
        for(int i=0;i<lives5;i++) tft.fillCircle(8+i*16,197,5,C_RED);
        char lb[20]; snprintf(lb,20,"LV%d STREAK:%d",level,streak3);
        tft.setTextColor(C_YELLOW); tft.setTextSize(1); tft.setCursor(100,193); tft.print(lb);
    }

    static void newRound(){
        curCh=random(NUM_CH);
        waitFrames=30+random(max(60-level*3,15)); // random wait
        showFrames=0;
        maxReact=1200-level*50; if(maxReact<300)maxReact=300;
        phase=PH_WAIT;
        responded=false;
        drawWaiting();
    }
}

void binaryTrainerInit(int lv){
    using namespace RB;
    level=lv; score=0; hi=hiScore[14]; lives5=3; streak3=0; paused=false; frameC=0;
    ft=FrameTimer(16); // fast timer for reaction game
    newRound();
}

void binaryTrainerLoop(){
    using namespace RB;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;drawWaiting();}
        else if(r==2){savedLevel[14]=level;saveData();binaryTrainerInit(1);}
        else{savedLevel[14]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B&&phase!=PH_SHOW){paused=true;return;}

    if(phase==PH_WAIT){
        waitFrames--;
        // Animate dots
        if(frameC%8==0){
            int dots=(frameC/8)%4;
            tft.fillRect(80,100,100,20,tft.color565(6,10,24));
            tft.setTextSize(1); tft.setTextColor(tft.color565(60,70,100));
            char ds[16]="WAIT"; for(int d=0;d<dots;d++) ds[4+d]='.'; ds[4+dots]=0;
            cText(ds,102,1,tft.color565(60,70,100));
        }
        // False start penalty
        if(p&&p!=BTN_B){
            sfx_wrong();
            tft.fillRoundRect(20,60,200,100,12,tft.color565(30,0,0));
            tft.drawRoundRect(20,60,200,100,12,C_RED);
            cText("TOO EARLY!",90,2,C_RED);
            cText("Wait for the prompt!",116,1,C_WHITE);
            delay(900);
            lives5--;
            if(lives5<=0){
                bool r2=showGameOver(score,hi,score>hiScore[14]);
                if(score>hiScore[14])hiScore[14]=score;
                gamePlayed[14]=true;saveData();
                if(r2)binaryTrainerInit(1);else{appState=APP_HOME;homeScreen();}return;
            }
            streak3=0;
            newRound(); return;
        }
        if(waitFrames<=0){
            phase=PH_SHOW;
            showTime=millis();
            drawPrompt();
        }
    }
    else if(phase==PH_SHOW){
        uint32_t elapsed=millis()-showTime;
        // Update timer bar every frame
        if(frameC%2==0){
            int barW=(int)(200-(elapsed*200)/maxReact);
            if(barW<0)barW=0;
            tft.fillRect(20,150,200,12,tft.color565(30,0,0));
            tft.drawRect(20,150,200,12,CH[curCh].col);
            if(barW>0) tft.fillRect(20,150,barW,12,CH[curCh].col);
        }
        if(elapsed>(uint32_t)maxReact){
            // Timeout
            sfx_wrong();
            tft.fillScreen(C_BG);
            glowText("TOO SLOW!",90,2,C_RED);
            cText("React faster!",120,1,C_WHITE);
            delay(900);
            lives5--; streak3=0;
            if(lives5<=0){
                bool r2=showGameOver(score,hi,score>hiScore[14]);
                if(score>hiScore[14])hiScore[14]=score;
                gamePlayed[14]=true;saveData();
                if(r2)binaryTrainerInit(1);else{appState=APP_HOME;homeScreen();}return;
            }
            newRound(); return;
        }
        if(p){
            bool hit=(p&CH[curCh].btn)!=0;
            reactionTime=elapsed;
            tft.fillScreen(C_BG);
            statusBar("REACTION BLITZ",score,hi);
            if(hit){
                streak3++;
                int gain=50+level*10+(streak3*5);
                // Faster = more points
                if(reactionTime<200) gain+=50;
                else if(reactionTime<400) gain+=25;
                score+=gain;
                if(score>hi){hi=score;hiScore[14]=hi;}
                sfx_success();
                tft.fillRect(0,22,240,188,tft.color565(0,20,0));
                glowText("PERFECT!",70,2,C_GREEN);
                char rb[30]; snprintf(rb,30,"%dms  +%d pts",(int)reactionTime,gain);
                cText(rb,110,1,C_WHITE);
                if(streak3>=3){
                    char sb[20]; snprintf(sb,20,"STREAK x%d!",streak3);
                    cText(sb,130,1,C_YELLOW);
                }
                delay(700);
                if(score>=(level*400)){level++;if(level>15)level=15;showLevelUp(level,C_CYAN);}
            } else {
                streak3=0; lives5--; sfx_wrong();
                tft.fillRect(0,22,240,188,tft.color565(20,0,0));
                glowText("WRONG!",80,2,C_RED);
                char wb[30]; snprintf(wb,30,"Need: %s",CH[curCh].sub);
                cText(wb,120,1,C_WHITE);
                delay(900);
                if(lives5<=0){
                    bool r2=showGameOver(score,hi,score>hiScore[14]);
                    if(score>hiScore[14])hiScore[14]=score;
                    gamePlayed[14]=true;saveData();
                    if(r2)binaryTrainerInit(1);else{appState=APP_HOME;homeScreen();}return;
                }
            }
            newRound();
        }
    }
}