/*
 * File:   INIT_1827.c
 */
#include <xc.h>
 #include <stdio.h>
 #include "init_1827.h"

void init(){
    //OSCCON = 0b11110000; //32MHz
    OSCCONbits.SCS = 0b00;  //�N���b�N�I���r�b�g
    OSCCONbits.IRCF = 0b1110;   //8MHz
    OSCCONbits.SPLLEN = 1;  //4xPLL��L��
    //OSCCONbits.IRCF = 0b1110;   //8MHz HFINTOSC
    
    //�|�[�g�ݒ�
    TRISA = 0b00000110;
    TRISB = 0b00000000;
    
    ANSELA = 0b00000100;
    ANSELB = 0b00000000;  

    PORTA = 0b00000000;
    PORTB = 0b00000000;

    OPTION_REGbits.nWPUEN = 0;  //���ׂĂ̎�v���A�b�v��[0:�L��,1:����]
    OPTION_REGbits.INTEDG = 0;  //RB0��[0:������,1:�����オ��]�Ŋ��荞��
    OPTION_REGbits.TMR0CS = 0;  //TMR0�N���b�N��[0:Fosc/4,1:PA4]
    OPTION_REGbits.PSA = 0; //�v���X�P�[����TMR0�Ɋ��蓖��[0:�Ȃ�,1:��]
    OPTION_REGbits.PS = 0b000;  //�v���X�P�[�������r�b�g

    //�v���A�b�v�ݒ�    
    /*
    WPUA = 0b00000000;
    WPUB = 0b00000000;*/
    
    INTCONbits.GIE = 1; //�����荞�݋���
}
void ADC_init(void){
    //�`�����l���ݒ�
    ADCON0bits.CHS = 2;
    //�Q�Ɠd��
    ADCON1bits.ADNREF = 0;  //-�Q�Ɠd����Vss��I��(1827�p)
    ADCON1bits.ADPREF = 0b11;  //+�Q�Ɠd����[00:Vdd,10:Vref+:,11:FVR]��I��
    FVR_init();
    //�N���b�N��
    ADCON1bits.ADCS = 0b010;  //ADC�N���b�N����TAD=1.0us��I��(32MHz�ɂ����čō���))
    ADCON1bits.ADCS = 0b011;  //��p�I�V���[�^
    //���荞��
    PIR1bits.ADIF = 0;  //�t���O�N���A
    PIE1bits.ADIE = 0;
    //�ϊ��t�H�[�}�b�g�ݒ�
    ADCON1bits.ADFM = 0;   //���8bit��ADRESH��
    //ADCON1bits.ADFM = 1;   //����8bit��ADRESL��
    
    //ADC�L��
    ADCON0bits.ADON = 1;
    //ADCON0bits.GO_nDONE = 1�����s��ADC�J�n
}
void TMR2_init(void){

//TMR2�ݒ�
    PR2 = 63;//255;      //duty�ő�l��ݒ�
    
    T2CONbits.TMR2ON = 1;  //TMR2���N��
    T2CONbits.T2CKPS = 0b0;    //�v���X�P�[���ݒ�x-1
    T2CONbits.T2OUTPS = 0b0000; //�|�X�g�X�P�[��4^x
    //�����܂ł�PWM��31.25kHz8bit(10bit)�ŉғ�����
    
//���荞�ݐݒ�
    //INTCON = 0;     //���荞�ݐ��䃌�W�X�^
    INTCONbits.PEIE = 1; //���Ӌ@�\���荞�݋���
    //PIE1bits.TMR2IE = 1;
    //���Ƃ�PIE1bits.TMR2IE = 1;��錾���邾��  //
}
void FVR_init(void){
    FVRCONbits.FVREN = 1;   //FVR�L��
    
    FVRCONbits.ADFVR = 0b11;    //FVR��ADC�o�͂�4�{
        
    while(FVRCONbits.FVRRDY == 0){   //FVR�̗����オ���҂�
    }
}
void PWM_init(void){
    
    CCP1CONbits.CCP1M = 0b1100; //PWM���[�h�ŋN��
    CCP2CONbits.CCP2M = 0b1100; //PWM���[�h�ŋN��
    CCP3CONbits.CCP3M = 0b1100; //PWM���[�h�ŋN��
    CCP4CONbits.CCP4M = 0b1100; //PWM���[�h�ŋN��

    //PR2 = 255;    PWM����\MAX
    TMR2_init();
}
void IOC_init(void){
    
}    //��ԕω����荞��