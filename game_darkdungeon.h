#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

namespace DD {
    static const int MW=15, MH=13;
    static const int TW=14, TH=14;
    static const int OX=15, OY=22;

    enum { T_WALL=0, T_FLOOR, T_DOOR, T_KEY, T_CHEST, T_STAIRS, T_SPIKE };
    static uint8_t dmap[MH][MW];
    static int  px,py,opx,opy;
    static int  hp,maxHp,score,hi,level,frameC;
    static bool paused, hasKey;
    static FrameTimer ft;

    struct Enemy { int x,y,ox,oy,hp2,dir; bool alive; };
    static const int MAX_EN=6;
    static Enemy ens[MAX_EN];

    static uint16_t tileCol(uint8_t t){
        switch(t){
            case T_WALL:   return tft.color565(20,30,60);
            case T_FLOOR:  return tft.color565(8,12,26);
            case T_DOOR:   return tft.color565(80,50,10);
            case T_KEY:    return C_YELLOW;
            case T_CHEST:  return tft.color565(60,40,5);
            case T_STAIRS: return C_GREEN;
            case T_SPIKE:  return C_RED;
        }
        return C_BG;
    }

    static void drawTile(int tx, int ty){
        int px2=OX+tx*TW, py2=OY+ty*TH;
        uint8_t t=dmap[ty][tx];
        tft.fillRect(px2,py2,TW-1,TH-1,tileCol(t));
        if(t==T_WALL){
            tft.drawRect(px2,py2,TW-1,TH-1,tft.color565(30,40,80));
        } else if(t==T_KEY){
            tft.fillCircle(px2+6,py2+4,3,C_YELLOW);
            tft.fillRect(px2+4,py2+6,5,2,C_YELLOW);
            tft.fillRect(px2+4,py2+8,2,2,C_YELLOW);
        } else if(t==T_CHEST){
            tft.fillRect(px2+1,py2+4,TW-3,TH-6,tft.color565(100,70,10));
            tft.drawRect(px2+1,py2+4,TW-3,TH-6,C_YELLOW);
            tft.fillRect(px2+5,py2+4,4,2,C_YELLOW);
        } else if(t==T_STAIRS){
            for(int s=0;s<3;s++) tft.fillRect(px2+s*4,py2+s*4,4,TH-1-s*4,tft.color565(0,120,50));
        } else if(t==T_SPIKE){
            tft.fillTriangle(px2+2,py2+TH-1,px2+6,py2+2,px2+10,py2+TH-1,C_RED);
        } else if(t==T_DOOR){
            tft.fillRect(px2+2,py2+1,TW-5,TH-2,tft.color565(100,65,15));
            tft.drawRect(px2+2,py2+1,TW-5,TH-2,tft.color565(150,100,30));
            tft.fillCircle(px2+8,py2+7,1,C_YELLOW);
        }
    }

    // Pixel-art hero: cyan armored knight 12x12
    static void drawHero(int tx,int ty,bool erase){
        int x=OX+tx*TW+1, y=OY+ty*TH+1;
        if(erase){ tft.fillRect(x,y,TW-2,TH-2,tileCol(dmap[ty][tx])); return; }
        // Helmet
        tft.fillRect(x+2,y,8,3,tft.color565(0,180,220));
        tft.fillRect(x+3,y+1,6,2,tft.color565(100,230,255));
        // Face
        tft.fillRect(x+2,y+3,8,4,tft.color565(255,200,150));
        tft.fillRect(x+3,y+4,2,1,tft.color565(50,30,10)); // left eye
        tft.fillRect(x+7,y+4,2,1,tft.color565(50,30,10)); // right eye
        // Body armor
        tft.fillRect(x+1,y+7,10,4,tft.color565(0,140,180));
        tft.fillRect(x+2,y+8,8,2,tft.color565(0,200,240));
        // Sword glint
        tft.fillRect(x+10,y+3,2,6,tft.color565(200,220,255));
        tft.drawPixel(x+11,y+2,C_WHITE);
    }

    // Pixel-art enemy: red goblin 10x10
    static void drawEnemy(int i, bool erase){
        Enemy& e=ens[i];
        int tx=erase?e.ox:e.x;
        int ty=erase?e.oy:e.y;
        int x=OX+tx*TW+2, y=OY+ty*TH+2;
        if(erase){ tft.fillRect(x,y,TW-4,TH-4,tileCol(dmap[ty][tx])); return; }
        // Body
        tft.fillRect(x+1,y+3,8,6,tft.color565(180,40,40));
        // Head
        tft.fillRect(x+2,y,6,5,tft.color565(120,200,60));
        // Eyes glowing red
        tft.fillRect(x+2,y+1,2,1,C_RED);
        tft.fillRect(x+6,y+1,2,1,C_RED);
        // Claws
        tft.drawPixel(x,y+8,tft.color565(255,200,100));
        tft.drawPixel(x+9,y+8,tft.color565(255,200,100));
        // HP bar above
        int hw=(e.hp2*8)/4; if(hw>8)hw=8;
        tft.fillRect(x+1,y-2,8,2,tft.color565(40,0,0));
        if(hw>0) tft.fillRect(x+1,y-2,hw,2,C_GREEN);
    }

    static void drawAll(){
        for(int gy=0;gy<MH;gy++) for(int gx=0;gx<MW;gx++) drawTile(gx,gy);
        for(int i=0;i<MAX_EN;i++) if(ens[i].alive){ ens[i].ox=ens[i].x; ens[i].oy=ens[i].y; drawEnemy(i,false); }
        drawHero(px,py,false);
        statusBar("DARK DUNGEON",score,hi);
        tft.fillRect(0,206,240,14,tft.color565(0,0,0));
        tft.fillRect(4,208,80,8,tft.color565(30,0,0));
        int hpw=(hp*80)/maxHp; if(hpw>0) tft.fillRect(4,208,hpw,8,C_RED);
        tft.setTextSize(1); tft.setTextColor(C_WHITE); tft.setCursor(86,208);
        char hb[16]; snprintf(hb,16,"HP%d KEY:%s L%d",hp,hasKey?"Y":"N",level);
        tft.print(hb);
        tft.fillRect(0,222,240,10,tft.color565(0,0,0));
        cText("DPAD=Move  A=Attack  B=Pause",224,1,tft.color565(30,40,65));
    }

    static bool isWalkable(int tx,int ty){
        if(tx<0||tx>=MW||ty<0||ty>=MH) return false;
        uint8_t t=dmap[ty][tx];
        return t!=T_WALL;
    }

    static void genMap(){
        memset(dmap,T_WALL,sizeof(dmap));
        int rooms=3+level/3; if(rooms>6) rooms=6;
        for(int r=0;r<rooms;r++){
            int rw=3+random(4), rh=3+random(3);
            int rx=1+random(MW-rw-2), ry=1+random(MH-rh-2);
            for(int y=ry;y<ry+rh;y++) for(int x=rx;x<rx+rw;x++) dmap[y][x]=T_FLOOR;
            if(r>0){
                int cx2=rx+rw/2, cy2=ry+rh/2, px2=MW/2, py2=MH/2;
                while(cx2!=px2){ dmap[cy2][cx2]=T_FLOOR; cx2+=(px2>cx2?1:-1); }
                while(cy2!=py2){ dmap[cy2][cx2]=T_FLOOR; cy2+=(py2>cy2?1:-1); }
            }
        }
        px=MW/2; py=MH/2;
        while(dmap[py][px]!=T_FLOOR){ px=(px+1)%MW; }
        opx=px; opy=py;
        // Place items
        auto place=[](uint8_t tile){
            for(int a=0;a<20;a++){int x=1+random(MW-2),y=1+random(MH-2);if(dmap[y][x]==T_FLOOR){dmap[y][x]=tile;return;}}
        };
        place(T_STAIRS); place(T_KEY);
        for(int a=0;a<10;a++){int x=1+random(MW-2),y=1+random(MH-2);if(dmap[y][x]==T_WALL&&(x==0||x==MW-1||y==0||y==MH-1)){dmap[y][x]=T_DOOR;break;}}
        int chests=1+level/4; for(int c=0;c<chests;c++) place(T_CHEST);
        int spikes=level/3;   for(int s=0;s<spikes;s++) place(T_SPIKE);
        // Enemies
        int enCount=min(1+level/2,MAX_EN);
        for(int i=0;i<MAX_EN;i++) ens[i].alive=false;
        for(int i=0;i<enCount;i++){
            for(int a=0;a<20;a++){
                int ex=1+random(MW-2),ey=1+random(MH-2);
                if(dmap[ey][ex]==T_FLOOR&&!(ex==px&&ey==py)){
                    ens[i]={ex,ey,ex,ey,2+level/3,random(4),true}; break;
                }
            }
        }
        hasKey=false;
    }

    static void moveEnemies(){
        for(int i=0;i<MAX_EN;i++){
            if(!ens[i].alive) continue;
            drawEnemy(i,true); // erase old pos
            ens[i].ox=ens[i].x; ens[i].oy=ens[i].y;
            int mx=(px>ens[i].x?1:(px<ens[i].x?-1:0));
            int my=(py>ens[i].y?1:(py<ens[i].y?-1:0));
            int nx=ens[i].x+(random(3)==0?(random(3)-1):mx);
            int ny=ens[i].y+(random(3)==0?(random(3)-1):my);
            if(nx==px&&ny==py){ hp--; sfx_hit(); }
            else if(isWalkable(nx,ny)){
                bool blocked=false;
                for(int j=0;j<MAX_EN;j++) if(j!=i&&ens[j].alive&&ens[j].x==nx&&ens[j].y==ny) blocked=true;
                if(!blocked){ ens[i].x=nx; ens[i].y=ny; }
            }
            drawTile(ens[i].ox,ens[i].oy); // redraw tile under old pos
            drawEnemy(i,false);
        }
    }
}

void darkDungeonInit(int lv){
    using namespace DD;
    level=lv; score=0; hi=hiScore[0];
    hp=maxHp=8+lv; paused=false; frameC=0;
    ft=FrameTimer(180-min(lv*8,130));
    genMap();
    tft.fillScreen(C_BG);
    drawAll();
}

void darkDungeonLoop(){
    using namespace DD;
    if(paused){
        uint8_t r=showPauseMenu();
        if(r==1){ paused=false; tft.fillScreen(C_BG); drawAll(); }
        else if(r==2){ savedLevel[0]=level; saveData(); darkDungeonInit(1); }
        else{ savedLevel[0]=level; saveData(); appState=APP_HOME; homeScreen(); }
        return;
    }
    if(!ft.ready()) return;
    frameC++;

    uint8_t p=btnPressed();
    if(p&BTN_B){ paused=true; return; }

    int dx2=0, dy2=0;
    if(p&BTN_UP)    dy2=-1;
    if(p&BTN_DOWN)  dy2= 1;
    if(p&BTN_LEFT)  dx2=-1;
    if(p&BTN_RIGHT) dx2= 1;

    if(dx2||dy2){
        int nx=px+dx2, ny=py+dy2;
        if(nx>=0&&nx<MW&&ny>=0&&ny<MH){
            uint8_t t=dmap[ny][nx];
            bool hitEnemy=false;
            if(p&BTN_A){
                for(int i=0;i<MAX_EN;i++){
                    if(ens[i].alive&&ens[i].x==nx&&ens[i].y==ny){
                        ens[i].hp2-=(1+level/4); sfx_hit();
                        if(ens[i].hp2<=0){ ens[i].alive=false; drawTile(nx,ny); score+=20+level*5; sfx_coin(); }
                        else { drawEnemy(i,false); }
                        hitEnemy=true; break;
                    }
                }
            }
            if(!hitEnemy && t!=T_WALL){
                if(t==T_DOOR && !hasKey){ sfx_wrong(); }
                else {
                    // Erase hero from old pos
                    drawHero(px,py,true);
                    opx=px; opy=py;
                    px=nx; py=ny;
                    sfx_step();
                    if(t==T_KEY)    { dmap[ny][nx]=T_FLOOR; hasKey=true; sfx_coin(); drawTile(nx,ny); }
                    else if(t==T_DOOR)   { dmap[ny][nx]=T_FLOOR; hasKey=false; sfx_door(); drawTile(nx,ny); }
                    else if(t==T_CHEST)  { dmap[ny][nx]=T_FLOOR; score+=50+level*10; hp=min(hp+2,maxHp); sfx_chest(); drawTile(nx,ny); }
                    else if(t==T_SPIKE)  { hp-=2; sfx_hit(); vibroPulse(180,80); }
                    else if(t==T_STAIRS) {
                        level++; if(level>15) level=15;
                        savedLevel[0]=level; score+=100+level*20;
                        if(score>hi){ hi=score; hiScore[0]=hi; }
                        showLevelUp(level,C_CYAN);
                        genMap(); tft.fillScreen(C_BG); drawAll(); return;
                    }
                    drawHero(px,py,false);
                }
            }
        }
    }

    if(frameC%3==0) moveEnemies();
    if(score>hi){ hi=score; hiScore[0]=hi; }
    if(frameC%10==0) statusBar("DARK DUNGEON",score,hi);

    if(hp<=0){
        bool retry=showGameOver(score,hi,score>hiScore[0]);
        if(score>hiScore[0]) hiScore[0]=score;
        gamePlayed[0]=true; saveData();
        if(retry) darkDungeonInit(1);
        else{ appState=APP_HOME; homeScreen(); }
    }
}