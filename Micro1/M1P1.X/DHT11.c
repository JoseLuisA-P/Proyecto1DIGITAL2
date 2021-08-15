#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DHT11.h"
#define _XTAL_FREQ 8000000 //utilizado para los delays

#define DHT11_config TRISEbits.TRISE0 //al modificarlo oscila el valor del pin
#define DHT11_data   RE0              //se indica cual es el dato

void DHT11_START(void){
    /*En esta parte se utiliza la rutina del uC para indicarle al DHT11 que se
     desea obtener la lectura de su valor, luego de indicarlo se queda 
     como entrada esperando el valor en el pin*/
    DHT11_config = 0; //salida para indicar lectura de la temperatura
    DHT11_data = 0;   //pulso bajo
    __delay_ms(25);   //espera para que el DHT11 lo identifique
    DHT11_data = 1;   //pulso alto
    __delay_us(25);   //espera para que el DHT11 lo identifique
    DHT11_config = 1; //colocado como entrada listo para leer el dato

}

uint8_t DHT11_ALIVE(void){
    /*Luego de enviar la señal para activarlo revisa que la respuesta obtenida
     * por parte del sensor sea correcta y que no se tome mas de los 
     * tiempos estipulados
     */
    TMR1H = 0;  //reinicia el timmer 
    TMR1L = 0;
    T1CONbits.TMR1ON = 1; //enciende el timmer
    while(!DHT11_data && TMR1L < 100); //espera a que cambie el valor en el
                                        //puerto
    if(TMR1L > 99)return 0; //fallo la comunicacion
    else{
        TMR1H = 0;
        TMR1L = 0; //reinicia los valores para volver a leer
        while(DHT11_data && TMR1L < 100); //espera a que el pin tome un estado 
                                          //bajo de nuevo
        if(TMR1L > 99) return 0;            //fallo la comunicacion
        else return 1;                       //comunicacion correcta
    }
}

void DHT11_ReadData(uint8_t* datos){
    uint8_t i; //para el loop
    *datos = 0;
    for(i=0; i<8; i++){
        while(!DHT11_data); //se mantiene esperando a que cambie de 0
        TMR1H = 0;
        TMR1L = 0;
        while(DHT11_data);//revisar cuanto tiempo estuvo arriba
        if(TMR1L > 50)*datos |= (1 << (7-i));
        //coloca un 1 si la señal indica que es 1, de no ser asi el valor 
        //colocado es 0 en su posicion
    }

}