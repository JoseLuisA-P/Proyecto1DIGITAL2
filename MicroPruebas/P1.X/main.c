/* 
 * ESTE ES EL CODIGO DEL MICRO DE PRUEBAS
 * File:   main.c
 * Author: Jose Alvarez (19392)
 * 
 * Creado 4 de Agosto de 2021
 * 
 */
//******************************************************************************
//  Palabras de configurafcion e inclusion de librerias
//******************************************************************************
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "I2C.h"
#include "LCDD2.h"
#define _XTAL_FREQ 8000000 //utilizado para los delays

//******************************************************************************
//  Variables y prototipos de funciones
//******************************************************************************
uint8_t clean; //dummy usada para limpiar el buffer
uint8_t RDATA ,DATA; //datos que se enviaran
uint8_t contador;
unsigned char HumR[2];
unsigned char TEMPdig[6];
void config(void);
//******************************************************************************
//  Rutina de interrupcion
//******************************************************************************
void __interrupt() interrupcion(void){
    if(PIR1bits.SSPIF){
    
        SSPCONbits.CKP = 0; //mantiene el clock en 0
        //proteccion por si existe una colision
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            clean = SSPBUF; //lee el dato previo que se escribio en el buffer
            SSPCONbits.SSPOV = 0; //luego de leer indica que no hay overflow
            SSPCONbits.WCOL = 0; //si existe colision la elimina
            SSPCONbits.CKP = 1;  //habilita el clock de nuevo
        }
        
        if(!SSPSTATbits.R && !SSPSTATbits.D){ //mira si es dato y si escribe
            clean = SSPBUF; //limpia el buffer
            SSPCONbits.CKP = 1;//Habilita el clock de nuevo
            while(!SSPSTATbits.BF);//espera a que se llene el buffer
            RDATA = SSPBUF;
            switch(contador){
                case 0:
                    TEMPdig[5] = RDATA;
                    break;
                case 1:
                    TEMPdig[4] = RDATA;
                    break;
                case 2:
                    TEMPdig[3] = RDATA;
                    break;
                case 3:
                    TEMPdig[2] = RDATA;
                    break;
                case 4:
                    TEMPdig[1] = RDATA;
                    break;
                case 5:
                    TEMPdig[0] = RDATA;
                    break;
                case 6:
                    HumR[0] = RDATA;
                    break;
                case 7:
                    HumR[1] = RDATA;
                    break;
            }
            contador++;
            if(contador >= 8)contador = 0;
            __delay_ms(1);
        }
        
        else if(SSPSTATbits.R && !SSPSTATbits.D){
            clean = SSPBUF; //limpia el buffer
            SSPSTATbits.BF = 0;
            SSPBUF = DATA; //Coloca el dato en el buffer
            SSPCONbits.CKP = 1; //Habilita el clock de nuevo
            __delay_ms(1);
            while(SSPSTATbits.BF); //espera a que lo envie
        }
        
        
        PIR1bits.SSPIF = 0; 
    
    }
    
}

//******************************************************************************
//  loop principal
//******************************************************************************
void main(void) {
    config();
    initLCD();
    while(1){
        cursorLCD(1,1);
        dispCHAR(TEMPdig[5]+48);
        dispCHAR(TEMPdig[4]+48);
        dispCHAR(TEMPdig[3]+48);
        dispCHAR('.');
        dispCHAR(TEMPdig[2]+48);
        dispCHAR(TEMPdig[1]+48);
        dispCHAR(TEMPdig[0]+48);
        cursorLCD(2,1);
        dispCHAR(HumR[0]);
        dispCHAR(HumR[1]);

    }
}

//******************************************************************************
//  funciones secundarias
//******************************************************************************
void config(void){
    ANSEL =     0X00;
    ANSELH =    0X00;
    TRISA =     0X01;
    TRISB =     0X00;
    TRISD =     0X00;
    PORTA =     0X00;
    PORTB =     0X00;
    PORTD =     0X00;
    
    //Configuracion del oscilador
    OSCCONbits.IRCF = 0b111; //oscilador a 8Mhz
    OSCCONbits.SCS = 0b1;
    
    contador = 0;
    
    INTCONbits.GIE = 1; //Interrupciones usadas por el I2C y ADC
    INTCONbits.PEIE = 1;
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    PIR1bits.SSPIF = 0;
    PIE1bits.SSPIE = 1;
    
    SlaveInit_I2C(0X20);
}
