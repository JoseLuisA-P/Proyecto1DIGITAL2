//EN este funciona el Ultrasonico y el I2C

/* File: Libreria C Ultasónico 
// * Author: Valerie Lorraine Sofia Valdez Trujillo
// * Compilador: pic-as (v2.30), MPLABX V5.45
// * 
// * Descripción del programa: Comunicaión I2C
// * 
// * Hardware: 3 PIC16F887, potenciómetros, 8 LEDS
// * 
// * Created on 28 de julio de 2021
// */

//******************************************************************************
//                           L I B R E R Í A S
//******************************************************************************
#include <xc.h>
#include <stdint.h>                // Librería para variables de ancho definido
#include <stdio.h>                 // Para el sprintf funcione
#include <stdlib.h>
#include <string.h>                // Librería para concatenar
#include "I2C.h"                   // Librería del I2C
#include "Ultrasonicoo.h"          // Librería para el sensor ultrasónico

//******************************************************************************
//                      C O N F I G U R A C I Ó N 
//******************************************************************************

// PIC16F887 Configuration Bit Settings
#pragma config FOSC=INTRC_NOCLKOUT // Oscillador interno I/O RA6
#pragma config WDTE=OFF            // WDT disabled (reinicio rep. del pic)
#pragma config PWRTE=OFF           // Power-up Timer (PWRT disabled)
#pragma config MCLRE=OFF           // El pin de MCLR se utiliza como I/O
#pragma config CP=OFF              // Sin protección de código
#pragma config CPD=OFF             // Sin protección de datos

#pragma config BOREN=OFF           // No reinicia cuándo Vin baja de 4v
#pragma config IESO=OFF            // Reinicio sin cambio de reloj inter-exter.
#pragma config FCMEN=OFF           // Cambio de reloj exter-inter en caso falla
#pragma config LVP=OFF             // Progra en bajo voltaje permitido

// CONFIG2
#pragma config BOR4V = BOR40V      // Reinicio a bajo de 4v, (BOR21V=2.1V)
#pragma config WRT = OFF           // Protección de autoescritura x prog. desact.

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
unsigned char z;

//******************************************************************************
//                 P R O T O T I P O S  de  F U N C I O N E S
//******************************************************************************
void setup(void);                  // Prototipo para la configuración

//******************************************************************************
//                     F U N C I Ó N   para   I S R
//******************************************************************************
void __interrupt() isr(void){
     if(PIR1bits.SSPIF == 1){ 
        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;             // Leer valor previo para limpiar buffer
            SSPCONbits.SSPOV = 0;   // Limpiar bandera del overflow
            SSPCONbits.WCOL = 0;    // Limpiar collision bit
            SSPCONbits.CKP = 1;     // Utilizar el SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            z = SSPBUF;             // Lec. del SSBUF para limpiar buffer y flag BF
            PIR1bits.SSPIF = 0;     // Limpiar FLAG de interr. recepción/transmisión SSP
            SSPCONbits.CKP = 1;     // Habilitar entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF); // Esperar a que la recepción se complete
            dist = SSPBUF;          // Guardar val. buffer de recepción en PORTD
            __delay_us(250);   
        }
        
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;             // Variable temporal
            BF = 0;
            SSPBUF = dist;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
        PIR1bits.SSPIF = 0;         // Limpiar bandera
    }
}

//******************************************************************************
//                      C O N F I G U R A C I Ó N
//******************************************************************************
void setup(void){
    // CONFIGURACIÓN DE LOS PUERTOS
    ANSEL = 0X00;                  // Pines digitales en el puerto A
    ANSELH = 0x00000001;                 // Puerto B digital
    
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB3 = 0;          // Puertos como otputs
    TRISBbits.TRISB2 = 0;          // Las del B son las leds
    TRISBbits.TRISB4 = 1;          // ECHO
    TRISBbits.TRISB5 = 0;          // TRIGGER        
    TRISCbits.TRISC3 = 0;          // Señal del clock SCL
    TRISCbits.TRISC4 = 0;          // Datos seriales SDA
    TRISD = 0X00;                  // Desplegar valor del puerto
 
    PORTA = 0X00;                  // Inicializar los puertos
    PORTB = 0X00;
    PORTC = 0X00;
    PORTD = 0X00;
    PORTE = 0X00;
    
    // Configuración del oscilador
    OSCCONbits.SCS = 1;            // Utilizar el oscilador itnterno
    OSCCONbits.IRCF2 = 1;          // 8Mhz
    OSCCONbits.IRCF1 = 1; 
    OSCCONbits.IRCF0 = 1;
    
    // Configuración de los special register
    INTCONbits.GIE = 1;            // GIE Encender interrupción de global
    INTCONbits.PEIE = 1;           // PEIE 

    // Configuración TMR1 
    TMR1 = 0X00;                   // TMR1L y TMR1H = 0
    T1CONbits.T1CKPS = 0B01;       // Prescaler de 1:2
    T1CONbits.TMR1GE = 0;          // Contador siempre cuenta
    T1CONbits.TMR1CS = 0;          // Internal clock (FOCSC/4)
    
    // Asignar esta direccion al esclavo
    I2C_Slave_Init(0x50);
    }

//******************************************************************************
//                         L O O P   P R I N C I P A L
//******************************************************************************
void main(void){  
    setup();                       // Llamar al set up       
    while (1){  
        __delay_ms(200);
        C_distancia(dist);
        
        if(dist <= 4){             // Si el objeto se encuentra a menos de 4cm
            PORTBbits.RB3 = 1;     // Encender RB1 y apagar RB2
            PORTBbits.RB2 = 0;
            __delay_ms(1);
        }
        if(dist >= 5){             // Si el objeto se encuentra a más de 5cm
            PORTBbits.RB3 = 0;     // Encender RB2 y apagar RB1
            PORTBbits.RB2 = 1;
            __delay_ms(1);
        }
    }
}
        









