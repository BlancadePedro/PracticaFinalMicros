#include <xc.h>
#include "Pic32Ini.h"
//#include "Buzzer.h"

#define PIN_BUZZER      1


static uint16_t sonido = 0;


void InicializarBuzzer(void){
     
    //Se inicializa el temporizador 
    T3CON = 0;
    TMR3 = 0;
    //TMR2<65535
    PR3 = 2499;         //0.5 ms
    
    IPC3bits.T3IP = 3;
    IPC3bits.T3IS = 0;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; //Habilitamos su interrupción
    
    T3CONbits.ON = 1;
    T3CONbits.TCKPS = 0;
    
    INTCONbits.MVEC = 1;
    asm("   ei");
}

uint16_t getSonido(void){
    uint16_t copia_sonido;
    asm("   di");
    copia_sonido = sonido;
    asm("   ei");
    return copia_sonido;
}

void setSonido(uint16_t sonidoMain){
    asm("   di");
    sonido = sonidoMain;
    asm("   ei");
}

__attribute__((vector(8), interrupt(IPL3SOFT), nomips16))
void InterrupcionBuzzer(void)
    {
        sonido = getSonido();
        IFS0bits.T2IF = 0;
        if(sonido == 0){
            LATASET = 1<<PIN_BUZZER;
        }else{
            LATACLR = 1<<PIN_BUZZER;
        }
           
    }


int main(void){
    int puls_ant, puls_act;
    puls_ant = (PORTB >> 5)&1; 
    
    ANSELACLR = 1<<1;
    TRISACLR = 1<<1;
    LATA = 0;
    
    while(1){
        puls_act = (PORTB >> 5)&1; 
        if( (puls_act != puls_ant) && (puls_act == 0) ){
            
            setSonido(0);
        }else{
            
            setSonido(1);
        }puls_ant = puls_act;
        
    }
    
}
