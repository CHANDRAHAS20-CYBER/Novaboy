#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace SHS {
    static int   shipX,oShipX;
    static int   score,hi,level,lives5,frameC;
    static bool  paused;
    static FrameTimer ft;

    struct Bullet3 { int x,y,oy; bool alive; };
    static const int MAX_BUL3=6;
    static Bullet3 bullets[MAX_BUL3];

    struct Star3 { int x,y,oy; bool alive; };
    static const int MAX_STARS3=50;
    static Star3 bgStars[MAX_STARS3];

    struct Enemy4 { int x,y,oy,vx,hp; bool alive; };
    static const int MAX_EN4=8;
    static Enemy4 enemies[MAX_EN4];

    struct EBullet { int x,y,oy; bool alive; };
    static const int MAX_EBUL=8;
    static EBullet ebuls[MAX_EBUL];

    static uint16_t starfieldBG[MAX_STARS3];

    static void initStarfield(){
        for(int i=0;i<MAX_STARS3;i++){
            bgStars[i].x=random(240);
            bgStars[i].y=random(188)+22;
            bgStars[i].oy=bgStars[i].y;
            bgStars[i].alive=true;
            uint8_t b=30+random(160);
            starfieldBG[i]=tft.color565(b>>2,b>>2,b);
        }
    }

    static void scrollStars(){
        for(int i=0;i<MAX_STARS3;i++){
            tft.drawPixel(bgStars[i].x,bgStars[i].oy,C_BG);
            bgStars[i].y++;
            if(bgStars[i].y>210){bgStars[i].y=22;bgStars[i].x=random(240);}
            bgStars[i].oy=bgStars[i].y;
            tft.drawPixel(bgStars[i].x,bgStars[i].y,starfieldBG[i]);
        }
    }

    static void drawShip(bool erase){
        if(erase){
            tft.fillRect(oShipX-14,190,30,26,C_BG);
            for(int i=0;i<MAX_STARS3;i++)
                if(bgStars[i].x>=oShipX-14&&bgStars[i].x<=oShipX+14&&bgStars[i].y>=190&&bgStars[i].y<=216)
                    tft.drawPixel(bgStars[i].x,bgStars[i].y,starfieldBG[i]);
            return;
        }
        tft.fillTriangle(shipX,192,shipX-8,210,shipX+8,210,tft.color565(0,180,220));
        tft.fillTriangle(shipX,195,shipX-5,207,shipX+5,207,tft.color565(100,220,255));
        tft.fillRect(shipX-3,208,7,4,(frameC%6<3)?C_ORANGE:C_YELLOW);
        tft.fillTriangle(shipX-8,210,shipX-14,214,shipX-4,210,tft.color565(0,120,160));
        tft.fillTriangle(shipX+8,210,shipX+14,214,shipX+4,210,tft.color565(0,120,160));
        oShipX=shipX;
    }

    static void drawEnemy4(int i, bool erase){
        Enemy4& e=enemies[i];
        if(erase){
            tft.fillRect(e.x-10,e.oy-8,22,20,C_BG);
            for(int s=0;s<MAX_STARS3;s++)
                if(bgStars[s].x>=e.x-10&&bgStars[s].x<=e.x+10&&bgStars[s].y>=e.oy-8&&bgStars[s].y<=e.oy+12)
                    tft.drawPixel(bgStars[s].x,bgStars[s].y,starfieldBG[s]);
            return;
        }
        tft.fillRect(e.x-8,e.y,17,6,tft.color565(180,30,30));
        tft.fillRect(e.x-5,e.y-5,11,6,tft.color565(220,60,60));
        tft.fillCircle(e.x,e.y-2,3,tft.color565(255,150,150));
        tft.fillRect(e.x-8,e.y+7,17,2,tft.color565(40,0,0));
        int maxhp=2+level/3;
        int hw=(e.hp*17)/maxhp; if(hw>17)hw=17; if(hw<0)hw=0;
        if(hw>0) tft.fillRect(e.x-8,e.y+7,hw,2,C_GREEN);
        e.oy=e.y;
    }

    static void spawnEnemy(){
        for(int i=0;i<MAX_EN4;i++) if(!enemies[i].alive){
            enemies[i]={random(220)+10,28,28,(random(3)-1)*(1+level/4),2+level/3,true};
            break;
        }
    }

    static void drawHUD7(){
        statusBar("STAR SHOOTER",score,hi);
        tft.fillRect(0,214,240,12,tft.color565(0,0,0));
        char lb[30]; snprintf(lb,30,"LV%d",level);
        tft.setTextSize(1); tft.setTextColor(C_YELLOW); tft.setCursor(4,216); tft.print(lb);
        for(int i=0;i<lives5;i++) tft.fillCircle(60+i*14,220,4,C_RED);
        for(int i=lives5;i<3;i++) tft.drawCircle(60+i*14,220,4,tft.color565(60,0,0));
        tft.fillRect(0,228,240,12,tft.color565(0,0,0));
        cText("L/R=Move  A=Shoot  B=Pause",230,1,tft.color565(28,36,62));
    }
}

void gloveInit(int lv){
    using namespace SHS;
    level=lv; score=0; hi=hiScore[11]; lives5=3; paused=false; frameC=0;
    shipX=120; oShipX=120;
    for(int i=0;i<MAX_BUL3;i++) bullets[i].alive=false;
    for(int i=0;i<MAX_EN4;i++) enemies[i].alive=false;
    for(int i=0;i<MAX_EBUL;i++) ebuls[i].alive=false;
    ft=FrameTimer(30);
    tft.fillScreen(C_BG);
    initStarfield();
    for(int i=0;i<MAX_STARS3;i++) tft.drawPixel(bgStars[i].x,bgStars[i].y,starfieldBG[i]);
    drawShip(false);
    drawHUD7();
}

void gloveLoop(){
    using namespace SHS;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){
            paused=false;
            tft.fillScreen(C_BG);
            for(int i=0;i<MAX_STARS3;i++) tft.drawPixel(bgStars[i].x,bgStars[i].y,starfieldBG[i]);
            drawShip(false);
            for(int i=0;i<MAX_EN4;i++) if(enemies[i].alive) drawEnemy4(i,false);
            drawHUD7();
        }
        else if(r==2){savedLevel[11]=level;saveData();gloveInit(1);}
        else{savedLevel[11]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed(); uint8_t h=btnHeld();
    if(p&BTN_B){paused=true;return;}

    drawShip(true);
    if(h&BTN_LEFT  &&shipX>14) shipX-=4;
    if(h&BTN_RIGHT &&shipX<226)shipX+=4;

    if(p&BTN_A){
        for(int i=0;i<MAX_BUL3;i++) if(!bullets[i].alive){
            bullets[i]={shipX,190,190,true};
            sfx_shoot(); break;
        }
    }

    if(frameC%2==0) scrollStars();

    // Move player bullets
    for(int i=0;i<MAX_BUL3;i++){
        if(!bullets[i].alive)continue;
        tft.fillRect(bullets[i].x-1,bullets[i].oy-4,3,8,C_BG);
        for(int s=0;s<MAX_STARS3;s++)
            if(bgStars[s].x>=bullets[i].x-1&&bgStars[s].x<=bullets[i].x+1
            &&bgStars[s].y>=bullets[i].oy-4&&bgStars[s].y<=bullets[i].oy+4)
                tft.drawPixel(bgStars[s].x,bgStars[s].y,starfieldBG[s]);
        bullets[i].oy=bullets[i].y;
        bullets[i].y-=8;
        // FIX: was <22 (let it draw right up to / into the status bar). Now stops
        // 4px earlier so the bullet can never touch the status bar area.
        if(bullets[i].y<26){bullets[i].alive=false;continue;}
        tft.fillRect(bullets[i].x-1,bullets[i].y-4,3,8,C_CYAN);
        for(int j=0;j<MAX_EN4;j++){
            if(!enemies[j].alive)continue;
            if(abs(bullets[i].x-enemies[j].x)<10&&abs(bullets[i].y-enemies[j].y)<10){
                enemies[j].hp--;
                tft.fillRect(bullets[i].x-1,bullets[i].y-4,3,8,C_BG);
                bullets[i].alive=false;
                if(enemies[j].hp<=0){
                    drawEnemy4(j,true);
                    enemies[j].alive=false;
                    score+=10+level*5; sfx_coin();
                    if(score>hi){hi=score;hiScore[11]=hi;}
                } else {
                    drawEnemy4(j,false); sfx_hit();
                }
                break;
            }
        }
    }

    // Move enemies
    for(int i=0;i<MAX_EN4;i++){
        if(!enemies[i].alive)continue;
        drawEnemy4(i,true);
        enemies[i].y+=1+level/5;
        enemies[i].x+=enemies[i].vx;
        if(enemies[i].x<10||enemies[i].x>230) enemies[i].vx=-enemies[i].vx;
        if(enemies[i].y>200){
            enemies[i].alive=false;
            lives5--; sfx_die(); vibroPulse(180,80); drawHUD7();
            if(lives5<=0){
                bool r2=showGameOver(score,hi,score>hiScore[11]);
                if(score>hiScore[11])hiScore[11]=score;
                gamePlayed[11]=true;saveData();
                if(r2)gloveInit(1);else{appState=APP_HOME;homeScreen();}return;
            }
            continue;
        }
        if(abs(enemies[i].x-shipX)<14&&enemies[i].y>188){
            drawEnemy4(i,true);
            enemies[i].alive=false;
            lives5--; sfx_hit(); vibroPulse(200,100); drawHUD7();
            if(lives5<=0){
                bool r2=showGameOver(score,hi,score>hiScore[11]);
                if(score>hiScore[11])hiScore[11]=score;
                gamePlayed[11]=true;saveData();
                if(r2)gloveInit(1);else{appState=APP_HOME;homeScreen();}return;
            }
            continue;
        }
        drawEnemy4(i,false);
        if(frameC%(max(60-level*3,20))==0&&i==frameC%MAX_EN4){
            for(int b=0;b<MAX_EBUL;b++) if(!ebuls[b].alive){
                ebuls[b]={enemies[i].x,enemies[i].y,enemies[i].y,true};
                break;
            }
        }
    }

    // Move enemy bullets
    for(int i=0;i<MAX_EBUL;i++){
        if(!ebuls[i].alive)continue;
        tft.fillRect(ebuls[i].x-1,ebuls[i].oy-3,3,7,C_BG);
        for(int s=0;s<MAX_STARS3;s++)
            if(bgStars[s].x>=ebuls[i].x-1&&bgStars[s].x<=ebuls[i].x+1
            &&bgStars[s].y>=ebuls[i].oy-3&&bgStars[s].y<=ebuls[i].oy+3)
                tft.drawPixel(bgStars[s].x,bgStars[s].y,starfieldBG[s]);
        ebuls[i].oy=ebuls[i].y;
        ebuls[i].y+=5;
        // FIX: was >212 (let it draw into the bottom HUD area). Now stops earlier.
        if(ebuls[i].y>208){ebuls[i].alive=false;continue;}
        tft.fillRect(ebuls[i].x-1,ebuls[i].y-3,3,7,C_RED);
        if(abs(ebuls[i].x-shipX)<10&&ebuls[i].y>188){
            tft.fillRect(ebuls[i].x-1,ebuls[i].y-3,3,7,C_BG);
            ebuls[i].alive=false;
            lives5--; sfx_hit(); vibroPulse(150,80); drawHUD7();
            if(lives5<=0){
                bool r2=showGameOver(score,hi,score>hiScore[11]);
                if(score>hiScore[11])hiScore[11]=score;
                gamePlayed[11]=true;saveData();
                if(r2)gloveInit(1);else{appState=APP_HOME;homeScreen();}return;
            }
        }
    }

    if(frameC%(max(50-level*3,15))==0) spawnEnemy();

    if(score>=(level*200)){
        level++;if(level>15)level=15;
        showLevelUp(level,C_CYAN);
        tft.fillScreen(C_BG);
        for(int i=0;i<MAX_STARS3;i++) tft.drawPixel(bgStars[i].x,bgStars[i].y,starfieldBG[i]);
        for(int i=0;i<MAX_EN4;i++) enemies[i].alive=false;
        for(int i=0;i<MAX_BUL3;i++) bullets[i].alive=false;
        for(int i=0;i<MAX_EBUL;i++) ebuls[i].alive=false;
        drawShip(false); drawHUD7();
    }

    drawShip(false);
    if(frameC%8==0) statusBar("STAR SHOOTER",score,hi);
}