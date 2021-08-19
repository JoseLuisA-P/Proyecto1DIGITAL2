/* 
 * File: ComSerial.h  
 * Author: Jose Alvarez
 * Comments:
 * Creado: 21 de Julio 2021
 * Revision history: 23 de julio 2021
 */
#ifndef XC_HEADER_TEMPLATE_D
#define	XC_HEADER_TEMPLATE_D

#include <xc.h> // include processor files - each processor file is guarded.  

void configUART(void);
/*configura el UART, como predeterminado usa un reloj de 8MHz*/
void send1dato(char dato);
/*envia un caracter por el UART*/
void sendString(unsigned char *mensaje);
/*envia un string por el UART*/
void sendfloat(const float valor);
/*envia una variable flotante por el UART, por el momento solo valores menores
 o iguales a 9.99, despliega dos decimales unicamente*/
void division(uint8_t conteo,uint8_t* un,uint8_t* dec);
/*utilizado para determinar los dos decimales del flotante*/
void divisiondecimal(uint8_t conteo,uint8_t* un,uint8_t* dec,uint8_t* cent);
/*utilizado para determinar todas las posiciones del hexadecimal enviado*/
void sendhex(uint8_t *valor);
/*utilizado para enviar un valor hexadecimal en su forma decimal*/
void floTochar(const float valor,unsigned char *conv);
/*convierte una variable flotante en un arreglo de caracteres, con 1 entero
 y 2 decimales*/
void hexTochar(uint8_t valor,unsigned char *conv);
/*convierte un hexadecimal en un arreglo de caracteres, su valor se despliega
 en decimales usando 3 enteros*/
#endif	/* XC_HEADER_TEMPLATE_H */

