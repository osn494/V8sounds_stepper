/*
 * File:   agt2.c
 *
 * Created on 2017/09/03, 21:15
 */

#include <xc.h>
#include "init_1827.h"

//CCP1-PB0/6PIN*RB3/9PIN
//CCP2-PB6/12PIN*RA7/16PIN
//CCP3-RA3/2PIN
//CCP4-RA4/3PIN

//80rpmで振動収まる//240rpmで自励起不能//320rpmで脱調

#define MAX_RPM_CNT 10  //最速回転時のIWM分解能あたりのTMR2待ち時間

//int sin_table[9]={0,50,98,142,180,212,236,250,255};
//int sin_table[9]={0,32,64,96,128,159,191,223,255};
int sin_table[8] = {50, 98, 142, 180, 212, 236, 250, 255};
//int sin_table[8]={12,24,35,45,52,58,62,63}; //8bit

//int V8_table[16] = {110, 220, 82, 183, 136, 142, 76, 72, 170, 204, 42, 130, 145, 192, 146, 0}; //16bit、V8音源変調率100％バージョン(かけた後〉〉8)
//int V8_table[16] = {156,183,148,174,162,164,147,146,170,179,138,161,164,176,164,128}; //16bit、V8音源変調率50％バージョン(かけた後〉〉4)
//int V8_table[16] = {126,194,111,152,129,130,107,104,142,173,70,128,130,160,130,0}; //16bit、V8音源変調率100％そして二乗バージョン(かけた後〉〉8)
int V8_table[16] = {159,177,156,166,160,160,155,154,163,171,146,160,161,168,161,128}; //16bit、V8音源変調率50％そして二乗バージョン(かけた後〉〉4)

int adc_velocity; //bit_angleインクリメント待ち時間。ADCの変数。
int bit_angle; //角度。80カウントで90度
int quadrant = 1; //象限

int cos_culc(int cos_angle_val); //引数はbit_angle
int sin_culc(int sin_angle_val); //引数はbit_angle
void output(void); //PWMセッティング

void main(void) {

    init();
    ADC_init();
    PWM_init();

    PIE1bits.TMR2IE = 1;


    while (1) {
        output();
    }

    return;
}

void output() {
    //1サイクルで8*4=32データを供給
    int sin_out, cos_out;
    int sin_out_L, cos_out_L;
    static V8_num = 0;
    sin_out = sin_culc(bit_angle);
    cos_out = cos_culc(bit_angle);
    
    if (RA1 == 1) {

        cos_out = (cos_out * V8_table[V8_num]) >> 4;
        sin_out = (sin_out * V8_table[V8_num]) >> 4;

        V8_num++;
        if (V8_num == 16) {
            V8_num = 0;
        }
    }
    /*
    switch(quadrant){
                
        case 1:
            CCPR1L = cos_out;
            //DC1B = 
            CCPR2L = sin_out;
            CCPR3L = 0;
            CCPR4L = 0;
            break;
        case 2:
            CCPR1L = 0;
            CCPR2L = cos_out;
            CCPR3L = sin_out;
            CCPR4L = 0;
            break;
        case 3:
            CCPR1L = 0;
            CCPR2L = 0;
            CCPR3L = cos_out;
            CCPR4L = sin_out;
            break;
        case 4:
            CCPR1L = sin_out;
            CCPR2L = 0;
            CCPR3L = 0;
            CCPR4L = cos_out;
    }*/
    int PWM_DUTY[4] = {0, 0, 0, 0};
    switch (quadrant) {

        case 1:
            PWM_DUTY[0] = cos_out;
            PWM_DUTY[1] = sin_out;
            PWM_DUTY[2] = 0;
            PWM_DUTY[3] = 0;
            break;
        case 2:
            PWM_DUTY[0] = 0;
            PWM_DUTY[1] = cos_out;
            PWM_DUTY[2] = sin_out;
            PWM_DUTY[3] = 0;
            break;
        case 3:
            PWM_DUTY[0] = 0;
            PWM_DUTY[1] = 0;
            PWM_DUTY[2] = cos_out;
            PWM_DUTY[3] = sin_out;
            break;
        case 4:
            PWM_DUTY[0] = sin_out;
            PWM_DUTY[1] = 0;
            PWM_DUTY[2] = 0;
            PWM_DUTY[3] = cos_out;
    }

    //PWM_DUTY[quadrant % 4] = sin_out;
    //PWM_DUTY[(quadrant + 1) % 4] = cos_out;

    CCP1CONbits.DC1B = PWM_DUTY[4] && 0b11;
    CCPR1L = PWM_DUTY[0] >> 2;
    CCP2CONbits.DC2B = PWM_DUTY[1] && 0b11;
    CCPR2L = PWM_DUTY[1] >> 2;
    CCP3CONbits.DC3B = PWM_DUTY[2] && 0b11;
    CCPR3L = PWM_DUTY[2] >> 2;
    CCP4CONbits.DC4B = PWM_DUTY[3] && 0b11;
    CCPR4L = PWM_DUTY[3] >> 2;

}

int cos_culc(int cos_angle_val) { //引数はbit_angle
    return sin_culc(7 - cos_angle_val);
}

int sin_culc(int sin_angle_val) { //引数はbit_angle
    /*
    if(bit_angle < 2){
        sin_angle_val = sin_angle_val;
    }
    else if(bit_angle < 4){
        sin_angle_val = (sin_angle_val - 16) >> 1 +16;
    }
    else if(bit_angle < 6){
        sin_angle_val = (sin_angle_val - 32) >> 2 +16+8;
    }
    else if(bit_angle < 8){
        sin_angle_val = (sin_angle_val - 48) >> 3 +16+8+4;
    }
     */

    return sin_table[sin_angle_val];
}

void interrupt intr() {

    //TMR2割り込み
    if (PIR1bits.TMR2IF == 1) {//
        static int TMR2_counter = 0;
        TMR2_counter++;
        if (TMR2_counter > MAX_RPM_CNT + adc_velocity) {
            bit_angle++;
            TMR2_counter = 0;
            if (bit_angle >= 8) {
                quadrant++; //象限
                bit_angle = 0;
                if (quadrant > 4) {
                    quadrant = 1;
                }
            }
        }

        ADCON0bits.GO_nDONE = 1; //変換開始
        PIR1bits.TMR2IF = 0; //
    }

    //ADC割り込み
    if (PIR1bits.ADIF == 1) {//
        adc_velocity = ADRESH;
        PIR1bits.ADIF = 0;
    }
}