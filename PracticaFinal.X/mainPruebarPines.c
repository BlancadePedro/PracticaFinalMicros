#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"


#include "InicializarAlarma.h"

void main(void){
    int vueltas = 0;
    ANSELC = 0;
    TRISC = 0;
    LATC = 0xF;   
    
    InicializarAlarma();
    
    while(1){
        //LATC = 0;
        //Retardo(5000);
        //LATC = 0xF;
        
        if(vueltas >= 2){
            LATC = 0xF; 
        }else{
            CambioLeds();
        }
        vueltas++;
    }
}