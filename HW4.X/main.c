/* 
 * File:   main.c
 * Author: elutz
 *
 * Created on April 10, 2017, 11:31 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <xc.h>
#include <sys/attribs.h>

#define CS LATBbits.LATB7 //chip select = B7

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

unsigned char SPI_IO(unsigned char o);
static int j;
void initSPI();
void setOutputVoltage(char output, unsigned char voltage);
static volatile char sineWave[200];  
static volatile char triangleWave[200];  
void waves();
void cycle();

int main() {
    initSPI();
    waves();
    j = 0;
    while(1){
        cycle();
    }
    return 0;
}

void waves(){
    int i = 0;
     for(i = 0; i < 200; i++) {
		triangleWave[i] = 255 * i / 200;
		sineWave[i] = 127.5 + 127.5 * sin(2 * 3.1415926 * i / 200);
	}  
}

void cycle(){
    
    if(_CP0_GET_COUNT() >12000) {
         _CP0_SET_COUNT(0);
        setOutputVoltage(0, sineWave[j]);
        setOutputVoltage(1, triangleWave[j]);
        j++;
    }
    if (j > 200) {
        j = 0;
    }
}


void initSPI(){
    TRISBbits.TRISB7 = 0;
    CS = 1;
    
    RPB8Rbits.RPB8R = 0b0011;// SDO1 is pin RPB8 (17)
    
    SPI1CON = 0;// turn off SPI mod
    SPI1BUF;// clear buffer
    SPI1BRG = 0x1; // set baud rate
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
   // _CP0_SET_COUNT(0);
    //while(_CP0_GET_COUNT()<1000000) {;}
    unsigned short test = 0x0;
    test = test | output << 15;
    test = test | 0x7 << 12;
    test = test | voltage << 4;
    CS = 0;
    SPI_IO((test & 0xFF00)>>8);
    SPI_IO(test & 0x00FF);
    CS = 1;
}
