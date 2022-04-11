#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "Pic32Ini.h"
#include "InterrupcionUART.h"
#include "InicializarAlarma.h"
#define PIN_RX          13
#define PIN_TX          7

#define BAUDIOS 9600

static char contrasena[100];


int main(void) {
    //PINS
    InicializarUART1(BAUDIOS);
    InicializarReloj();
    T3CON = 0;
    TMR3 = 0;
    IFS0bits.T3IF = 0;
    PR3 = 19531; 
    
    T3CONbits.ON = 1;
    T3CONbits.TCKPS = 7;
    
    INTCONbits.MVEC = 1; 
    asm("   ei");

    char  opcion;
    char codigo[5] = "3795";
    int i = 0;
    
    int puls_ant, puls_act;
    puls_ant = (PORTB >> 5)&1; 
    
    char cadena[5] = "1111";
    while(1){
        
        puls_act = (PORTB >> 5)&1; 
        
        if( (puls_act != puls_ant) && (puls_act == 0) ){
            //menuOpciones();
            putsUART("Por favor, introduzca la contrasñea \n");
        }
        puls_ant = puls_act;
        
        opcion = getcUART();
         
        

        
            //Fin del comando, pasamos a interpretar el resultado
           
            
        strcpy(codigo,cadena);
        int stop = 1;
         
         
    }
}