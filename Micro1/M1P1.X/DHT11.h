
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef XC_DHT11_H
#define	XC_DHT11_H

#include <xc.h> // include processor files - each processor file is guarded.  

uint8_t DHT11_ALIVE(void);
void DHT11_ReadData(uint8_t* datos);
void DHT11_START(void);

#endif	/* XC_HEADER_TEMPLATE_H */

