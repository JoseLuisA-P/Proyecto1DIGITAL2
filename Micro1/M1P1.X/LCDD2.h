
/* 
 * File: LCDD2.h   
 * Author: Jose Alvarez
 * Comments:
 * Creado: 21 de Julio 2021
 * Revision history: 23 de julio 2021
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

void initLCD(void);
/*Utilizado para inicializar la LCD. La configura en 8 bits, sin cursor, con 
 caracteres en 5X8*/
void dispCHAR(char b);
/*Despliega un caracter en la LCD, es necesario indicar primero la ubicacion */
void cursorLCD(uint8_t fila, uint8_t columna);
/*Utilizado para colocar el cursor en una posicion*/
void comandoLCD(uint8_t cmd);
/*utilizado para enviar comandos a la LCD*/
void ClearLCD(void);
/*Limpia la pantalla de la LCD*/
void LCDstring(unsigned char* mensaje);
/*Envia un STRING en la LCD*/

#endif	/* XC_HEADER_TEMPLATE_H */

