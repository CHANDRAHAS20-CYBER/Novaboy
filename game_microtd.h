#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace TD {
    static const int GW=12, GH=9;
    static const int TW=18, TH=18, OX=12, OY=22;
    enum { T_EMPTY=0, T_PATH, T_TOWER_GUN, T_TOWER_SLOW, T_BASE };
    static uint8_t grid2[GH][GW];
    static int cx,cy,gold,lives2,score,hi,level,wave,frameC;
    static bool paused, waveRunning;
    static FrameTimer ft;

    struct Enemy3 { int pathIdx; float prog; int hp,maxHp; int ox,oy; bool alive; };
    static const int MAX_EN3=12;
    static Enemy3 ens3[MAX_EN3];

    struct Bullet2 { float x,y,tx,ty,ox,oy; bool alive; int dmg; };
    static const int MAX_BUL=16;
    static Bullet2 buls[MAX_BUL];

    static const int PATH_N=20;
    static int pathX[PATH_N]={0,1,2,3,4,4,4,4,4,5,6,7,8,8,8,8,8,9,10,11};
    static int pathY[PATH_N]={4,4,4,4,4,3,2,1,0,0,0,0,0,1,2,3,4,4,4,4};

    static bool gridDirty=true;

    static uint16_t tileC2(uint8_t t){
        switch(t){
            case T_PATH:      return tft.color565(30,25,10);
            case T_TOWER_GUN: return tft.color565(0,80,200);
            case T_TOWER_SLOW:return tft.color565(0,160,80);
            case T_BASE:      return tft.color565(180,0,0);
        }
        return tft.color565(10,30,10);
    }

    static void drawTile2(int gx, int gy){
        int px2=OX+gx*TW, py2=OY+gy*TH;
        uint8_t t=grid2[gy][gx];
        tft.fillRect(px2,py2,TW-1,TH-1,tileC2(t));
        bool isCur=(gx==cx&&gy==cy);
        if(isCur) tft.drawRect(px2,py2,TW-1,TH-1,C_WHITE);
        if(t==T_TOWER_GUN){
            tft.fillRect(px2+5,py2+5,8,8,tft.color565(0,50,150));
            tft.fillRect(px2+8,py2+2,3,6,tft.color565(0,120,220));
            tft.fillRect(px2+7,py2+1,5,3,tft.color565(200,200,0));
        } else if(t==T_TOWER_SLOW){
            tft.fillCircle(px2+8,py2+9,5,tft.color565(0,100,50));
            tft.fillCircle(px2+8,py2+6,3,tft.color565(0,200,100));
            tft.fillCircle(px2+8,py2+5,1,tft.color565(100,255,180));
        } else if(t==T_BASE){
            tft.fillRect(px2+3,py2+3,TW-5,TH-5,tft.color565(150,0,0));
            tft.setTextSize(1); tft.setTextColor(C_WHITE);
            tft.setCursor(px2+2,py2+5); tft.print("HQ");
        }
    }

    static void drawGrid2(){
        for(int gy=0;gy<GH;gy++) for(int gx=0;gx<GW;gx++) drawTile2(gx,gy);
        gridDirty=false;
    }

    static int enemyScreenX(int i){ int pi=(int)ens3[i].prog; if(pi>=PATH_N-1)return-99; float frac=ens3[i].prog-pi; return OX+pathX[pi]*TW+(int)((pathX[min(pi+1,PATH_N-1)]-pathX[pi])*frac*TW)+TW/2; }
    static int enemyScreenY(int i){ int pi=(int)ens3[i].prog; if(pi>=PATH_N-1)return-99; float frac=ens3[i].prog-pi; return OY+pathY[pi]*TH+(int)((pathY[min(pi+1,PATH_N-1)]-pathY[pi])*frac*TH)+TH/2; }

    static void eraseEnemy3(int i){ tft.fillRect(ens3[i].ox-7,ens3[i].oy-7,16,16,tileC2(grid2[pathY[(int)min((int)ens3[i].prog,PATH_N-1)]][pathX[(int)min((int)ens3[i].prog,PATH_N-1)]])); }

    static void drawEnemy3(int i){
        int ex=enemyScreenX(i), ey=enemyScreenY(i);
        if(ex<0) return;
        // Bug body - use fillRect instead of fillEllipse
        tft.fillRoundRect(ex-5,ey-5,11,10,2,tft.color565(180,60,0));
        tft.fillRect(ex-4,ey-3,9,5,tft.color565(220,100,20));
        tft.fillRect(ex-3,ey-4,2,2,tft.color565(255,50,50));
        tft.fillRect(ex+1,ey-4,2,2,tft.color565(255,50,50));
        tft.drawLine(ex-5,ey,ex-8,ey-2,tft.color565(100,40,0));
        tft.drawLine(ex+5,ey,ex+8,ey-2,tft.color565(100,40,0));
        int hw=(ens3[i].hp*10)/ens3[i].maxHp; if(hw<0)hw=0;
        tft.fillRect(ex-5,ey-8,10,2,tft.color565(30,0,0));
        if(hw>0) tft.fillRect(ex-5,ey-8,hw,2,C_GREEN);
        ens3[i].ox=ex; ens3[i].oy=ey;
    }

    static void drawHUD2(){
        statusBar("MICRO TD",score,hi);
        tft.fillRect(0,184,240,14,tft.color565(0,0,0));
        char lb[50]; snprintf(lb,50,"GOLD:%d LIVES:%d WAVE:%d LV%d",gold,lives2,wave,level);
        cText(lb,186,1,C_YELLOW);
        tft.fillRect(0,200,240,22,tft.color565(0,0,0));
        neonBtn(4,202,80,18,"GUN $30",tft.color565(0,80,200),cx==0&&cy==0);
        neonBtn(90,202,90,18,"SLOW $50",tft.color565(0,160,80),false);
        neonBtn(186,202,50,18,"SELL",C_RED,false);
        tft.fillRect(0,224,240,10,tft.color565(0,0,0));
        cText("DPAD=Move A=Place/Sell B=Pause",226,1,tft.color565(28,36,62));
    }

    static void spawnWave(){
        int count=2+wave*2; if(count>MAX_EN3)count=MAX_EN3;
        for(int i=0;i<count;i++){ ens3[i]={0,-(float)i*1.5f,3+wave*2,3+wave*2,-99,-99,true}; }
        for(int i=count;i<MAX_EN3;i++) ens3[i].alive=false;
        waveRunning=true;
    }

    static void towerShoot(){
        for(int gy=0;gy<GH;gy++) for(int gx=0;gx<GW;gx++){
            if(grid2[gy][gx]!=T_TOWER_GUN&&grid2[gy][gx]!=T_TOWER_SLOW) continue;
            int tx=OX+gx*TW+TW/2, ty=OY+gy*TH+TH/2;
            int range=(grid2[gy][gx]==T_TOWER_GUN)?65:85;
            float nearest=9999; int ni=-1;
            for(int i=0;i<MAX_EN3;i++){
                if(!ens3[i].alive) continue;
                int ex=enemyScreenX(i), ey=enemyScreenY(i); if(ex<0)continue;
                float d=sqrtf((float)((tx-ex)*(tx-ex)+(ty-ey)*(ty-ey)));
                if(d<range&&d<nearest){nearest=d;ni=i;}
            }
            if(ni<0) continue;
            for(int b=0;b<MAX_BUL;b++) if(!buls[b].alive){
                int ex=enemyScreenX(ni), ey=enemyScreenY(ni);
                buls[b]={(float)tx,(float)ty,(float)ex,(float)ey,(float)tx,(float)ty,true,grid2[gy][gx]==T_TOWER_GUN?2:1};
                break;
            }
        }
    }
}

void microTDInit(int lv){
    using namespace TD;
    level=lv; score=0; hi=hiScore[5]; gold=80+lv*20; lives2=10; wave=0; paused=false; frameC=0; waveRunning=false;
    ft=FrameTimer(38);
    memset(grid2,0,sizeof(grid2));
    for(int i=0;i<PATH_N;i++) grid2[pathY[i]][pathX[i]]=T_PATH;
    grid2[4][11]=T_BASE; cx=0; cy=0;
    for(int i=0;i<MAX_EN3;i++){ens3[i].alive=false;ens3[i].ox=-99;ens3[i].oy=-99;}
    for(int i=0;i<MAX_BUL;i++) buls[i].alive=false;
    tft.fillScreen(C_BG);
    drawGrid2(); drawHUD2(); gridDirty=false;
}

void microTDLoop(){
    using namespace TD;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){paused=false;tft.fillScreen(C_BG);drawGrid2();drawHUD2();}
        else if(r==2){savedLevel[5]=level;saveData();microTDInit(1);}
        else{savedLevel[5]=level;saveData();appState=APP_HOME;homeScreen();}
        return;
    }
    if(!ft.ready())return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B){paused=true;return;}

    int ocx=cx,ocy=cy;
    if(p&BTN_UP    &&cy>0)    cy--;
    if(p&BTN_DOWN  &&cy<GH-1) cy++;
    if(p&BTN_LEFT  &&cx>0)    cx--;
    if(p&BTN_RIGHT &&cx<GW-1) cx++;
    if(cx!=ocx||cy!=ocy){ drawTile2(ocx,ocy); drawTile2(cx,cy); sfx_tap(); }

    if(p&BTN_A){
        int gv=grid2[cy][cx];
        if(gv==T_EMPTY){
            if(gold>=30){grid2[cy][cx]=T_TOWER_GUN;gold-=30;drawTile2(cx,cy);drawHUD2();sfx_ping();}
            else if(gold>=50){grid2[cy][cx]=T_TOWER_SLOW;gold-=50;drawTile2(cx,cy);drawHUD2();sfx_ping();}
            else sfx_wrong();
        } else if(gv==T_TOWER_GUN||gv==T_TOWER_SLOW){
            grid2[cy][cx]=T_EMPTY; gold+=15; drawTile2(cx,cy); drawHUD2(); sfx_back();
        }
    }

    if(!waveRunning&&frameC>20&&frameC%50==0){ wave++; spawnWave(); drawHUD2(); }

    if(waveRunning){
        float spd=0.03f+wave*0.01f;
        bool anyAlive=false;
        for(int i=0;i<MAX_EN3;i++){
            if(!ens3[i].alive)continue;
            anyAlive=true;
            if(ens3[i].prog<0){ens3[i].prog+=spd;continue;}
            if(ens3[i].ox>=0) eraseEnemy3(i);
            ens3[i].prog+=spd;
            if(ens3[i].prog>=PATH_N-1){ens3[i].alive=false;lives2--;sfx_hit();drawHUD2();continue;}
            drawEnemy3(i);
        }
        if(!anyAlive){
            waveRunning=false; gold+=30+wave*10; score+=wave*50;
            sfx_level(); drawHUD2();
            if(wave>=level){ level++;if(level>15)level=15; showLevelUp(level,C_ORANGE); microTDInit(level); return; }
        }

        if(frameC%2==0) towerShoot();

        for(int i=0;i<MAX_BUL;i++){
            if(!buls[i].alive)continue;
            tft.fillRect((int)buls[i].ox-2,(int)buls[i].oy-2,5,5,tileC2(grid2[min((int)((buls[i].oy-OY)/TH),GH-1)][min((int)((buls[i].ox-OX)/TW),GW-1)]));
            float dx2=buls[i].tx-buls[i].x, dy2=buls[i].ty-buls[i].y;
            float d=sqrtf(dx2*dx2+dy2*dy2);
            if(d<6){
                buls[i].alive=false;
                for(int j=0;j<MAX_EN3;j++){
                    if(!ens3[j].alive)continue;
                    int ex=ens3[j].ox, ey=ens3[j].oy;
                    float dd=sqrtf((float)((ex-(int)buls[i].tx)*(ex-(int)buls[i].tx)+(ey-(int)buls[i].ty)*(ey-(int)buls[i].ty)));
                    if(dd<12){ens3[j].hp-=buls[i].dmg;if(ens3[j].hp<=0){ens3[j].alive=false;if(ens3[j].ox>=0)eraseEnemy3(j);gold+=5;score+=10;sfx_coin();drawHUD2();}break;}
                }
            } else {
                buls[i].ox=buls[i].x; buls[i].oy=buls[i].y;
                buls[i].x+=dx2/d*5; buls[i].y+=dy2/d*5;
                tft.fillCircle((int)buls[i].x,(int)buls[i].y,2,C_YELLOW);
            }
        }
    }

    if(lives2<=0){
        bool r2=showGameOver(score,hi,score>hiScore[5]);
        if(score>hiScore[5])hiScore[5]=score;
        gamePlayed[5]=true;saveData();
        if(r2)microTDInit(1);else{appState=APP_HOME;homeScreen();}
        return;
    }
    if(score>hi){hi=score;hiScore[5]=hi;}
    if(frameC%8==0) statusBar("MICRO TD",score,hi);
}