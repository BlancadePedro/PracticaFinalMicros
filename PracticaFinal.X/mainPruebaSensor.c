#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"
#include "InicializarAlarma.h"

#define PIN_SENSOR      7 
#define PIN_BUZZER      1

void main(void){
   
    ANSELC &= ~(1 << PIN_SENSOR);
    ANSELA |= (1 << PIN_BUZZER);
    TRISA = 0;
    TRISC = 0;
    TRISC |= 1 << PIN_SENSOR;
    LATA &= ~(1 << PIN_BUZZER);
    LATC = 0xF; 
    InicializarAlarma();
    
    uint8_t sensor; 
    
    while(1){
        sensor = (PORTC >> PIN_SENSOR)&1;
        
        if((sensor == 0)){
            LATCCLR = 1<<0; 
            Retardo(1);
            LATAINV = (1 << PIN_BUZZER);
        }else{
            LATCSET = 1<<0; 
            LATACLR = (1 << PIN_BUZZER);
        }
        
    }
}

