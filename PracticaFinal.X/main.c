#include <xc.h>
#include <stdint.h>
#include "Pic32Ini.h"
#include "InicializarAlarma.h"
#include "InterrupcionUART.h"

#define BAUDIOS                 9600
#define PIN_PULSADOR            5          
#define PIN_ALARMA_ACTIVADA     0
#define PIN_SENSOR_ACTIVADO     1
#define PIN_BUZZER              1 
#define PIN_SENSOR              7 

int main(void) {

    ANSELC &= ~(1 << PIN_SENSOR);
    ANSELA |= (1 << PIN_BUZZER);
    TRISA = 0;
    TRISB |= 1 << PIN_PULSADOR;
    TRISC = 0;
    TRISC |= 1 << PIN_SENSOR;
    LATA &= ~(1 << PIN_BUZZER);
    LATC = 0xF;

    SYSKEY = 0xAA996655; // Desbloqueamos los regs.
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

    InicializarReloj();
    InicializarAlarma();
    InicializarUART1(BAUDIOS);

    char cadena[4];
    char opcion;
    int puls_ant, puls_act;
    int t_alto = 2500;
    int error = 0;
    int res = 0;
    char s_c[0];
    int i = 0;
    int accion,alarma = 0, intruso = 0, apagado = 0;
    uint8_t sensor;
   

    enum {
        Arranque, Activada, Eleccion, Intruso, Sonido, Contrasena, Apagado
    } estado;


    INTCONbits.MVEC = 1;
    asm("   ei");

    estado = Apagado;
    puls_ant = (PORTB >> PIN_PULSADOR)&1;

    while (1) {

        puls_act = (PORTB >> PIN_PULSADOR)&1;
        if ((puls_act != puls_ant) && (puls_act == 0)) {
            t_alto += 2500;
            estado = Arranque;
            CambioLeds();
            putsUART("Por favor, introduzca la contraseña \n");
            if (t_alto > 5000) {
                t_alto = 2500;
                estado = Apagado;
            }
            OC1RS = t_alto;
        }
        puls_ant = puls_act;

        //Máquina de estados
        opcion = getcUART();
        if (opcion != '\0') {
            //Vamos asignando las letras 
            cadena[i] = opcion;
            i++;
        }
        if (cadena[i - 1] == '\n') {
            i = 0;
            switch (estado) {

                case Arranque:
                    alarma = 0;
                    apagado = 0;
                    intruso = 0;
                    res = IntroducirContrasena(cadena);
                    if (res == 0) {
                        menuOpciones();
                        estado = Eleccion;
                    } else {
                        putsUART("\nERROR\n");
                        error++;
                    }
                    //Contraseña correcta
                    if (error >= 3) {
                        intruso = 1;
                        putsUART("\nLa contraseña ha sido introducida incorrectamente tres veces\n");
                    }
                    break;

                case Eleccion:
                    
                    s_c[0] = cadena[0];
                    if (s_c[0] == '1') {
                        putsUART("Ha elegido activar la alarma \n");
                        alarma = 1;
                        estado = Arranque;
                        putsUART("Si desea desactivarla introduzca el código de acceso\n");
                    }else if (s_c[0] == '2') {
                        //Cambiar la contraseña 
                        putsUART("Ha elegido cambiar la contraseña \n");
                        putsUART("Introduzca la nueva contraseña \n");
                        LATCCLR = 1<<2;
                        estado = Contrasena;
                    }else if (s_c[0] == '3') {
                        //Apagar la alarma
                        putsUART("Ha elegido apagar la alarma \n");
                        CambioLeds();
                        apagado = 1;
                    } else {
                        putsUART("Error, opción inválida \n");
                    }

                    break;
                case Contrasena:
                    
                    accion = cambiarContrasena(cadena);
                    if(accion == 0){
                        menuOpciones();
                        LATCSET = 1<<2;
                        estado = Eleccion;
                    }
                    break;

                

            }

        }
        
        if(alarma == 1){
            
            LATCCLR = 1<<0; 
            sensor = (PORTC >> 7)&1;

                if(sensor == 0){
                    LATCCLR = 1<<1;  
                    intruso = 1;
                    putsUART("Llamando a la policia \n");
                }else{
                    LATCSET = 1<<1; 
                }    
        }else{
            LATCSET = 1<<0;
        }
        
        if(intruso == 1){
            Retardo(1);
            LATAINV = (1 << 1);
        }else{
             LATACLR = (1 << 1);
        }
        
        if(apagado == 1){
            LATC = 0xF;
            LATACLR = (1 << PIN_BUZZER);
            OC1RS = 2500;
        }


    }

}