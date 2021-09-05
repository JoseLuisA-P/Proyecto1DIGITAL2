 /* File: Libreria C Ultas�nico 
 * Author: Valerie Lorraine Sofia Valdez Trujillo
 * Compilador: pic-as (v2.30), MPLABX V5.45
 * 
 * Descripci�n del programa: Detecci�n de dist a la que se encuentra alg�n objeto
 * Hardware: Sensor ultras�nico, leds y resistencias
 * Created on 28 de julio de 2021
 */

//******************************************************************************
//                           L I B R E R � A S
//******************************************************************************
#include <xc.h>
#include <stdint.h>                // Librer�a para variables de ancho definido
#include <stdio.h>                 // Para el sprintf funcione
#include <stdlib.h>
#include <string.h>                // Librer�a para concatenar
#include "Ultrasonicoo.h"          // Librer�a para el sensor ultras�nico

//******************************************************************************
//             D I R E C T I V A S  del  C O M P I L A D O R
//******************************************************************************
#define _XTAL_FREQ 8000000         // Frecuencia del oscilador
#define pinTrig RB5;               // Definir las variables como nombres de los 
#define pinEcho RB4;               // pines 

//******************************************************************************
//                           V A R I A B L E S
//******************************************************************************
unsigned char dist = 0x00;
 
//******************************************************************************
//                           F U N C I O N E S 
//******************************************************************************
    
int C_distancia(void){ 
    dist = 0x00;                  // Inicializar distancia
    TMR1 = 0X00;                  // Inicializar timer
    PORTBbits.RB5 = 1;            // Enviar se�al al sensor (TRIGGER)
    __delay_us(10);               // Esperar los 10us que debe el sensor
    PORTBbits.RB5 = 0;            // Apagar la se�al (TRIGGER)
    while(PORTBbits.RB4 == 0){};  // Esperar el pulso del sensor (ECHO)
    T1CONbits.TMR1ON = 1;         // Encender el modulo del timer
    while(PORTBbits.RB4 == 1){};  // Esperar a que el pulso termine (ECHO)
    T1CONbits.TMR1ON = 0;         // Apagar el timer
    dist = TMR1/58.82;            // Funci�n para obtener dist. en cm
    return dist;
}  

//int casos(void){
//    if(dist <= 4){ 
//        PORTBbits.RB1 = 1; 
//        PORTBbits.RB2 = 0;
//        __delay_ms(1);
//        }
//    if(dist >= 5){ 
//        PORTBbits.RB1 = 0;  
//        PORTBbits.RB2 = 1;
//        __delay_ms(1);
//        }
//}
