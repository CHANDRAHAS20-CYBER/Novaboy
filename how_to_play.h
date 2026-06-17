#pragma once
#include "globals.h"
#include "audio.h"
#include "ui.h"

static const char* HTP_TITLE[NUM_GAMES]={
    "DARK DUNGEON","MICRO CITY","CAR DODGE","ARDU GOLF","NEON SNAKE",
    "MICRO TD","PIXEL PONG","TANK STRIKE","TINY SOKOBAN","OMEGA HORIZON",
    "BANGI","STAR SHOOTER","NEON STACKER","CUBE PIPES","REACTION BLITZ"
};
static const char* HTP_L1[NUM_GAMES]={
    "Explore dungeons — collect loot — survive",
    "Zone land to grow your city population",
    "Dodge traffic — collect coins — survive!",
    "Aim & power your shot to reach the hole",
    "Eat food, grow longer, don't crash",
    "Place towers to stop waves of enemies",
    "Deflect the ball — first to 7 wins!",
    "Blast enemy tanks before they get you",
    "Push crates onto target squares",
    "Run & dodge — survive as long as you can",
    "Deflect the ball — keep it in play",
    "Shoot enemies — dodge their fire!",
    "Stack falling blocks — clear full rows to score!",
    "Rotate pipes to connect source to sink",
    "React fast to on-screen prompts — press right button!"
};
static const char* HTP_L2[NUM_GAMES]={
    "D-PAD=Move  A=Attack  B=Pause",
    "D-PAD=Move cursor  A=Place zone  B=Menu",
    "LEFT/RIGHT=Change lane  A=Speed boost  B=Pause",
    "LEFT/RIGHT=Aim  A=Power up  release=Shoot",
    "D-PAD=Direction  B=Pause",
    "D-PAD=Move cursor  A=Place tower  B=Pause",
    "UP/DOWN=Move paddle  B=Pause",
    "D-PAD=Move  A=Fire  B=Pause",
    "D-PAD=Push crate  B=Undo last move",
    "LEFT/RIGHT=Dodge  A=Jump  B=Pause",
    "LEFT/RIGHT=Move paddle  B=Pause",
    "LEFT/RIGHT=Move  A=Shoot  B=Pause",
    "LEFT/RIGHT=Move  DOWN=Drop  A=Rotate",
    "D-PAD=Select pipe  A=Rotate  B=Pause",
    "Watch the prompt — press correct button FAST!"
};
static const char* HTP_L3[NUM_GAMES]={
    "B=Pause  Collect keys to open doors",
    "B=Pause  Balance residential+commercial",
    "B=Pause  3 lives — don't crash!",
    "B=Pause  Par determines level score",
    "Speed increases every level",
    "B=Pause  Upgrade towers with gold",
    "B=Pause  7 points wins the round!",
    "B=Pause  3 lives — don't get hit!",
    "B=Pause  Plan before you push!",
    "B=Pause  Collect power-ups to survive",
    "B=Pause  Miss 3 times = game over",
    "B=Pause  Kill all enemies to level up",
    "B=Pause  Clear lines to level up!",
    "B=Pause  Complete before water flows",
    "B=Pause  Don't press early — false start = life lost!"
};
static const char* HTP_TIP[NUM_GAMES]={
    "Clear rooms fully before moving deeper!",
    "Residential next to park = double score!",
    "Stay in middle lanes — more room to dodge!",
    "Wind affects long shots — watch the arrow!",
    "Plan 3 moves ahead — corners are traps!",
    "Slow towers + damage towers = best combo!",
    "Aim for corners — CPU struggles with them!",
    "Strafe sideways to dodge their shots!",
    "Mark your exit before pushing any crate!",
    "Dash through gap clusters, not around them!",
    "Small paddle taps are more accurate!",
    "Shoot enemies as soon as they appear!",
    "Keep the stack flat — avoid leaving gaps!",
    "Start from the sink end and work backwards!",
    "Faster reaction = more points — aim under 200ms!"
};
static const uint16_t HTP_COL[NUM_GAMES]={
    0x07FF,0x07E0,0xF800,0x3FE0,0x07E0,
    0xFD20,0xF81F,0xFC60,0xFFE0,0xFC00,
    0xF8DF,0x07FF,0xF81F,0xFBE0,0x07FF
};

static int htpGameIdx=-1;

static void htpDraw(int g){
    tft.fillScreen(C_BG);
    uint16_t c=HTP_COL[g];
    tft.fillRect(0,0,240,30,tft.color565(0,0,0));
    tft.drawFastHLine(0,30,240,c);
    glowText("HOW  TO  PLAY",7,1,c);
    tft.setTextColor(tft.color565(38,48,78)); tft.setTextSize(1);
    tft.setCursor(4,18); tft.print("B:Back   A:Play");
    tft.fillRoundRect(16,34,208,24,7,tft.color565(0,0,0));
    tft.drawRoundRect(16,34,208,24,7,c);
    glowText(HTP_TITLE[g],38,2,c);
    neonPanel(6,62,228,86,tft.color565(20,28,56));
    tft.setTextSize(1); tft.setTextColor(C_WHITE);
    tft.setCursor(12,68); tft.print(HTP_L1[g]);
    tft.setTextColor(C_CYAN);
    tft.setCursor(12,82); tft.print(HTP_L2[g]);
    tft.setTextColor(tft.color565(140,150,190));
    tft.setCursor(12,96); tft.print(HTP_L3[g]);
    tft.setTextColor(tft.color565(90,100,140));
    tft.drawFastHLine(12,110,216,tft.color565(18,26,50));
    tft.setCursor(12,114); tft.print("Levels 1-15  |  Progress auto-saved");
    tft.fillRoundRect(6,154,228,48,7,tft.color565(28,20,0));
    tft.drawRoundRect(6,154,228,48,7,C_YELLOW);
    tft.fillRect(6,154,228,18,tft.color565(48,34,0));
    tft.drawFastHLine(6,172,228,C_YELLOW);
    tft.setTextColor(C_YELLOW); tft.setCursor(12,158); tft.print("PRO TIP:");
    tft.setTextColor(tft.color565(210,195,110));
    tft.setCursor(12,176); tft.print(HTP_TIP[g]);
    neonBtn(8,208,108,26,"A   PLAY NOW",C_GREEN,true);
    neonBtn(124,208,108,26,"B   BACK",C_RED,true);
    tft.fillRect(0,236,240,4,tft.color565(0,0,0));
}

void showHowToPlay(int gidx){ htpGameIdx=gidx; htpDraw(gidx); appState=APP_HOW_TO_PLAY; }

void howToPlayLoop(){
    uint8_t p=btnPressed();
    if(p&BTN_A){ sfx_select(); launchGame(htpGameIdx,false); }
    if(p&BTN_B){ sfx_back(); appState=APP_MENU; menuInit(); }
    vTaskDelay(1);
}