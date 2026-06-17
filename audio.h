#pragma once
#include "globals.h"

#define SAMPLE_RATE  16000
#define I2S_PORT     I2S_NUM_0
#define AMPLITUDE    16000
#define WAVE_BUF     256

static int16_t _wb[WAVE_BUF];

void i2s_init(){
    i2s_config_t cfg = {};
    cfg.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    cfg.sample_rate          = SAMPLE_RATE;
    cfg.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.channel_format       = I2S_CHANNEL_FMT_ONLY_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;
    cfg.dma_buf_count        = 8;
    cfg.dma_buf_len          = 256;
    cfg.use_apll             = false;
    cfg.tx_desc_auto_clear   = true;
    i2s_pin_config_t pins = {};
    pins.bck_io_num   = I2S_BCLK;
    pins.ws_io_num    = I2S_LRC;
    pins.data_out_num = I2S_DOUT;
    pins.data_in_num  = I2S_PIN_NO_CHANGE;
    i2s_driver_install(I2S_PORT, &cfg, 0, NULL);
    i2s_set_pin(I2S_PORT, &pins);
    i2s_zero_dma_buffer(I2S_PORT);
}

void playTone(float freq, int ms, float vol = 1.0f){
    if(gVolume == 0) return;
    float ev = vol * (gVolume / 10.0f);
    if(freq < 1.0f){
        memset(_wb, 0, sizeof(_wb));
        int tot = (SAMPLE_RATE * ms) / 1000;
        for(int i = 0; i < tot; i += WAVE_BUF){ size_t w; i2s_write(I2S_PORT,_wb,sizeof(_wb),&w,portMAX_DELAY); }
        return;
    }
    float ph=0, inc=2.0f*M_PI*freq/SAMPLE_RATE;
    int rem=(SAMPLE_RATE*ms)/1000;
    while(rem>0){
        int c=min(rem,WAVE_BUF);
        for(int i=0;i<c;i++){_wb[i]=(int16_t)(sinf(ph)*AMPLITUDE*ev);ph+=inc;if(ph>2*M_PI)ph-=2*M_PI;}
        for(int i=c;i<WAVE_BUF;i++)_wb[i]=0;
        size_t w; i2s_write(I2S_PORT,_wb,WAVE_BUF*2,&w,portMAX_DELAY);
        rem-=c;
    }
}

void sfx_beep()    { playTone(880,  25, 0.35f); }
void sfx_ui()      { playTone(800,  15, 0.3f);  }
void sfx_select()  { playTone(600,  15, 0.3f); playTone(900,15,0.4f); }
void sfx_back()    { playTone(400,  18, 0.28f); playTone(260,18,0.28f); }
void sfx_start()   { playTone(330,  45, 0.5f);  playTone(523,45,0.6f); playTone(784,65,0.7f); }
void sfx_die()     { playTone(280,  70, 0.7f);  playTone(170,90,0.7f); playTone(100,110,0.6f); }
void sfx_score()   { playTone(784,  28, 0.5f);  playTone(1047,38,0.6f); }
void sfx_powerup() { playTone(523,  38, 0.5f);  playTone(659,38,0.6f); playTone(784,38,0.7f); playTone(1047,65,0.8f); }
void sfx_hit()     { playTone(350,  32, 0.5f);  playTone(220,22,0.4f); }
void sfx_coin()    { playTone(1047, 22, 0.5f);  playTone(1319,32,0.6f); }
void sfx_wrong()   { playTone(180,  52, 0.5f);  playTone(130,65,0.4f); }
void sfx_level()   { playTone(523,  45, 0.6f);  playTone(659,45,0.7f); playTone(784,45,0.8f); playTone(1047,80,0.9f); }
void sfx_tap()     { playTone(1100, 10, 0.38f); }
void sfx_explode() { playTone(220,  52, 0.7f);  playTone(150,65,0.6f); }
void sfx_shoot()   { playTone(1100, 15, 0.38f); playTone(650,15,0.28f); }
void sfx_jump()    { playTone(400,  28, 0.38f); playTone(600,28,0.48f); }
void sfx_match()   { playTone(659,  42, 0.6f);  playTone(880,42,0.7f); playTone(1100,65,0.8f); }
void sfx_laser()   { playTone(1400, 12, 0.4f);  playTone(900,12,0.3f); }
void sfx_success() { playTone(523,  55, 0.6f);  playTone(784,55,0.7f); playTone(1047,90,0.9f); }
void sfx_buzz()    { playTone(160,  60, 0.55f); playTone(120,60,0.5f); }
void sfx_ping()    { playTone(1200, 20, 0.4f);  }
void sfx_step()    { playTone(300,  8,  0.2f);  }
void sfx_door()    { playTone(440,  30, 0.4f);  playTone(660,30,0.5f); }
void sfx_chest()   { playTone(880,  20, 0.5f);  playTone(1100,20,0.5f); playTone(1320,40,0.7f); }