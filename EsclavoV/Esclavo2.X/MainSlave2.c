// Fotoresistencia y servos
// * Author: Valerie Lorraine Sofia Valdez Trujillo
// * Compilador: pic-as (v2.30), MPLABX V5.45
// * 
// * Descripci�n del programa: Comunicai�n I2C
// * 
// * Hardware: 3 PIC16F887, potenci�metros, 8 LEDS
// * 
// * Created on 28 de julio de 2021
// */

////******************************************************************************
////                           L I B R E R � A S
////******************************************************************************
#include <xc.h>
#include <stdint.h>                // Librer�a para variables de ancho definido
#include <stdio.h>                 // Para el sprintf funcione
#include <stdlib.h>
#include <string.h>                // Librer�a para concatenar

//******************************************************************************
//                      C O N F I G U R A C I � N 
//******************************************************************************

// PIC16F887 Configuration Bit Settings
#pragma config FOSC=INTRC_NOCLKOUT // Oscillador interno I/O RA6
#pragma config WDTE=OFF            // WDT disabled (reinicio rep. del pic)
#pragma config PWRTE=OFF           // Power-up Timer (PWRT disabled)
#pragma config MCLRE=OFF           // El pin de MCLR se utiliza como I/O
#pragma config CP=OFF              // Sin protecci�n de c�digo
#pragma config CPD=OFF             // Sin protecci�n de datos

#pragma config BOREN=OFF           // No reinicia cu�ndo Vin baja de 4v
#pragma config IESO=OFF            // Reinicio sin cambio de reloj inter-exter.
#pragma config FCMEN=OFF           // Cambio de reloj exter-inter en caso falla
#pragma config LVP=OFF             // Progra en bajo voltaje permitido

// CONFIG2
#pragma config BOR4V = BOR40V      // Reinicio a bajo de 4v, (BOR21V=2.1V)
#pragma config WRT = OFF           // Protecci�n de autoescritura x prog. desact.

//******************************************************************************
//             D I R E C T I V A S  del  C O M P I L A D O R
//******************************************************************************
#define _XTAL_FREQ 8000000         // Frecuencia del oscilador
#define pinTrig RB5;               // Definir las variables como nombres de los 
#define pinEcho RB4;               // pines 

//******************************************************************************
//                           V A R I A B L E S
//******************************************************************************
uint8_t dist = 0x00;               // Variable de la distancia
uint8_t cont = 0x00;               // Variable para probar la comunicaci�n serial
unsigned char z;
uint8_t VAL;                       // Variable para las fotoresistencias
uint8_t VAL2;                      // Variable para las fotoresistencias
//                      
//******************************************************************************
//                 P R O T O T I P O S  de  F U N C I O N E S
//******************************************************************************
void setup(void);                   // Configuraciones
void canales(void);                 // Switcheo de pots con servos

////******************************************************************************
////                     F U N C I � N   para   I S R
////******************************************************************************
void __interrupt() isr(void){  
    if(PIR1bits.ADIF == 1){         // INTERRUPCI�N DEL ADC
        switch(ADCON0bits.CHS){     // Asignaci�n del ADRESH a las variables
            case 0:                 // Tambi�n es un switcheo con casos
                VAL = ADRESH;       
                break;

            case 1:
                VAL2 = ADRESH; 
                break;
            }        
        PIR1bits.ADIF = 0;          // Limpiar bandera   
       }
        
    PIR1bits.TMR2IF = 0;            // Limpiar la bandera del TMR2
}


void setup(void){
 // CONFIGURACI�N DE LOS PUERTOS
    ANSEL = 0B00000011;             // Pines digitales en el puerto A
    ANSELH = 0x00;                  // Puerto B digital
    
    TRISA = 0B00000011;             // Puertos como outputs 
    TRISCbits.TRISC1 = 0;           // CCP2
    TRISCbits.TRISC2 = 0;           // CCP1      

 
    PORTA = 0X00;                   // Inicializar los puertos
    PORTB = 0X00;
    PORTC = 0X00;
    PORTD = 0X00;
    PORTE = 0X00;
    
    // Configuraci�n de los special register
    INTCONbits.GIE = 1;             // GIE Encender interrupci�n de global
    INTCONbits.PEIE = 1;            // PEIE 
 
    // Configuraci�n del oscilador
    OSCCONbits.SCS = 1;             // Utilizar el oscilador itnterno
    OSCCONbits.IRCF2 = 1;           // Oscilador de 8MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
     
    // Configuraci�n del TMR2
    PIE1bits.TMR2IE = 1;        // Habilita el TMR2 to PR2 match interrupt 
    PIR1bits.TMR2IF = 0;        // Limpiar la bandera del TMR2
    T2CON = 0X4E;               // Encender TMR2ON, Pre 1:16 y Post 1:10
    
    // Configuraciones del m�dulo ADC
    ADCON0bits.CHS = 0;         // Usar canal 0
    ADCON0bits.CHS = 1;         // Usar canal 2
    __delay_us(100);            // Delay de 100
    
    PIE1bits.ADIE = 1;          // ADIE Habilitar para comprobar FLAG -GF
    PIR1bits.ADIF = 0;          // Limpiar bandera de interrupci�n del ADC
    ADCON0bits.ADON = 1;        // Encender el m�dulo
    ADCON0bits.ADCS = 1;        // FOSC/8 
    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
    ADCON1bits.VCFG0 = 0;       // Voltaje de referencia en VSS y VDD
    ADCON1bits.VCFG1 = 0;
    
    // Configuraci�n del PWM
    PR2 = 250;                  // Per�odo del pwm 4ms
    CCP1CON = 0B00001100;       // El CCP1 se encuentra en Modo PWM 
    CCP2CON = 0B00001111;       // El CCP2 se encuentra en modoo PWM
    }
 
////******************************************************************************
////                         L O O P   P R I N C I P A L
////******************************************************************************
void main(void){  
    setup();                        // Llamar al set up       
    while (1){  
        canales();                  // Swicheo de los canales
    }
}

//******************************************************************************
//                           F U N C I O N E S 
//******************************************************************************
// Bit banging se refiere a manejar el PWM por tiempos manuales
void canales(){                // Switcheo de los canales
    if(ADCON0bits.GO == 0){
        switch(ADCON0bits.CHS){           
            case 0: 
                CCPR1L = 2*((0.247*VAL)+62);  // Funci�n para el servo
                ADCON0bits.CHS = 1;         // Canal 2
                __delay_us(100);            // Delay para activar una medici�n
                ADCON0bits.GO = 1;          // Comienza el ciclo del ADC
                break; 
                              
            case 1: 
                CCPR2L = 2*((0.247*VAL2)+62); // Funci�n para el servo     VALOR 62          
                ADCON0bits.CHS = 0;         // Canal 3
                __delay_us(100);            // Delay para activar una medici�n
                ADCON0bits.GO = 1;          // Comienza el ciclo del ADC
                break; 
                
            default:
                break;
         }
    }
}
