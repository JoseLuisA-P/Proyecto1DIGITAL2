#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16f887.h>
#include "LCDD2.h"
#define _XTAL_FREQ 8000000 //utilizado para los delays

//******************************************************************************
//  definicion datos usables
//******************************************************************************
#define LCDport PORTD
#define RS RE1
#define EN RE2

//******************************************************************************
//  funciones de manejo
//******************************************************************************

void initLCD(void){
    RS =    0;
    LCDport = 0X00; //formato de 8 bits
    __delay_ms(50);
    comandoLCD(0X30); //formato de 8 bits
    __delay_ms(5);
    comandoLCD(0X30); //formato de 8 bits
    __delay_ms(5);
    comandoLCD(0X30); //formato de 8 bits
    comandoLCD(0X38); //8 bits, 2 lines y formato 5x8
    comandoLCD(0X06); //incremento sin scroll
    comandoLCD(0X0C); //display on, cursor off, blink ff
    comandoLCD(0X01); //limpiar display
    
}

//******************************************************************************
//  Despliegue de datos
//******************************************************************************

void dispCHAR(char b){
    //enviar un dato en la LCD
    RS = 1;             //se envia dato
    LCDport = b;        //se coloca el dato en el puerto
    EN = 1;
    __delay_us(40);     //cambio en EN para la recepcion del dato
    EN = 0;

}

void cursorLCD(uint8_t fila, uint8_t columna){
    uint8_t temp;
    if(fila == 1){
        temp = 0X80 + columna - 1;
        comandoLCD(temp);
    }
    
    if(fila == 2){
        temp = 0XC0 + columna -1;
        comandoLCD(temp);
    } 
}

void comandoLCD(uint8_t cmd){
    RS = 0;
    LCDport = cmd;
    EN = 1;
    __delay_ms(4);     //cambio en EN para la recepcion del dato
    EN = 0;
}

void ClearLCD(void){
    comandoLCD(0X01);
}

void LCDstring(unsigned char* mensaje){ 
    while(*mensaje != 0x00){    //mira si el valor es distinto a 0
        dispCHAR(*mensaje);    //si lo es, envia el dato
        mensaje ++;             //apunta a la siguiente letra en el mensaje
    }
}