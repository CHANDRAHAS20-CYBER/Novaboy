#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace TNK {
    struct Bullet { float x,y,vx,vy,ox,oy; bool active,wasDrawn; };

    static const int MAX_PB=3, MAX_EB=4;
    static Bullet pBul[MAX_PB], eBul[MAX_EB];

    static float px,py,opx,opy;
    static int   pDirX,pDirY;
    static float ex,ey,oex,oey;
    static int   eDirX,eDirY;
    static bool  enemyAlive,enemyWasDrawn;
    static int   eFireCD,respawnCD;
    static float exPosX,exPosY; static bool exDrawn;

    static int  score,hi,level,frameC;
    static int  lives,kills,killsNeeded,invuln;
    static bool paused;
    static FrameTimer ft;

    static const int OX=20, OY=26, GW=200, GH=156;
    static const uint16_t ARENA_COL  = 0x0841;
    static const uint16_t BORDER_COL = 0x04FF;

    static void restoreFieldAt(int x,int y,int w,int h){
        int x1=max(x,OX), y1=max(y,OY);
        int x2=min(x+w,OX+GW), y2=min(y+h,OY+GH);
        if(x2<=x1||y2<=y1) return;
        tft.fillRect(x1,y1,x2-x1,y2-y1,ARENA_COL);
        if(y1<=OY)      tft.drawFastHLine(OX,OY,GW,BORDER_COL);
        if(y2>=OY+GH)   tft.drawFastHLine(OX,OY+GH-1,GW,BORDER_COL);
        if(x1<=OX)      tft.drawFastVLine(OX,OY,GH,BORDER_COL);
        if(x2>=OX+GW)   tft.drawFastVLine(OX+GW-1,OY,GH,BORDER_COL);
    }
    static void drawField(){
        tft.fillRect(OX,OY,GW,GH,ARENA_COL);
        tft.drawRect(OX,OY,GW,GH,BORDER_COL);
    }
    static void eraseAt(int x,int y,int r){ restoreFieldAt(x-r,y-r,r*2+1,r*2+1); }

    static void drawTank(int x,int y,int dx,int dy,uint16_t col,bool hilite){
        tft.fillRoundRect(x-6,y-6,12,12,2,col);
        tft.drawRoundRect(x-6,y-6,12,12,2,hilite?C_YELLOW:C_WHITE);
        int bx=x+dx*9, by=y+dy*9;
        if(dx!=0) tft.fillRect(min(x,bx),y-1,abs(bx-x)+1,2,C_WHITE);
        else      tft.fillRect(x-1,min(y,by),2,abs(by-y)+1,C_WHITE);
    }

    static void drawHUD(){
        statusBar("TANK STRIKE",score,hi);
        tft.fillRect(0,190,240,50,tft.color565(0,0,0));
        char lb[40]; snprintf(lb,40,"LIVES %d   KILLS %d/%d   LV%d",lives,kills,killsNeeded,level);
        cText(lb,196,1,C_YELLOW);
        cText("DPAD=Move  A=Fire  B=Pause",208,1,tft.color565(28,36,62));
        cText("Destroy tanks - avoid their fire!",220,1,tft.color565(55,65,100));
    }

    static void spawnEnemy(){
        int c=random(4);
        if(c==0){ex=OX+16; ey=OY+16;}
        else if(c==1){ex=OX+GW-16; ey=OY+16;}
        else if(c==2){ex=OX+16; ey=OY+GH-16;}
        else {ex=OX+GW-16; ey=OY+GH-16;}
        oex=ex; oey=ey; eDirX=0; eDirY=1;
        enemyAlive=true; eFireCD=40; respawnCD=0;
    }
    static void resetRound(){
        for(int i=0;i<MAX_PB;i++){ pBul[i].active=false; pBul[i].wasDrawn=false; }
        for(int i=0;i<MAX_EB;i++){ eBul[i].active=false; eBul[i].wasDrawn=false; }
        spawnEnemy();
    }
}

void tankStrikeInit(int lv){
    using namespace TNK;
    level=lv; score=0; hi=hiScore[7]; lives=3; kills=0;
    killsNeeded=4+level/2; if(killsNeeded>10)killsNeeded=10;
    paused=false; frameC=0; invuln=0; exDrawn=false;
    ft=FrameTimer(35);
    px=OX+GW/2; py=OY+GH-20; opx=px; opy=py; pDirX=0; pDirY=-1;
    tft.fillScreen(C_BG);
    drawField();
    resetRound();
    drawTank((int)px,(int)py,pDirX,pDirY,C_CYAN,false);
    drawTank((int)ex,(int)ey,eDirX,eDirY,C_RED,false);
    enemyWasDrawn=true;
    drawHUD();
}

void tankStrikeLoop(){
    using namespace TNK;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){
            paused=false; tft.fillScreen(C_BG); drawField();
            drawTank((int)px,(int)py,pDirX,pDirY,C_CYAN,invuln>0);
            if(enemyAlive) drawTank((int)ex,(int)ey,eDirX,eDirY,C_RED,false);
            drawHUD();
            opx=px; opy=py;
            if(enemyAlive){oex=ex;oey=ey;enemyWasDrawn=true;} else enemyWasDrawn=false;
        }
        else if(r==2){savedLevel[7]=level;saveData();tankStrikeInit(1);}
        else{savedLevel[7]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed(); uint8_t h=btnHeld();
    if(p&BTN_B){paused=true;return;}

    // ---- ERASE (using LAST FRAME's drawn positions) ----
    eraseAt((int)opx,(int)opy,10);
    if(enemyWasDrawn) eraseAt((int)oex,(int)oey,10);
    for(int i=0;i<MAX_PB;i++) if(pBul[i].wasDrawn){eraseAt((int)pBul[i].ox,(int)pBul[i].oy,3); pBul[i].wasDrawn=false;}
    for(int i=0;i<MAX_EB;i++) if(eBul[i].wasDrawn){eraseAt((int)eBul[i].ox,(int)eBul[i].oy,3); eBul[i].wasDrawn=false;}
    if(exDrawn){eraseAt((int)exPosX,(int)exPosY,14); exDrawn=false;}

    // ---- PLAYER MOVE ----
    float spd=3.0f;
    if(h&BTN_LEFT  &&px>OX+8){px-=spd; pDirX=-1; pDirY=0;}
    if(h&BTN_RIGHT &&px<OX+GW-8){px+=spd; pDirX=1; pDirY=0;}
    if(h&BTN_UP    &&py>OY+8){py-=spd; pDirY=-1; pDirX=0;}
    if(h&BTN_DOWN  &&py<OY+GH-8){py+=spd; pDirY=1; pDirX=0;}

    // ---- FIRE ----
    if(p&BTN_A){
        for(int i=0;i<MAX_PB;i++) if(!pBul[i].active){
            pBul[i].active=true;
            pBul[i].x=px+pDirX*9; pBul[i].y=py+pDirY*9;
            pBul[i].vx=pDirX*4.5f; pBul[i].vy=pDirY*4.5f;
            sfx_hit();
            break;
        }
    }

    // ---- BULLET UPDATES ----
    for(int i=0;i<MAX_PB;i++) if(pBul[i].active){
        pBul[i].x+=pBul[i].vx; pBul[i].y+=pBul[i].vy;
        if(pBul[i].x<OX||pBul[i].x>OX+GW||pBul[i].y<OY||pBul[i].y>OY+GH) pBul[i].active=false;
    }
    for(int i=0;i<MAX_EB;i++) if(eBul[i].active){
        eBul[i].x+=eBul[i].vx; eBul[i].y+=eBul[i].vy;
        if(eBul[i].x<OX||eBul[i].x>OX+GW||eBul[i].y<OY||eBul[i].y>OY+GH) eBul[i].active=false;
    }

    // ---- ENEMY AI ----
    bool justKilled=false; float killX=0,killY=0;
    if(enemyAlive){
        float aspd=1.0f+level*0.08f;
        if(px>ex+2) ex+=aspd; else if(px<ex-2) ex-=aspd;
        if(py>ey+2) ey+=aspd; else if(py<ey-2) ey-=aspd;
        ex=constrain(ex,(float)OX+8,(float)OX+GW-8);
        ey=constrain(ey,(float)OY+8,(float)OY+GH-8);
        eDirX=(px>ex)?1:((px<ex)?-1:0);
        eDirY=(py>ey)?1:((py<ey)?-1:0);
        if(eDirX!=0) eDirY=0;

        if(eFireCD>0) eFireCD--;
        if(eFireCD<=0){
            bool aX=fabsf(ex-px)<10, aY=fabsf(ey-py)<10;
            if(aX||aY){
                for(int i=0;i<MAX_EB;i++) if(!eBul[i].active){
                    int dx2=0,dy2=0;
                    if(aX&&!aY) dy2=(py>ey)?1:-1;
                    else if(aY&&!aX) dx2=(px>ex)?1:-1;
                    else dy2=(py>ey)?1:-1;
                    eBul[i].active=true;
                    eBul[i].x=ex+dx2*9; eBul[i].y=ey+dy2*9;
                    eBul[i].vx=dx2*3.2f; eBul[i].vy=dy2*3.2f;
                    break;
                }
                eFireCD=max(22,55-level*3);
            } else eFireCD=8;
        }
    } else if(respawnCD>0){
        respawnCD--;
        if(respawnCD==0) spawnEnemy();
    }

    // ---- PLAYER BULLETS vs ENEMY ----
    if(enemyAlive){
        for(int i=0;i<MAX_PB;i++) if(pBul[i].active){
            float dx3=pBul[i].x-ex, dy3=pBul[i].y-ey;
            if(dx3*dx3+dy3*dy3<10*10){
                pBul[i].active=false;
                enemyAlive=false; enemyWasDrawn=false; respawnCD=18;
                kills++; score+=80+level*8;
                if(score>hi){hi=score;hiScore[7]=hi;}
                sfx_success();
                justKilled=true; killX=ex; killY=ey;
            }
        }
    }

    // ---- HITS ON PLAYER ----
    if(invuln>0) invuln--;
    if(invuln==0){
        bool hitMe=false;
        for(int i=0;i<MAX_EB;i++) if(eBul[i].active){
            float dx4=eBul[i].x-px, dy4=eBul[i].y-py;
            if(dx4*dx4+dy4*dy4<8*8){ eBul[i].active=false; hitMe=true; }
        }
        if(enemyAlive){
            float dx5=ex-px, dy5=ey-py;
            if(dx5*dx5+dy5*dy5<10*10) hitMe=true;
        }
        if(hitMe){ lives--; invuln=45; vibroPulse(170,90); sfx_die(); }
    }

    if(lives<=0){
        bool r2=showGameOver(score,hi,score>hiScore[7]);
        if(score>hiScore[7])hiScore[7]=score;
        gamePlayed[7]=true; saveData();
        if(r2) tankStrikeInit(1); else { appState=APP_HOME; homeScreen(); }
        return;
    }

    if(justKilled && kills>=killsNeeded){
        sfx_level(); score+=100+level*15;
        if(score>hi){hi=score;hiScore[7]=hi;}
        level++; if(level>15) level=15;
        showLevelUp(level,C_ORANGE);
        kills=0; killsNeeded=4+level/2; if(killsNeeded>10)killsNeeded=10;
        tft.fillScreen(C_BG); drawField();
        resetRound();
        opx=px; opy=py;
        drawTank((int)px,(int)py,pDirX,pDirY,C_CYAN,false);
        drawTank((int)ex,(int)ey,eDirX,eDirY,C_RED,false);
        oex=ex; oey=ey; enemyWasDrawn=true;
        drawHUD();
        return;
    }

    // ---- DRAW (becomes "last frame" for next erase) ----
    drawTank((int)px,(int)py,pDirX,pDirY,C_CYAN,invuln>0);
    opx=px; opy=py;
    if(enemyAlive){
        drawTank((int)ex,(int)ey,eDirX,eDirY,C_RED,false);
        oex=ex; oey=ey; enemyWasDrawn=true;
    }
    for(int i=0;i<MAX_PB;i++) if(pBul[i].active){
        tft.fillCircle((int)pBul[i].x,(int)pBul[i].y,2,C_WHITE);
        pBul[i].ox=pBul[i].x; pBul[i].oy=pBul[i].y; pBul[i].wasDrawn=true;
    }
    for(int i=0;i<MAX_EB;i++) if(eBul[i].active){
        tft.fillCircle((int)eBul[i].x,(int)eBul[i].y,2,C_YELLOW);
        eBul[i].ox=eBul[i].x; eBul[i].oy=eBul[i].y; eBul[i].wasDrawn=true;
    }
    if(justKilled){
        tft.drawCircle((int)killX,(int)killY,12,C_ORANGE);
        tft.drawCircle((int)killX,(int)killY,6,C_WHITE);
        exPosX=killX; exPosY=killY; exDrawn=true;
    }

    if(frameC%10==0) statusBar("TANK STRIKE",score,hi);
}