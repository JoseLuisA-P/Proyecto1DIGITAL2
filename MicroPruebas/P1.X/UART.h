
#ifndef XC_UART_H
#define	XC_UART_H

#include <xc.h> // include processor files - each processor file is guarded.  
void configUART(void);
/*configura el UART, como predeterminado usa un reloj de 8MHz*/
void send1dato(char dato);
/*envia un caracter por el UART*/
void sendString(unsigned char *mensaje);
/*envia un string por el UART*/
void sendhex(uint8_t *valor);
/*Utilizado para enviar un valor decimal en caracteres*/
#endif	/* XC_HEADER_TEMPLATE_H */

