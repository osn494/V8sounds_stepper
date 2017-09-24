/*
 * File:   INIT_1827.c
 */
#include <xc.h>
 #include <stdio.h>
 #include "init_1827.h"

void init(){
    //OSCCON = 0b11110000; //32MHz
    OSCCONbits.SCS = 0b00;  //クロック選択ビット
    OSCCONbits.IRCF = 0b1110;   //8MHz
    OSCCONbits.SPLLEN = 1;  //4xPLLを有効
    //OSCCONbits.IRCF = 0b1110;   //8MHz HFINTOSC
    
    //ポート設定
    TRISA = 0b00000110;
    TRISB = 0b00000000;
    
    ANSELA = 0b00000100;
    ANSELB = 0b00000000;  

    PORTA = 0b00000000;
    PORTB = 0b00000000;

    OPTION_REGbits.nWPUEN = 0;  //すべての弱プルアップを[0:有効,1:無効]
    OPTION_REGbits.INTEDG = 0;  //RB0の[0:立下り,1:立ち上がり]で割り込み
    OPTION_REGbits.TMR0CS = 0;  //TMR0クロック源[0:Fosc/4,1:PA4]
    OPTION_REGbits.PSA = 0; //プリスケーラをTMR0に割り当て[0:ない,1:る]
    OPTION_REGbits.PS = 0b000;  //プリスケーラ割当ビット

    //プルアップ設定    
    /*
    WPUA = 0b00000000;
    WPUB = 0b00000000;*/
    
    INTCONbits.GIE = 1; //総割り込み許可
}
void ADC_init(void){
    //チャンネル設定
    ADCON0bits.CHS = 2;
    //参照電圧
    ADCON1bits.ADNREF = 0;  //-参照電圧にVssを選択(1827用)
    ADCON1bits.ADPREF = 0b11;  //+参照電圧に[00:Vdd,10:Vref+:,11:FVR]を選択
    FVR_init();
    //クロック源
    ADCON1bits.ADCS = 0b010;  //ADCクロック周期TAD=1.0usを選択(32MHzにおいて最高速))
    ADCON1bits.ADCS = 0b011;  //専用オシレータ
    //割り込み
    PIR1bits.ADIF = 0;  //フラグクリア
    PIE1bits.ADIE = 0;
    //変換フォーマット設定
    ADCON1bits.ADFM = 0;   //上位8bitをADRESHに
    //ADCON1bits.ADFM = 1;   //下位8bitをADRESLに
    
    //ADC有効
    ADCON0bits.ADON = 1;
    //ADCON0bits.GO_nDONE = 1を実行しADC開始
}
void TMR2_init(void){

//TMR2設定
    PR2 = 63;//255;      //duty最大値を設定
    
    T2CONbits.TMR2ON = 1;  //TMR2を起動
    T2CONbits.T2CKPS = 0b0;    //プリスケーラ設定x-1
    T2CONbits.T2OUTPS = 0b0000; //ポストスケーラ4^x
    //ここまででPWMは31.25kHz8bit(10bit)で稼働する
    
//割り込み設定
    //INTCON = 0;     //割り込み制御レジスタ
    INTCONbits.PEIE = 1; //周辺機能割り込み許可
    //PIE1bits.TMR2IE = 1;
    //あとはPIE1bits.TMR2IE = 1;を宣言するだけ  //
}
void FVR_init(void){
    FVRCONbits.FVREN = 1;   //FVR有効
    
    FVRCONbits.ADFVR = 0b11;    //FVRのADC出力を4倍
        
    while(FVRCONbits.FVRRDY == 0){   //FVRの立ち上がりを待つ
    }
}
void PWM_init(void){
    
    CCP1CONbits.CCP1M = 0b1100; //PWMモードで起動
    CCP2CONbits.CCP2M = 0b1100; //PWMモードで起動
    CCP3CONbits.CCP3M = 0b1100; //PWMモードで起動
    CCP4CONbits.CCP4M = 0b1100; //PWMモードで起動

    //PR2 = 255;    PWM分解能MAX
    TMR2_init();
}
void IOC_init(void){
    
}    //状態変化割り込み