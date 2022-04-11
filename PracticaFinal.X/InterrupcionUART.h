#ifndef INTERRUPCIONUART_H
#define	INTERRUPCIONUART_H

void InicializarUART1(int baudios);
void InterrrupcionUART1(void);
char getcUART(void);
void putsUART(char *s);
int charToInt(char c);
void menuOpciones(void);
int selecionarOpcion();
int IntroducirContrasena(char cadena[]);
int cambiarContrasena(char nueva[]);
void ActivarAlarma(void);
#endif	/* INTERRUPCIONAURT_H */


