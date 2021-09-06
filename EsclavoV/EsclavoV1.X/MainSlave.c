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
        //PORTD = dist;              // Probar el valor en el puerto
        
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
        
//******************************************************************************
//                           F U N C I O N E S 
//******************************************************************************   
//int C_distancia(void){ 
//    dist = 0x00;                  // Inicializar distancia
//    TMR1 = 0X00;                  // Inicializar timer
//    PORTCbits.RC2 = 1;            // Enviar señal al sensor (TRIGGER)
//    __delay_us(10);               // Esperar los 10us que debe el sensor
//    PORTCbits.RC2 = 0;            // Apagar la señal (TRIGGER)
//    while(PORTCbits.RC3 == 0){};  // Esperar el pulso del sensor (ECHO)
//    T1CONbits.TMR1ON = 1;         // Encender el modulo del timer
//    while(PORTCbits.RC3 == 1){};  // Esperar a que el pulso termine (ECHO)
//    T1CONbits.TMR1ON = 0;         // Apagar el timer
//    dist = TMR1/58.82;            // Función para obtener dist. en cm
//    return dist;
//}  





//////aqui FUNCIONAN LOS POTS SIN INCLUIR EL ULTRASONICO
////
//////aqui esta lo del MAINNNNSLAVE
////
//////* File: Libreria C Ultasónico 
////// * Author: Valerie Lorraine Sofia Valdez Trujillo
////// * Compilador: pic-as (v2.30), MPLABX V5.45
////// * 
////// * Descripción del programa: Comunicaión I2C
////// * 
////// * Hardware: 3 PIC16F887, potenciómetros, 8 LEDS
////// * 
////// * Created on 28 de julio de 2021
////// */
////
//////******************************************************************************
//////                           L I B R E R Í A S
//////******************************************************************************
//#include <xc.h>
//#include <stdint.h>                // Librería para variables de ancho definido
//#include <stdio.h>                 // Para el sprintf funcione
//#include <stdlib.h>
//#include <string.h>                // Librería para concatenar
//#include "I2C.h"                   // Librería del I2C
//#include "Ultrasonicoo.h"          // Librería para el sensor ultrasónico
//
////******************************************************************************
////                      C O N F I G U R A C I Ó N 
////******************************************************************************
//
//// PIC16F887 Configuration Bit Settings
//#pragma config FOSC=INTRC_NOCLKOUT // Oscillador interno I/O RA6
//#pragma config WDTE=OFF            // WDT disabled (reinicio rep. del pic)
//#pragma config PWRTE=OFF           // Power-up Timer (PWRT disabled)
//#pragma config MCLRE=OFF           // El pin de MCLR se utiliza como I/O
//#pragma config CP=OFF              // Sin protección de código
//#pragma config CPD=OFF             // Sin protección de datos
//
//#pragma config BOREN=OFF           // No reinicia cuándo Vin baja de 4v
//#pragma config IESO=OFF            // Reinicio sin cambio de reloj inter-exter.
//#pragma config FCMEN=OFF           // Cambio de reloj exter-inter en caso falla
//#pragma config LVP=OFF             // Progra en bajo voltaje permitido
//
//// CONFIG2
//#pragma config BOR4V = BOR40V      // Reinicio a bajo de 4v, (BOR21V=2.1V)
//#pragma config WRT = OFF           // Protección de autoescritura x prog. desact.
//
////******************************************************************************
////             D I R E C T I V A S  del  C O M P I L A D O R
////******************************************************************************
//#define _XTAL_FREQ 8000000         // Frecuencia del oscilador
//#define pinTrig RB5;               // Definir las variables como nombres de los 
//#define pinEcho RB4;               // pines 
//
////******************************************************************************
////                           V A R I A B L E S
////******************************************************************************
//uint8_t dist = 0x00;               // Variable de la distancia
//uint8_t cont = 0x00;               // Variable para probar la comunicación serial
//unsigned char z;
//uint8_t VAL;                       // Variable para las fotoresistencias
//uint8_t VAL2;                      // Variable para las fotoresistencias
////                      
////******************************************************************************
////                 P R O T O T I P O S  de  F U N C I O N E S
////******************************************************************************
//void setup(void);                   // Configuraciones
//void canales(void);                 // Switcheo de pots con servos
//
//////******************************************************************************
//////                     F U N C I Ó N   para   I S R
//////******************************************************************************
//void __interrupt() isr(void){  
//    if(PIR1bits.ADIF == 1){         //INTERRUPCIÓN DEL ADC
//        switch(ADCON0bits.CHS){     // Asignación del ADRESH a las variables
//            case 0:                 // También es un switcheo con casos
//                VAL = ADRESH;       
//                break;
//
//            case 1:
//                VAL2 = ADRESH; 
//                break;
//            }        
//        PIR1bits.ADIF = 0;          // Limpiar bandera   
//       }
//        
//    PIR1bits.TMR2IF = 0;           // Limpiar la bandera del TMR2
//
////    PIR1bits.TMR2IF = 0;            // Limpiar la bandera del TMR2
////
////  
//////     if(PIR1bits.SSPIF == 1){ 
//////        SSPCONbits.CKP = 0;
//////       
//////        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
//////            z = SSPBUF;             // Leer valor previo para limpiar buffer
//////            SSPCONbits.SSPOV = 0;   // Limpiar bandera del overflow
//////            SSPCONbits.WCOL = 0;    // Limpiar collision bit
//////            SSPCONbits.CKP = 1;     // Utilizar el SCL (Clock)
//////        }
//////
//////        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
//////            z = SSPBUF;             // Lec. del SSBUF para limpiar buffer y flag BF
//////            PIR1bits.SSPIF = 0;     // Limpiar FLAG de interr. recepción/transmisión SSP
//////            SSPCONbits.CKP = 1;     // Habilitar entrada de pulsos de reloj SCL
//////            while(!SSPSTATbits.BF); // Esperar a que la recepción se complete
//////            cont = SSPBUF;          // Guardar val. buffer de recepción en PORTD
//////            __delay_us(250);   
//////        }
//////        
//////        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
//////            z = SSPBUF;             // Variable temporal
//////            BF = 0;
//////            SSPBUF = cont;
//////            SSPCONbits.CKP = 1;
//////            __delay_us(250);
//////            while(SSPSTATbits.BF);
//////        }
//////        PIR1bits.SSPIF = 0;         // Limpiar bandera
//////    }
//}
//
//
//void setup(void){
// // CONFIGURACIÓN DE LOS PUERTOS
//    ANSEL = 0B00000011;        // Pines digitales en el puerto A
//    ANSELH = 0x00;           // Puerto B digital
//    
//    TRISA = 0B00000011;        // Puertos como outputs 
//    TRISBbits.TRISB2 = 0;          // Led 
//    TRISBbits.TRISB3 = 0;          // Led
//    TRISBbits.TRISB4 = 1;          // ECHO
//    TRISBbits.TRISB5 = 0;          // TRIGGER   
//    
//    TRISCbits.TRISC1 = 0;          // CCP2
//    TRISCbits.TRISC2 = 0;          // CCP1      
////    TRISCbits.TRISC3 = 0;        // Señal del clock SCL
////    TRISCbits.TRISC4 = 0;        // Datos seriales SDA
////TRISD = 0X00;                  // Desplegar valor del puerto
// 
//    PORTA = 0X00;                  // Inicializar los puertos
//    PORTB = 0X00;
//    PORTC = 0X00;
//    PORTD = 0X00;
//    PORTE = 0X00;
//    
//    // Configuración de los special register
//    INTCONbits.GIE = 1;            // GIE Encender interrupción de global
//    INTCONbits.PEIE = 1;           // PEIE 
//
//    
////////    // Configuración del oscilador
////////    OSCCONbits.SCS = 1;            // Utilizar el oscilador itnterno
////////    OSCCONbits.IRCF2 = 1;          // 8Mhz
////////    OSCCONbits.IRCF1 = 1; 
////////    OSCCONbits.IRCF0 = 1;
//    
//    // Configuración del oscilador
//    OSCCONbits.SCS = 1;         // Utilizar el oscilador itnterno
//    OSCCONbits.IRCF2 = 1;       // Oscilador de 8MHz
//    OSCCONbits.IRCF1 = 1;
//    OSCCONbits.IRCF0 = 1;
//    
//    // Configuración TMR1 
//    TMR1 = 0X00;                   // TMR1L y TMR1H = 0
//    T1CONbits.T1CKPS = 0B01;       // Prescaler de 1:2
//    T1CONbits.TMR1GE = 0;          // Contador siempre cuenta
//    T1CONbits.TMR1CS = 0;          // Internal clock (FOCSC/4)
//    
//    // Configuración del TMR2
//    PIE1bits.TMR2IE = 1;        // Habilita el TMR2 to PR2 match interrupt 
//    PIR1bits.TMR2IF = 0;        // Limpiar la bandera del TMR2
//    T2CON = 0X4E;               // Encender TMR2ON, Pre 1:16 y Post 1:10
//    //T2CON = 0X26;               // Encender TMR2ON, Pre 1:16 y  Post 1:5
//    
//    // Configuraciones del módulo ADC
//    ADCON0bits.CHS = 0;         // Usar canal 0
//    ADCON0bits.CHS = 1;         // Usar canal 2
//    __delay_us(100);            // Delay de 100
//    
//    PIE1bits.ADIE = 1;          // ADIE Habilitar para comprobar FLAG -GF
//    PIR1bits.ADIF = 0;          // Limpiar bandera de interrupción del ADC
//    ADCON0bits.ADON = 1;        // Encender el módulo
//    ADCON0bits.ADCS = 1;        // FOSC/8 
//    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
//    ADCON1bits.VCFG0 = 0;       // Voltaje de referencia en VSS y VDD
//    ADCON1bits.VCFG1 = 0;
//    
//    // Configuración del PWM
//    PR2 = 250;                  // Período del pwm 4ms
//    CCP1CON = 0B00001100;       // El CCP1 se encuentra en Modo PWM 
//    CCP2CON = 0B00001111;       // El CCP2 se encuentra en modoo PWM
//    
//    // Asignar esta direccion al esclavo
//    I2C_Slave_Init(0x50);
//    }
// 
//////******************************************************************************
//////                         L O O P   P R I N C I P A L
//////******************************************************************************
//void main(void){  
//    setup();                        // Llamar al set up       
//    while (1){  
//        canales();                  // Swicheo de los canales
//
//        __delay_ms(200);            // Delay para que no loqueen los servos
//        C_distancia(dist);
//         //PORTD = dist;              // Probar el valor en el puerto
//         if(dist <= 4){             // Si el objeto se encuentra a menos de 4cm
//            PORTBbits.RB3 = 1;     // Encender RB1 y apagar RB2
//            PORTBbits.RB2 = 0;
//            __delay_ms(1);
//        }
//        if(dist >= 5){             // Si el objeto se encuentra a más de 5cm
//            PORTBbits.RB3 = 0;     // Encender RB2 y apagar RB1
//            PORTBbits.RB2 = 1;
//            __delay_ms(1);
//        }
//    }
//}
//
////******************************************************************************
////                           F U N C I O N E S 
////******************************************************************************
//// Bit banging se refiere a manejar el PWM por tiempos manuales
//void canales(){                // Switcheo de los canales
//    if(ADCON0bits.GO == 0){
//        switch(ADCON0bits.CHS){           
//            case 0: 
//                CCPR1L = 2*((0.247*VAL)+62);  // Función para el servo
//                ADCON0bits.CHS = 1;         // Canal 2
//                __delay_us(100);            // Delay para activar una medición
//                ADCON0bits.GO = 1;          // Comienza el ciclo del ADC
//                break; 
//                              
//            case 1: 
//                CCPR2L = 2*((0.247*VAL2)+62); // Función para el servo     VALOR 62          
//                ADCON0bits.CHS = 0;         // Canal 3
//                __delay_us(100);            // Delay para activar una medición
//                ADCON0bits.GO = 1;          // Comienza el ciclo del ADC
//                break; 
//                
//            default:
//                break;
//         }
//    }
//}




////PROBANDO JUNTAR TODO
////EN este funciona el Ultrasonico
////
/////* File: Libreria C Ultasónico 
//// * Author: Valerie Lorraine Sofia Valdez Trujillo
//// * Compilador: pic-as (v2.30), MPLABX V5.45
//// * 
//// * Descripción del programa: Comunicaión I2C
//// * 
//// * Hardware: 3 PIC16F887, potenciómetros, 8 LEDS
//// * 
//// * Created on 28 de julio de 2021
//// */
////
////
////******************************************************************************
////                           L I B R E R Í A S
////******************************************************************************
//#include <xc.h>
//#include <stdint.h>                // Librería para variables de ancho definido
//#include <stdio.h>                 // Para el sprintf funcione
//#include <stdlib.h>
//#include <string.h>                // Librería para concatenar
//#include "I2C.h"                   // Librería del I2C
//#include "Ultrasonicoo.h"          // Librería para el sensor ultrasónico
//
////******************************************************************************
////                      C O N F I G U R A C I Ó N 
////******************************************************************************
//
//// PIC16F887 Configuration Bit Settings
//#pragma config FOSC=INTRC_NOCLKOUT // Oscillador interno I/O RA6
//#pragma config WDTE=OFF            // WDT disabled (reinicio rep. del pic)
//#pragma config PWRTE=OFF           // Power-up Timer (PWRT disabled)
//#pragma config MCLRE=OFF           // El pin de MCLR se utiliza como I/O
//#pragma config CP=OFF              // Sin protección de código
//#pragma config CPD=OFF             // Sin protección de datos
//
//#pragma config BOREN=OFF           // No reinicia cuándo Vin baja de 4v
//#pragma config IESO=OFF            // Reinicio sin cambio de reloj inter-exter.
//#pragma config FCMEN=OFF           // Cambio de reloj exter-inter en caso falla
//#pragma config LVP=OFF             // Progra en bajo voltaje permitido
//
// //CONFIG2
//#pragma config BOR4V = BOR40V      // Reinicio a bajo de 4v, (BOR21V=2.1V)
//#pragma config WRT = OFF           // Protección de autoescritura x prog. desact.
//
////******************************************************************************
////             D I R E C T I V A S  del  C O M P I L A D O R
////******************************************************************************
//#define _XTAL_FREQ 8000000         // Frecuencia del oscilador
//#define pinTrig RB5;               // Definir las variables como nombres de los 
//#define pinEcho RB4;               // pines 
//
////******************************************************************************
////                           V A R I A B L E S
////******************************************************************************
//uint8_t dist = 0x00;               // Variable de la distancia
//uint8_t cont = 0x00;               // Variable para probar la comunicación serial
//unsigned char z;
//uint8_t VAL;                       // Variable para las fotoresistencias
//uint8_t VAL2;                      // Variable para las fotoresistencias
//                      
////******************************************************************************
////                 P R O T O T I P O S  de  F U N C I O N E S
////******************************************************************************
//void setup(void);                   // Configuraciones
//void canales(void);                 // Switcheo de pots con servos
//
////******************************************************************************
////                     F U N C I Ó N   para   I S R
////******************************************************************************
//void __interrupt() isr(void){
//    if(PIR1bits.ADIF == 1){         //INTERRUPCIÓN DEL ADC
//        switch(ADCON0bits.CHS){     // Asignación del ADRESH a las variables
//            case 0:                 // También es un switcheo con casos
//                VAL = ADRESH;       
//                break;
//
//            case 1:
//                VAL2 = ADRESH; 
//                break;
//            }        
//        PIR1bits.ADIF = 0;          // Limpiar bandera   
//       }
//
//     if(PIR1bits.SSPIF == 1){ 
//        SSPCONbits.CKP = 0;
//       
//        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
//            z = SSPBUF;             // Leer valor previo para limpiar buffer
//            SSPCONbits.SSPOV = 0;   // Limpiar bandera del overflow
//            SSPCONbits.WCOL = 0;    // Limpiar collision bit
//            SSPCONbits.CKP = 1;     // Utilizar el SCL (Clock)
//        }
//
//        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
//            z = SSPBUF;             // Lec. del SSBUF para limpiar buffer y flag BF
//            PIR1bits.SSPIF = 0;     // Limpiar FLAG de interr. recepción/transmisión SSP
//            SSPCONbits.CKP = 1;     // Habilitar entrada de pulsos de reloj SCL
//            while(!SSPSTATbits.BF); // Esperar a que la recepción se complete
//            dist = SSPBUF;          // Guardar val. buffer de recepción en PORTD
//            __delay_us(250);   
//            //temp = SSPBUF;
//        }
//        
//        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
//            z = SSPBUF;             // Variable temporal
//            BF = 0;
//            SSPBUF = dist;
//            SSPCONbits.CKP = 1;
//            __delay_us(250);
//            while(SSPSTATbits.BF);
//        }
//        PIR1bits.SSPIF = 0;         // Limpiar bandera
//    }
//        PIR1bits.TMR2IF = 0;           // Limpiar la bandera del TMR2
//}
//
////******************************************************************************
////                      C O N F I G U R A C I Ó N
////******************************************************************************
//void setup(void){
//     //CONFIGURACIÓN DE LOS PUERTOS
//    ANSEL = 0B00000011;        // Pines digitales en el puerto A
//    ANSELH = 0x00;                 // Puerto B digital
//    
//    TRISBbits.TRISB0 = 1;        // Puertos como otputs
//    TRISA = 0B00000011;            // Puertos como outputs     
//    TRISBbits.TRISB2 = 0;          // Las del B son las leds
//    TRISBbits.TRISB3 = 0;  
//    TRISBbits.TRISB4 = 1;          // ECHO
//    TRISBbits.TRISB5 = 0;          // TRIGGER     
//    TRISCbits.TRISC1 = 0;          // CCP2
//    TRISCbits.TRISC2 = 0;          // CCP1   
//    TRISCbits.TRISC3 = 0;          // Señal del clock SCL
//    TRISCbits.TRISC4 = 0;          // Datos seriales SDA
//    TRISD = 0X00;                  // Desplegar valor del puerto
// 
//    PORTA = 0X00;                  // Inicializar los puertos
//    PORTB = 0X00;
//    PORTC = 0X00;
//    PORTD = 0X00;
//    PORTE = 0X00;
//
//    // Configuración del oscilador
//    OSCCONbits.SCS = 1;            // Utilizar el oscilador itnterno
//    OSCCONbits.IRCF2 = 1;          // 8Mhz
//    OSCCONbits.IRCF1 = 1; 
//    OSCCONbits.IRCF0 = 1;
//    
//     //Configuración de los special register
//    INTCONbits.GIE = 1;            // GIE Encender interrupción de global
//    INTCONbits.PEIE = 1;           // PEIE 
//
//    // Configuración TMR1 
//    TMR1 = 0X00;                   // TMR1L y TMR1H = 0
//    T1CONbits.T1CKPS = 0B01;       // Prescaler de 1:2
//    T1CONbits.TMR1GE = 0;          // Contador siempre cuenta
//    T1CONbits.TMR1CS = 0;          // Internal clock (FOCSC/4)
//     
//     //Configuración del TMR2
//    PIE1bits.TMR2IE = 1;        // Habilita el TMR2 to PR2 match interrupt 
//    PIR1bits.TMR2IF = 0;        // Limpiar la bandera del TMR2
//    T2CON = 0X4E;               // Encender TMR2ON, Pre 1:16 y Post 1:10
//    T2CON = 0X26;               // Encender TMR2ON, Pre 1:16 y  Post 1:5
//    
//     //Configuraciones del módulo ADC
//    ADCON0bits.CHS = 0;         // Usar canal 0
//    ADCON0bits.CHS = 1;         // Usar canal 2
//    __delay_us(100);            // Delay de 100
//    
//    PIE1bits.ADIE = 1;          // ADIE Habilitar para comprobar FLAG -GF
//    PIR1bits.ADIF = 0;          // Limpiar bandera de interrupción del ADC
//    ADCON0bits.ADON = 1;        // Encender el módulo
//    ADCON0bits.ADCS = 1;        // FOSC/8 
//    ADCON1bits.ADFM = 0;        // Justificado a la izquierda
//    ADCON1bits.VCFG0 = 0;       // Voltaje de referencia en VSS y VDD
//    ADCON1bits.VCFG1 = 0;
//    
//     //Configuración del PWM
//    PR2 = 250;                  // Período del pwm 4ms
//    CCP1CON = 0B00001100;       // El CCP1 se encuentra en Modo PWM 
//    CCP2CON = 0B00001111;       // El CCP2 se encuentra en modoo PWM
//    
////     Asignar esta direccion al esclavo
//    I2C_Slave_Init(0x50);
//}
//
////******************************************************************************
////                         L O O P   P R I N C I P A L
////******************************************************************************
////void main(void){  
////    setup();                       // Llamar al set up       
////    while (1){ 
////        canales();
////        __delay_ms(200);
////        C_distancia(dist);
////        
////        cont++;
////        PORTD = dist;              // Probar el valor en el puerto
////        
////        if(dist <= 4){             // Si el objeto se encuentra a menos de 4cm
////            PORTBbits.RB3 = 1;     // Encender RB1 y apagar RB2
////            PORTBbits.RB2 = 0;
////            __delay_ms(1);
////        }
////        if(dist >= 5){             // Si el objeto se encuentra a más de 5cm
////            PORTBbits.RB3 = 0;     // Encender RB2 y apagar RB1
////            PORTBbits.RB2 = 1;
////            __delay_ms(1);
////        }
////
////
////    }
////}
//void main(void){  
//    setup();
//    while (1){  
//        canales();                  // Swicheo de los canales
//
//        __delay_ms(200);            // Delay para que no loqueen los servos
//        //C_distancia(dist);
//
//    }
//} 
////******************************************************************************
////                           F U N C I O N E S 
////******************************************************************************
//void canales(){                // Switcheo de los canales
//    if(ADCON0bits.GO == 0){
//        switch(ADCON0bits.CHS){           
//            case 0: 
//                CCPR1L = 2*((0.247*VAL)+62);  // Función para el servo
//                ADCON0bits.CHS = 1;         // Canal 2
//                __delay_us(100);            // Delay para activar una medición
//                ADCON0bits.GO = 1;          // Comienza el ciclo del ADC
//                break; 
//                              
//            case 1: 
//                CCPR2L = 2*((0.247*VAL2)+62); // Función para el servo     VALOR 62          
//                ADCON0bits.CHS = 0;         // Canal 3
//                __delay_us(100);            // Delay para activar una medición
//                ADCON0bits.GO = 1;          // Comienza el ciclo del ADC
//                break; 
//                
//            default:
//                break;
//         }
//    }
//}
//
////
////int C_distancia(void){ 
////    dist = 0x00;                  // Inicializar distancia
////    TMR1 = 0X00;                  // Inicializar timer
////    PORTCbits.RC2 = 1;            // Enviar señal al sensor (TRIGGER)
////    __delay_us(10);               // Esperar los 10us que debe el sensor
////    PORTCbits.RC2 = 0;            // Apagar la señal (TRIGGER)
////    while(PORTCbits.RC3 == 0){};  // Esperar el pulso del sensor (ECHO)
////    T1CONbits.TMR1ON = 1;         // Encender el modulo del timer
////    while(PORTCbits.RC3 == 1){};  // Esperar a que el pulso termine (ECHO)
////    T1CONbits.TMR1ON = 0;         // Apagar el timer
////    dist = TMR1/58.82;            // Función para obtener dist. en cm
////    return dist;
////}  













// LOOOO DELLLLLLL MAESTRO 
///* 
// * Archivo:   main.c
// * Autor: Jose Luis Alvarez (19392)
// *
// * Creado: 15 de Agosto de 2021
// * Modificado: 22 de Agosto de 2021
// */
//
////******************************************************************************
////  Palabras de configurafcion e inclusion de librerias
////******************************************************************************
//#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
//#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
//#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
//#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
//#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
//#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
//#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
//#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
//#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
//#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
//
//// CONFIG2
//#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
//#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
//
//#include <xc.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
////#include "DHT11.h"
////#include "UART.h"
//#include "I2C.h"
////#include "LCDD2.h"
//#define _XTAL_FREQ 8000000 //utilizado para los delays
//
////******************************************************************************
////  Variables
////******************************************************************************
//uint8_t Temp1;
//uint8_t dummyT1;
//uint8_t Hum1;
//uint8_t dummyHum1;
//uint8_t CHECKSUM;
//uint8_t TempENT, TempDEC;
//uint8_t slaveAddress = 0x20;
//uint16_t temp;
//uint8_t UARTData;
//uint8_t ULTR;
//float digTemp;
//unsigned char HumR[2];
//unsigned char TEMPdig[6];
//
////******************************************************************************
////  Prototipos
////******************************************************************************
////void floToChar(const float valor, unsigned char *salida);
////void divisiondecimal(uint8_t conteo,uint8_t* un,uint8_t* dec,uint8_t* cent);
//void CONFIG(void);
//
////void __interrupt() interrupcion(void){
////    
////    if(PIR1bits.RCIF){
////        UARTData = RCREG; 
////        PIR1bits.RCIF = 0;
////    }
////}
//
////******************************************************************************
////  Funciones principales y de configuracion
////******************************************************************************
//void main(void) {
//    CONFIG();
//   // initLCD();
//    TMR1H = 0;
//    TMR1L = 0;
//    __delay_us(100);
//    
////    //configurando el LM75BD
////    MasterStart_I2C();
////    MasterSend_I2C(0X90); //direccion del sensor
////    MasterSend_I2C(0X01); //registro de configuracion
////    MasterSend_I2C(0X00); //Configuracion de interrupcion
////    MasterStop_I2C();   //se detiene la comunicacion
////    __delay_ms(5000);
//    
//    while(1){
//        // Ultrasónico
//        I2C_Master_Start();         // START para el sensor
//        I2C_Master_Write(0X51);        // Direccion pero de lectura del ultrasónico
//        ULTR = I2C_Master_Read(0);     // Lee el valor y lo mete en ULTR
//        I2C_Master_Stop();
//         __delay_ms(200);
//        
////        MasterStart_I2C();
////        MasterReceive_I2C(&TempENT); // Lee los dos bytes de temperatura, entero
////        MasterReceive_I2C(&TempDEC); // Esta es la parte decimal
////        MasterStop_I2C();
////         __delay_ms(200);
////        
////        temp = (TempENT<<3)|(TempDEC>>5); //arreglar la temperatura
////        digTemp = (float)temp*0.125; //conseguir su valor flotante
////        floToChar(digTemp,TEMPdig); //convertir el valor a 6 caracteres
////        
////        cursorLCD(1,1);
////        LCDstring("Temp: ");
////        dispCHAR(TEMPdig[5]+48);
////        dispCHAR(TEMPdig[4]+48);
////        dispCHAR(TEMPdig[3]+48);
////        dispCHAR('.');
////        dispCHAR(TEMPdig[1]+48);
////        dispCHAR(TEMPdig[2]+48);
////        dispCHAR(TEMPdig[0]+48);
////        dispCHAR(223);
////        dispCHAR('C');
////        cursorLCD(2,1);
////        LCDstring("HUM R: ");
////        dispCHAR(HumR[0]);
////        dispCHAR(HumR[1]);
////        dispCHAR('%');
////        
////        switch(UARTData){
////            case 'a':
////                PORTBbits.RB0 = 1;
////                PORTBbits.RB1 = 0;
////                PORTBbits.RB2 = 1;
////                PORTBbits.RB3 = 0;
////                break;
////            case 'b':
////                PORTBbits.RB0 = 0;
////                PORTBbits.RB1 = 1;
////                PORTBbits.RB2 = 0;
////                PORTBbits.RB3 = 1;
////                break;
////            case 'c':
////                PORTBbits.RB0 = 1;
////                PORTBbits.RB1 = 0;
////                PORTBbits.RB2 = 0;
////                PORTBbits.RB3 = 1;
////                break;
////            case 'd':
////                PORTBbits.RB0 = 0;
////                PORTBbits.RB1 = 1;
////                PORTBbits.RB2 = 1;
////                PORTBbits.RB3 = 0;
////                break;
////            case 'e':
////                PORTBbits.RB0 = 0;
////                PORTBbits.RB1 = 0;
////                PORTBbits.RB2 = 0;
////                PORTBbits.RB3 = 0;
////                break;
////        }
////        
////        DHT11_START();
////        
////        if(DHT11_ALIVE()){
////            DHT11_ReadData(&Hum1);
////            DHT11_ReadData(&dummyHum1);
////            DHT11_ReadData(&Temp1);
////            DHT11_ReadData(&dummyT1);
////            DHT11_ReadData(&CHECKSUM);
////            
////            if(CHECKSUM == ((Hum1 + dummyHum1 + Temp1 + dummyT1) & 0XFF)){
////                HumR[0] = Hum1/10 + 48;
////                HumR[1] = Hum1%10 + 48;                   
////            }
////        }
////        
////        T1CONbits.TMR1ON = 0;
////        
////        send1dato('\n');
////        send1dato(TEMPdig[5]+48);
////        send1dato(TEMPdig[4]+48);
////        send1dato(TEMPdig[3]+48);
////        send1dato('\n');
////        send1dato(HumR[0]);
////        send1dato(HumR[1]);
////        send1dato('\n');
////        __delay_ms(100);
//        PORTA = ULTR;
//    }
////
//}
//
//void CONFIG(void){
//    ANSEL =     0X00;
//    ANSELH =    0X00;
//    TRISA =     0X00;
//    TRISB =     0X00;
//    TRISD =     0X00;
//    TRISEbits.TRISE1 = 0;
//    TRISEbits.TRISE2 = 0;
//    PORTA =     0X00;
//    PORTB =     0X00;
//    PORTD =     0X00;
//    
//    //configUART();
//    
//    //Configuracion del oscilador
//    OSCCONbits.IRCF = 0b111; //oscilador a 8Mhz
//    OSCCONbits.SCS = 0b1;
//    
//    //interrupciones utilizadas
//    INTCONbits.GIE =    1;
//    INTCONbits.PEIE =   1; //Permite INT perifericas
//    PIR1bits.RCIF =     0;
//    PIE1bits.RCIE =     1; //permite interrupciones de recepcion de datos
//    
//    //Configuracion del timmer1
//    T1CON = 0X10; //periodo de 1MHz, ya que fuente es FOSC/4
//    TMR1H = 0; //valores en 0 para el conteo de los periodos de pulso
//    TMR1L = 0;  
//    I2C_Master_Init(100000);
//}
//
////void floToChar(const float valor, unsigned char *salida){
////    uint8_t entero;
////    uint16_t decimal;
////    float temp;
////    unsigned char digdecimal[3];
////    
////    entero = valor;
////    temp = valor - (float)entero;
////    decimal = (temp*100);
////    divisiondecimal(entero,&digdecimal[0],&digdecimal[1],&digdecimal[2]);
////    salida[3] = digdecimal[0];
////    salida[4] = digdecimal[1];
////    salida[5] = digdecimal[2];
////    divisiondecimal(decimal,&digdecimal[0],&digdecimal[1],&digdecimal[2]);
////    salida[2] = digdecimal[0];
////    salida[1] = digdecimal[1];
////    salida[0] = digdecimal[2];
////}
////
////void divisiondecimal(uint8_t conteo,uint8_t* un,uint8_t* dec,uint8_t* cent){
////    uint8_t div = conteo;
////    *un =   0;
////    *dec =  0;
////    *cent = 0;
////    //modifica los valores de las variables asignadas de forma inmediata
////    
////    while(div >= 100){
////    *cent = div/100;
////    div = div - (*cent)*(100);
////    }
////    
////    while (div >= 10){
////    *dec = div/10;
////    div = div - (*dec)*(10);
////    }
////    
////    *un = div;
////}