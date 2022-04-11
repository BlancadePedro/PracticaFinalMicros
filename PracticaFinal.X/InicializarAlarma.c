#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"
#include "InicializarAlarma.h"

#define PIN_LED0                0
#define PIN_LED1                1
#define PIN_LED2                2
#define PIN_LED3                3

void InicializarAlarma(void){
    T3CON = 0;
    TMR3 = 0;
    IFS0bits.T3IF = 0;
    PR3 = 19531; 
    
    T3CONbits.TCKPS = 7;
    T3CONbits.ON = 1;
    
    
}


//Inicio y fin de la alarma
void CambioLeds(void){
    IFS0bits.T3IF = 0;
    LATC = 0xF;
    
    for(int i = 0; i<4;i++){
        LATCCLR = 1<<i;
        while(IFS0bits.T3IF == 0);
        LATC = 0xF;
        IFS0bits.T3IF = 0; 
    }
}

//Función retardo
int Retardo(uint32_t retardo_s){
    
    
    for(int i = 0; i<retardo_s;i++){
       
        while(IFS0bits.T3IF == 0);
        IFS0bits.T3IF = 0; 
    }
    return 0;
}