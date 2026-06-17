#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace BG {
    static float bx3,by3,bvx3,bvy3,obx3,oby3;
    static int   padX,oPadX,score,hi,level,misses,frameC;
    static bool  paused;
    static FrameTimer ft;

    static const int BROWS=5, BCOLS=8;
    static int bricks[BROWS][BCOLS];
    static bool bricksDirty=true;

    static uint16_t brickCol2(int hp){
        switch(hp){
            case 3: return C_RED;
            case 2: return C_ORANGE;
            case 1: return tft.color565(0,200,80);
        }
        return C_BG;
    }

    static void drawBrick(int r, int c){
        if(!bricks[r][c]){ tft.fillRect(4+c*29,26+r*18,27,15,C_BG); return; }
        int bx=4+c*29, by=26+r*18;
        tft.fillRect(bx+1,by+1,25,13,brickCol2(bricks[r][c]));
        tft.drawRect(bx,by,27,15,tft.color565(0,0,0));
        tft.drawLine(bx+1,by+1,bx+25,by+1,tft.color565(255,255,255));
        tft.drawLine(bx+1,by+1,bx+1,by+13,tft.color565(200,200,200));
    }

    static void genBricks(){
        for(int r=0;r<BROWS;r++) for(int c=0;c<BCOLS;c++) bricks[r][c]=1+(r<2?2:(r<4?1:0));
        bricksDirty=true;
    }

    static void drawAllBricks(){
        for(int r=0;r<BROWS;r++) for(int c=0;c<BCOLS;c++) drawBrick(r,c);
        bricksDirty=false;
    }

    static void erasePaddle(){ tft.fillRect(oPadX-24,205,50,12,C_BG); }
    static void drawPaddle(){
        tft.fillRoundRect(padX-22,207,44,7,3,tft.color565(0,180,220));
        tft.fillRoundRect(padX-20,207,40,3,2,tft.color565(100,230,255));
        tft.drawRoundRect(padX-22,207,44,7,3,tft.color565(0,100,150));
        tft.fillRect(padX-2,208,4,5,C_WHITE);
        oPadX=padX;
    }

    static void eraseBall3(){
        tft.fillRect((int)obx3-7,(int)oby3-7,16,16,C_BG);
        for(int r=0;r<BROWS;r++) for(int c=0;c<BCOLS;c++){
            int bx=4+c*29, by=26+r*18;
            if((int)obx3+7>=bx&&(int)obx3-7<=bx+27&&(int)oby3+7>=by&&(int)oby3-7<=by+15)
                drawBrick(r,c);
        }
    }
    static void drawBall3(){
        tft.fillCircle((int)bx3,(int)by3,5,C_WHITE);
        tft.fillCircle((int)bx3-1,(int)by3-1,2,tft.color565(200,200,255));
        tft.drawCircle((int)bx3,(int)by3,5,tft.color565(150,150,200));
        obx3=bx3; oby3=by3;
    }

    static void drawHUD4(){
        statusBar("BANGI",score,hi);
        tft.fillRect(0,218,240,12,tft.color565(0,0,0));
        char lb[30]; snprintf(lb,30,"MISSES:%d/3  LV%d",misses,level);
        cText(lb,220,1,C_YELLOW);
        tft.fillRect(0,230,240,10,tft.color565(0,0,0));
        cText("L/R=Paddle  B=Pause",232,1,tft.color565(28,36,62));
    }

    static bool allClear2(){
        for(int r=0;r<BROWS;r++) for(int c=0;c<BCOLS;c++) if(bricks[r][c]) return false;
        return true;
    }
}

void bangiInit(int lv){
    using namespace BG;
    level=lv; score=0; hi=hiScore[10]; misses=0; paused=false; frameC=0;
    padX=120; oPadX=120;
    bx3=120; by3=180; obx3=120; oby3=180;
    bvx3=2.5f+lv*0.18f; bvy3=-(2.5f+lv*0.18f);
    ft=FrameTimer(28);
    genBricks();
    tft.fillScreen(C_BG);
    drawAllBricks(); drawPaddle(); drawBall3(); drawHUD4();
}

void bangiLoop(){
    using namespace BG;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawAllBricks();drawPaddle();drawBall3();drawHUD4();}
        else if(r==2){savedLevel[10]=level;saveData();bangiInit(1);}
        else{savedLevel[10]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t h=btnHeld(); uint8_t p=btnPressed();
    if(p&BTN_B){paused=true;return;}

    erasePaddle();
    eraseBall3();

    if(h&BTN_LEFT  &&padX>26) padX-=5;
    if(h&BTN_RIGHT &&padX<214)padX+=5;

    bx3+=bvx3; by3+=bvy3;
    if(bx3<5){bx3=5;bvx3=-bvx3;}
    if(bx3>235){bx3=235;bvx3=-bvx3;}
    if(by3<26){by3=26;bvy3=-bvy3;}

    if(by3>=204&&by3<=214&&bx3>=padX-24&&bx3<=padX+24){
        bvy3=-fabsf(bvy3);
        bvx3+=(bx3-padX)*0.06f;
        if(bvx3>6)bvx3=6; if(bvx3<-6)bvx3=-6;
        sfx_tap();
    }

    if(by3>222){
        misses++;sfx_wrong();vibroPulse(150,60);
        bx3=padX;by3=190;
        bvx3=(random(2)?1:-1)*(2.5f+level*0.18f);
        bvy3=-(2.5f+level*0.18f);
        drawHUD4();
        if(misses>=3){
            bool r2=showGameOver(score,hi,score>hiScore[10]);
            if(score>hiScore[10])hiScore[10]=score;
            gamePlayed[10]=true;saveData();
            if(r2)bangiInit(1);else{appState=APP_HOME;homeScreen();}return;
        }
    }

    for(int r=0;r<BROWS;r++) for(int c=0;c<BCOLS;c++){
        if(!bricks[r][c])continue;
        int bkx=4+c*29, bky=26+r*18;
        if(bx3>=bkx&&bx3<=bkx+27&&by3>=bky&&by3<=bky+15){
            bricks[r][c]--;
            bvy3=-bvy3;
            score+=10*(bricks[r][c]+1)*level;
            if(score>hi){hi=score;hiScore[10]=hi;}
            drawBrick(r,c);
            sfx_hit();
            if(allClear2()){
                sfx_level(); score+=100*level;
                level++;if(level>15)level=15;
                showLevelUp(level,C_ORANGE);
                genBricks();
                bx3=padX;by3=190;
                bvx3=(random(2)?1:-1)*(2.5f+level*0.18f);
                bvy3=-(2.5f+level*0.18f);
                tft.fillScreen(C_BG);drawAllBricks();drawPaddle();drawBall3();drawHUD4();return;
            }
            break;
        }
    }

    drawPaddle();
    drawBall3();
    if(frameC%15==0) statusBar("BANGI",score,hi);
}