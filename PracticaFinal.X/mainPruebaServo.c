#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"
#define PIN_PULSADOR 5
#define INC_10_GRADOS 139

int main(void){

    int puls_ant, puls_act;
    int t_alto = 2500;
    
    ANSELC = ~0xF;
    
    LATA = 0;
    LATB = 0;
    LATC = 0xF;
    
    TRISA = 0;
    TRISB = (1<<PIN_PULSADOR);
    TRISC = 1<<7;
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPB15R = 5;
    SYSKEY = 0x1CA11CA1;
    
    OC1CON = 0;
    OC1R = 2500;
    OC1RS = 2500;
    OC1CON = 0x8006;
    
    T2CON = 0;
    TMR2 = 0;
    PR2 = 49999;
    T2CON = 0x8010;
    
    puls_ant = (PORTB >> PIN_PULSADOR)&1; 
    
    while(1){
        puls_act = (PORTB >> PIN_PULSADOR)&1; 
        if( (puls_act != puls_ant) && (puls_act == 0) ){
            t_alto += 2500;
            if(t_alto > 5000){
                t_alto = 2500;
            }
            OC1RS = t_alto;
        }
        puls_ant = puls_act;
    }
            
}

