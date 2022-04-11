#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "Pic32Ini.h"
#include "InterrupcionUART.h"
#include "InicializarAlarma.h"


#define TAM_COLA        500         
#define PIN_RX          13
#define PIN_TX          7

static char contrasena[100];
char codigo[4] = "3795";

typedef struct{
    uint16_t i_cabeza;
    uint16_t i_cola;
    char cola[TAM_COLA];
}cola_t;

static cola_t rx;               //Cola correspondiente al receptor
static cola_t tx;               //Cola correspondiente al emisor

void InicializarUART1(int baudios){
    rx.i_cabeza = 0;
    rx.i_cola = 0;
    tx.i_cabeza = 0;
    tx.i_cola = 0;
    
    //Calculamos UxBRG en función de los baudios introducidos
    if(baudios < 38400){
        U1BRG = 5000000/(16*baudios) + 0.5;
    }else{
        U1BRG = 5000000/(4*baudios) + 0.5;
    }
    
    //Inicializamos la interrupción
    IFS1bits.U1RXIF = 0;        //Flag a 0
    IEC1bits.U1RXIE = 1;        //Enable a 1
    IFS1bits.U1TXIF = 0;        //Flag a 0
    IPC8bits.U1IP = 3;          //Prioridad 3
    IPC8bits.U1IS = 1;          //Subprioridad 1
    
    //PINS
    ANSELB &= ~((1<<PIN_TX)|(1<<PIN_RX) | (1<<5));
       
    TRISB |= ~(1<<PIN_TX);  
    TRISB |= 1<<PIN_RX;
    TRISB |= 1<<5;
    LATB |= 1<<PIN_TX;
    
    SYSKEY = 0xAA996655; // Desbloqueamos los regs.
    SYSKEY = 0x556699AA;
    U1RXR = 3; // Asignamos los pines, Receptor a RB13 y Transmisor a RB7 (Uso de tabla, para sacar 3 y 1)
    RPB7R = 1;
    SYSKEY = 0x1CA11CA1;
 
    U1STAbits.URXISEL = 0; // Interrupcion cuando llegue 1 char
    U1STAbits.UTXISEL = 0; // Interrupcion que salte cuando se vacie la FIFO
    U1STAbits.URXEN = 1;   // Habilitamos receptor
    U1STAbits.UTXEN = 1;   // Habilitamos transmisor

    U1MODE = 0x8000; // Arrancamos la UART, solo esta el BIT ON activado (mirar tabla)
}

//Interrupcion UART
__attribute__((vector(32), interrupt(IPL3SOFT), nomips16))
    void InterrrupcionUART1(void)
    {
        //RECEPTOR
        if(IFS1bits.U1RXIF == 1 ){
            if((rx.i_cabeza+1 == rx.i_cola) || (rx.i_cabeza+1 == TAM_COLA && rx.i_cola == 0)){
                //La cola está llena
            }else{
                rx.cola[rx.i_cabeza] = U1RXREG;
                rx.i_cabeza++;
                if(rx.i_cabeza == TAM_COLA){
                    rx.i_cabeza = 0;
                }
            }
            IFS1bits.U1RXIF = 0;
        }
        
        //TRANSMISOR
        if(IFS1bits.U1TXIF == 1 ){
            if(tx.i_cola != tx.i_cabeza){
                U1TXREG = tx.cola[tx.i_cola];
                tx.i_cola++;
                if(tx.i_cola == TAM_COLA){
                    tx.i_cola = 0;
                }
            }else{
                IEC1bits.U1TXIE = 0;
            }
            IFS1bits.U1TXIF = 0;
        }
        
    }


char getcUART(void){
    char c;
    if(rx.i_cola != rx.i_cabeza){
        //La cola no está vacía
        asm("   di");
        c = rx.cola[rx.i_cola];
        rx.i_cola++;
        if(rx.i_cola == TAM_COLA){
            rx.i_cola = 0;
        }
        asm("   ei");
    }else{
        c = '\0';
    }
    return c;
    
}

void putsUART(char *s)
{
    while (*s != '\0')
    {
        if ((tx.i_cabeza + 1 == tx.i_cola) || (tx.i_cabeza + 1 == TAM_COLA && tx.i_cola == 0))
        {
            break;
        }
        else
        {
            tx.cola[tx.i_cabeza] = *s;
            s++;
            tx.i_cabeza++;
            if (tx.i_cabeza == TAM_COLA)
            {
                tx.i_cabeza = 0;
            }
        }

        IEC1bits.U1TXIE = 1;
    }
}

int charToInt(char c)
{
    if (c - 48 < 10)
        return c - 48;
    else if (c > 64 && c < 71)
        return c - 55;
    else
        return -1;
}

void menuOpciones(void)
{
    putsUART("\n== Seleccione la opción que desee ==\n");

    putsUART("1.Activar la alarma\n");
    putsUART("2.Cambiar la contraseña\n");
    putsUART("3.Apagar alarma\n");
}

int j = 0;
char opcion, cadena[4];
int selecionarOpcion()
{
    opcion = getcUART();
    
    if(opcion != '\0'){
                            //Vamos asignando las letras 
        cadena[j] = opcion;
        j++;
    }

    if (cadena[j-1]=='\n'){
        j = 0;
        if (cadena[0] == '1'){
            putsUART("Ha elegido activar la alarma \n"); 
        }else if (cadena[0] == '2'){
            //Cambiar la contraseña
            putsUART("Ha elegido cambiar la contraseña \n");    
        }else if (cadena[0] == '3'){
            //Apagar la alarma
            putsUART("Ha elegido apagar la alarma \n");     
        }else{
            putsUART("Error, opción inválida \n");  
        }
        return cadena[0];
    }
}

int i = 0;
int IntroducirContrasena(char cadena[]){
    i = 0;
    while(cadena[i] == codigo[i] && cadena[i]!= '\0' && codigo[i] != '\0'){
        i++;
    }
    
    if(i == 4){
        putsUART("La contraseña es correcta\n");
        return 0;
        
    }else{
        putsUART("La contraseña es incorrecta\n");
        return 1;
        
    }
    
    
}

char s_sub5[4];
int cambiarContrasena(char nueva[]){
    
    for (i = 0; i < 4; i++)
    {
        s_sub5[i] = nueva[i];
    }
    
    putsUART("\n");
    strcpy(codigo,s_sub5);
    putsUART("Nueva contraseña: ");
    putsUART(s_sub5);
    putsUART("\n");
 
    putsUART("Se ha cambiado correctamente\n");
    return 0;
    
}

uint8_t sensor;
void ActivarAlarma(void){
    LATCCLR = 1<<0; 
    sensor = (PORTC >> 7)&1;
        
        if(sensor == 0){
            LATCCLR = 1<<1; 
            LATAINV = (1 << 1);                   
        }else{
            LATCSET = 1<<1; 
            LATACLR = (1 << 1);
        } 
}


    
    
