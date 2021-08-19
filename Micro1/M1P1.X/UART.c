#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic16f887.h>
#include "UART.h"

//******************************************************************************
//  configuracion del UART
//******************************************************************************
void configUART(void){
//Configuracion del EUSART
    TRISCbits.TRISC6 = 0;   //TX como salida
    TRISCbits.TRISC7 = 1;   //RX como entrada
    SPBRG =                 12;      //12 para un baud rate de 9615
    TXSTAbits.BRGH =        0;      //baja velocidad por el reloj
    TXSTAbits.TXEN =        1;      //habilitar transmision
    RCSTAbits.CREN =        1;      //habilita la recepcion
    TXSTAbits.SYNC =        0;      //modo asincrono
    RCSTAbits.SPEN =        1;      //configura los pines como seriales
}
//******************************************************************************
//  manejo de datos
//******************************************************************************

void send1dato(char dato){ 
    TXREG = dato;   //carga el dato que se va a enviar
    while(!TXSTAbits.TRMT); //espera hasta que se envie el caracter
}

void sendString(unsigned char* mensaje){ // \r se utiliza para el salto de linea
    while(*mensaje != 0x00){    //mira si el valor es distinto a 0
        send1dato(*mensaje);    //si lo es, envia el dato
        mensaje ++;             //apunta a la siguiente letra en el mensaje
    }//como apunta a la direccion de inicio de todo el string, con cada
     //iteracion cambia al siguiente caracter del string
}

void sendhex(uint8_t *valor){
    uint8_t centena;
    uint8_t decena;
    uint8_t unidad;
    
    centena = (*valor/100);
    decena = (*valor-(centena*100))/10;
    unidad = (*valor-(centena*100))%10;
    
    send1dato(centena + 48);//envia cada valor con el arreglo para el 
    send1dato(decena + 48);//ASCII
    send1dato(unidad + 48);
}
