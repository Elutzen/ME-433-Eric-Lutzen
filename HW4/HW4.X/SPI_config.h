/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef SPI_config    /* Guard against multiple inclusion */
#define SPI_config

#include <xc.h>
#include <sys/attribs.h>

#define CS LATBbits.LATB7 //chip select = B7

unsigned char SPI_IO(unsigned char o);

void initSPI();

void setOutputVoltage(char output, unsigned char voltage);

#endif