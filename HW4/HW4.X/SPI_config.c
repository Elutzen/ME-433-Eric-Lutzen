/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
#include "SPI_config.h"



void initSPI(){
    TRISBbits.TRISB7 = 0;
    CS = 1;
    
    RPB8Rbits.RPB8R = 0b0011;// SDO1 is pin RPB8 (17)
    
    SPI1CON = 0;// turn off SPI mod
    SPI1BUF;// clear buffer
    SPI1BRG = 11999; // set baud rate
    SPI1STATbits.SPIROV = 0; // clear overflow
    SPI1CONbits.CKE = 1; //data changes when clock goes from high to low
    SPI1CONbits.MSTEN = 1; // master operator
    SPI1CONbits.ON = 1; //turn on SPI
}

unsigned char SPI_IO(unsigned char o) {

    SPI1BUF = o;
    while (!SPI1STATbits.SPIRBF) {
        ;
    }
    return SPI1BUF;
}

void setOutputVoltage(char output, unsigned char voltage){
    
    CS = 0;
    SPI_IO((output<<15)|(0x7)|(voltage<<4));
    CS = 1;
}
