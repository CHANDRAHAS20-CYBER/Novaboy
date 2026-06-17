#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace OH {
    static float px7,py7,pvy2,opx7,opy7;
    static bool  onGround2,dblJump;
    static int   score,hi,level,lives3,dist,frameC;
    static bool  paused;
    static FrameTimer ft;

    struct Obj { int x,y,w,h,type; bool alive; };
    static const int MAX_OBJ=8;
    static Obj objs[MAX_OBJ];
    static int groundY=185;
    static bool bgDirty=true;

    static void drawBG2(){
        tft.fillRect(0,22,240,163,tft.color565(2,5,20));
        srand(level*99);
        for(int i=0;i<8;i++){
            int bx=rand()%240, bh=20+rand()%50, bw=15+rand()%25;
            tft.fillRect(bx,groundY-bh,bw,bh,tft.color565(8+rand()%15,8+rand()%15,20+rand()%30));
            for(int wy=groundY-bh+4;wy<groundY-4;wy+=8) for(int wx=bx+2;wx<bx+bw-2;wx+=5)
                if(rand()%2) tft.fillRect(wx,wy,3,4,tft.color565(200,180,80));
        }
        tft.fillRect(0,groundY,240,8,tft.color565(30,30,35));
        tft.fillRect(0,groundY,240,3,tft.color565(60,60,70));
        for(int x=0;x<240;x+=30) tft.fillRect(x,groundY+4,20,2,tft.color565(80,80,90));
        bgDirty=false;
    }

    static void eraseObj(int i){
        Obj& o=objs[i];
        tft.fillRect(o.x-2,o.y-2,o.w+4,o.h+4,tft.color565(2,5,20));
        if(o.y+o.h>=groundY) tft.fillRect(o.x-2,groundY,o.w+4,8,tft.color565(30,30,35));
    }

    static void drawObj(int i){
        Obj& o=objs[i];
        if(o.type==0){
            tft.fillTriangle(o.x,o.y+o.h,o.x+5,o.y,o.x+10,o.y+o.h,C_RED);
            tft.fillTriangle(o.x+8,o.y+o.h,o.x+13,o.y,o.x+18,o.y+o.h,tft.color565(200,0,50));
            tft.drawLine(o.x,o.y+o.h,o.x+10,o.y,tft.color565(255,100,100));
        } else if(o.type==1){
            tft.fillRect(o.x,o.y,o.w,o.h,tft.color565(50,60,80));
            tft.fillRect(o.x,o.y,o.w,3,tft.color565(80,200,100));
            tft.drawRect(o.x,o.y,o.w,o.h,tft.color565(60,80,100));
        } else if(o.type==2){
            tft.fillCircle(o.x+5,o.y+5,5,C_YELLOW);
            tft.fillCircle(o.x+4,o.y+4,2,tft.color565(255,220,100));
            tft.drawCircle(o.x+5,o.y+5,5,tft.color565(180,130,0));
        } else {
            tft.fillRoundRect(o.x,o.y,12,12,3,C_CYAN);
            tft.fillRect(o.x+4,o.y+2,4,8,C_BG);
            tft.fillRect(o.x+2,o.y+5,8,2,C_BG);
            tft.drawRoundRect(o.x,o.y,12,12,3,C_WHITE);
        }
    }

    // FIX: erase box now matches the full sprite (was 2px short top AND bottom,
    // which is exactly what left boot/helmet residue while jumping).
    static void eraseRunner(){
        tft.fillRect((int)opx7-7,(int)opy7-18,16,25,tft.color565(2,5,20));
        if((int)opy7+6>=groundY) tft.fillRect((int)opx7-7,groundY,16,8,tft.color565(30,30,35));
    }
    static void drawRunner2(){
        int x=(int)px7, y=(int)py7;
        int fr=(frameC/3)%4;
        tft.fillRect(x-7,y-12,3,10,tft.color565(180,0,0));
        tft.fillRect(x-4,y-12,9,8,tft.color565(30,80,200));
        tft.fillRect(x-3,y-10,7,5,tft.color565(60,110,230));
        tft.fillCircle(x,y-14,4,tft.color565(255,210,160));
        tft.fillRect(x-4,y-18,8,5,tft.color565(20,20,20));
        tft.fillRect(x-3,y-17,6,2,tft.color565(0,200,255));
        if(fr==0||fr==2){
            tft.fillRect(x-3,y-4,3,7,tft.color565(20,50,150));
            tft.fillRect(x+1,y-4,3,5,tft.color565(20,50,150));
            tft.fillRect(x-3,y+3,4,3,tft.color565(20,20,20));
            tft.fillRect(x+1,y+1,4,3,tft.color565(20,20,20));
        } else {
            tft.fillRect(x-3,y-4,3,5,tft.color565(20,50,150));
            tft.fillRect(x+1,y-4,3,7,tft.color565(20,50,150));
            tft.fillRect(x-3,y+1,4,3,tft.color565(20,20,20));
            tft.fillRect(x+1,y+3,4,3,tft.color565(20,20,20));
        }
        if(dblJump){
            tft.fillRect(x+4,y-11,5,7,tft.color565(60,70,80));
            tft.fillRect(x+5,y-5,3,4,tft.color565(255,100,0));
        }
        opx7=px7; opy7=py7;
    }

    static void spawnObj2(){
        for(int i=0;i<MAX_OBJ;i++) if(!objs[i].alive){
            int t=random(4);
            if(t==0)      objs[i]={240,groundY-14,18,14,0,true};
            else if(t==1) objs[i]={240,groundY-35-random(20),40+random(40),8,1,true};
            else if(t==2) objs[i]={240,groundY-25-random(25),10,10,2,true};
            else          objs[i]={240,groundY-22,12,12,3,true};
            break;
        }
    }

    static void drawHUD3(){
        statusBar("OMEGA HORIZON",score,hi);
        tft.fillRect(0,194,240,12,tft.color565(0,0,0));
        char lb[40]; snprintf(lb,40,"DIST:%d LIVES:%d LV%d",dist,lives3,level);
        cText(lb,196,1,C_YELLOW);
        tft.fillRect(0,208,240,10,tft.color565(0,0,0));
        for(int i=0;i<lives3;i++) tft.fillCircle(8+i*14,213,4,C_RED);
        cText("A=Jump(x2) B=Pause",220,1,tft.color565(28,36,62));
    }
}

void omegaHorizonInit(int lv){
    using namespace OH;
    level=lv; score=0; hi=hiScore[9]; lives3=3; dist=0; paused=false; frameC=0;
    px7=40; py7=groundY; pvy2=0; opx7=40; opy7=groundY;
    onGround2=true; dblJump=false;
    ft=FrameTimer(32);
    for(int i=0;i<MAX_OBJ;i++) objs[i].alive=false;
    bgDirty=true;
    tft.fillScreen(C_BG);
    drawBG2(); drawRunner2(); drawHUD3();
}

void omegaHorizonLoop(){
    using namespace OH;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;bgDirty=true;tft.fillScreen(C_BG);drawBG2();drawRunner2();drawHUD3();}
        else if(r==2){savedLevel[9]=level;saveData();omegaHorizonInit(1);}
        else{savedLevel[9]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B){paused=true;return;}

    if(p&BTN_A){
        if(onGround2){pvy2=-8.5f;onGround2=false;dblJump=true;sfx_jump();}
        else if(dblJump){pvy2=-7.5f;dblJump=false;sfx_jump();}
    }

    pvy2+=0.5f; py7+=pvy2;
    int spd=2+level/3;

    onGround2=false;
    if(py7>=groundY){py7=groundY;pvy2=0;onGround2=true;dblJump=false;}

    for(int i=0;i<MAX_OBJ;i++){
        if(!objs[i].alive)continue;
        eraseObj(i);
        objs[i].x-=spd;
        if(objs[i].x+objs[i].w<0){objs[i].alive=false;continue;}
        if(objs[i].type==1&&pvy2>=0){
            if((int)px7+4>=objs[i].x&&(int)px7-4<=objs[i].x+objs[i].w&&
               (int)py7>=(objs[i].y-2)&&(int)py7<=(objs[i].y+10)){
                py7=objs[i].y; pvy2=0; onGround2=true;
            }
        }
        if(objs[i].type==0&&(int)px7+4>=objs[i].x&&(int)px7-4<=objs[i].x+objs[i].w&&(int)py7>=objs[i].y){
            lives3--;sfx_hit();vibroPulse(180,80);objs[i].alive=false;
            if(lives3<=0){bool r2=showGameOver(score,hi,score>hiScore[9]);if(score>hiScore[9])hiScore[9]=score;gamePlayed[9]=true;saveData();if(r2)omegaHorizonInit(1);else{appState=APP_HOME;homeScreen();}return;}
            drawHUD3();
        }
        if(objs[i].type==2&&abs((int)px7-objs[i].x-5)<14&&abs((int)py7-(objs[i].y+5))<14){
            score+=10;sfx_coin();objs[i].alive=false;drawHUD3();
        }
        if(objs[i].type==3&&abs((int)px7-objs[i].x-6)<14&&abs((int)py7-(objs[i].y+6))<14){
            lives3=min(lives3+1,5);sfx_powerup();objs[i].alive=false;drawHUD3();
        }
        if(objs[i].alive) drawObj(i);
    }

    if(frameC%(max(32-level*2,12))==0) spawnObj2();

    dist++;
    score++;
    if(score>hi){hi=score;hiScore[9]=hi;}
    if(dist>level*300){level++;if(level>15)level=15;showLevelUp(level,C_CYAN);omegaHorizonInit(level);return;}

    eraseRunner();
    drawRunner2();

    if(frameC%10==0) drawHUD3();
    if(frameC%20==0) statusBar("OMEGA HORIZON",score,hi);
}