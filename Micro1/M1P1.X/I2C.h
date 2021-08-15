/* 
 * File: ADC.h  
 * Author: Jose Alvarez
 * Comments:
 * Creado: 21 de Julio 2021
 * Revision history: 23 de julio 2021
 */

#include <xc.h> 
#include <stdint.h>

#ifndef XC_I2C_H
#define	XC_I2C_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000 //velocidad esperada a utilizar
#endif

#include <xc.h> // include processor files - each processor file is guarded.  
void MasterInit_I2C(unsigned long frec);
/*Utilizado para inicializar el master en I2C*/
void SlaveInit_I2C(uint8_t address);
/*utilizado para inicial el esclavo en I2C con las mismas condiciones que el
 maestro, de usar interrupciones es necesario colocarlas en el programa 
 principal*/
void waitCondition(void);
/*Espera a que el start bit y los bits que indican recepcion, envio,
     condiciones de parada o inicio se limpien automaticamente, es decir,
     espera a que el valor sea manipulado*/

/*Todas las funciones de abajo utilizan la waitCondition como medida
 de seguridad para esperar a que los datos sean manipulados de forma correcta*/
void MasterStart_I2C(void);
/*SDA y SCL realizan la condicion de inicio*/
void MasterStop_I2C(void);
/*SDA y SCL realizan la condicion de parada*/
void MasterRepeatS_I2C(void);
/*SDA y SCL realizan la condicion de reinicio de COM*/
void MasterSend_I2C(uint8_t dato);
/*coloca el dato en el buffer y luego lo envia*/
void MasterReceive_I2C(uint8_t *valor);
/*espera entre operaciones y coloca los valores de los bits para indicar
 que se va a recibir un valor*/
#endif	/* XC_HEADER_TEMPLATE_H */

