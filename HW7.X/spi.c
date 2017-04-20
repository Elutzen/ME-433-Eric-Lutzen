#include "spi.h"
#include <xc.h>

void spi_init(){
    SDI1Rbits.SDI1R = 0b0100;
    RPA1Rbits.RPA1R = 0b0011;
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB7 = 1;
    
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB7 = 0;
    LATBbits.LATB15 = 0;
    
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG;
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1;
}

unsigned char spi_IO(unsigned char o){
    
    SPI1BUF = o;
    while(!SPI1STATbits.SPIRBF){;}
    return SPI1BUF;
}